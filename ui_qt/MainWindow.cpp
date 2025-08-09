#include "MainWindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QList>
#include <QAction>
#include <QCoreApplication>
#include <QScrollBar>
#include <QStatusBar>
#include <QFontDatabase>
#include <QFile>
#include <QTextCursor>
#include <QSizePolicy>
#include "MapWidget.h"
#include "../core/Command.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    resize(1100, 720);
    setMinimumSize(1000, 640);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    auto* leftPane = new QWidget(splitter);
    leftPane->setObjectName("LeftPane");
    leftPane->setMaximumWidth(320);
    auto* leftLayout = new QVBoxLayout(leftPane);
    leftLayout->setContentsMargins(12, 20, 12, 20);
    leftLayout->setSpacing(30);

    auto* grpMove = new QGroupBox(QStringLiteral("移动"), leftPane);
    auto* g = new QGridLayout();
    g->setHorizontalSpacing(12);
    g->setVerticalSpacing(16);
    btnN_ = new QPushButton(QStringLiteral("▲"));
    btnS_ = new QPushButton(QStringLiteral("▼"));
    btnW_ = new QPushButton(QStringLiteral("◀"));
    btnE_ = new QPushButton(QStringLiteral("▶"));
    auto* btnC = new QPushButton(QStringLiteral("•"));
    for (auto btn : {btnN_, btnS_, btnW_, btnE_, btnC}) {
        btn->setFixedSize(64, 64);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    btnC->setEnabled(false);
    btnN_->setToolTip("W");
    btnS_->setToolTip("S");
    btnW_->setToolTip("A");
    btnE_->setToolTip("D");
    g->addItem(new QSpacerItem(64, 64), 0, 0);
    g->addWidget(btnN_, 0, 1);
    g->addItem(new QSpacerItem(64, 64), 0, 2);
    g->addWidget(btnW_, 1, 0);
    g->addWidget(btnC, 1, 1);
    g->addWidget(btnE_, 1, 2);
    g->addItem(new QSpacerItem(64, 64), 2, 0);
    g->addWidget(btnS_, 2, 1);
    g->addItem(new QSpacerItem(64, 64), 2, 2);
    grpMove->setLayout(g);
    leftLayout->addWidget(grpMove);

    grpInteract_ = new QGroupBox(QStringLiteral("交互"), leftPane);
    interactLayout_ = new QGridLayout();
    interactLayout_->setContentsMargins(0, 12, 0, 12);
    interactLayout_->setHorizontalSpacing(20);
    interactLayout_->setVerticalSpacing(24);
    btnChat_ = new QPushButton(QStringLiteral("聊天"));
    btnObserve_ = new QPushButton(QStringLiteral("观察"));
    btnTouch_ = new QPushButton(QStringLiteral("触摸"));
    btnAttack_ = new QPushButton(QStringLiteral("攻击"));
    btnTrade_ = new QPushButton(QStringLiteral("交易"));
    btnLeave_ = new QPushButton(QStringLiteral("离开"));
    QList<QPushButton*> ibtns{btnChat_, btnObserve_, btnTouch_, btnAttack_, btnTrade_, btnLeave_};
    for (int i = 0; i < ibtns.size(); ++i) {
        auto btn = ibtns[i];
        btn->setMinimumSize(92, 36);
        btn->setAutoDefault(false);
        btn->setDefault(false);
        btn->hide();
        interactLayout_->addWidget(btn, i / 3, i % 3);
    }
    grpInteract_->setLayout(interactLayout_);
    leftLayout->addWidget(grpInteract_);

    auto* grpSystem = new QGroupBox(QStringLiteral("系统"), leftPane);
    auto* gSystem = new QGridLayout();
    gSystem->setContentsMargins(0, 12, 0, 12);
    gSystem->setHorizontalSpacing(20);
    gSystem->setVerticalSpacing(24);
    btnInfo_ = new QPushButton();
    btnBag_ = new QPushButton();
    btnSettings_ = new QPushButton();
    btnSave_ = new QPushButton();
    btnLoad_ = new QPushButton();
    btnClear_ = new QPushButton();
    QList<QPushButton*> sysBtns{btnInfo_, btnBag_, btnSettings_, btnSave_, btnLoad_, btnClear_};
    for (auto btn : sysBtns) {
        btn->setMinimumSize(92, 36);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    btnInfo_->setText(QStringLiteral("个人信息"));
    btnBag_->setText(QStringLiteral("背包"));
    btnSettings_->setText(QStringLiteral("设置"));
    btnSave_->setText(QString::fromUtf8(reinterpret_cast<const char*>(u8"存档 (F5)")));
    btnLoad_->setText(QString::fromUtf8(reinterpret_cast<const char*>(u8"读档 (F9)")));
    btnClear_->setText(QString::fromUtf8(reinterpret_cast<const char*>(u8"清屏")));
    gSystem->addWidget(btnInfo_, 0, 0);
    gSystem->addWidget(btnBag_, 0, 1);
    gSystem->addWidget(btnSettings_, 0, 2);
    gSystem->addWidget(btnSave_, 1, 0);
    gSystem->addWidget(btnLoad_, 1, 1);
    gSystem->addWidget(btnClear_, 1, 2);
    for (int i = 0; i < 3; ++i) gSystem->setColumnStretch(i, 1);
    grpSystem->setLayout(gSystem);
    leftLayout->addWidget(grpSystem);

    auto* rightSplitter = new QSplitter(Qt::Vertical, splitter);
    map_ = new MapWidget(&world_, rightSplitter);
    log_ = new QTextEdit(rightSplitter);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);
    log_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    rightSplitter->addWidget(map_);
    rightSplitter->addWidget(log_);

    splitter->addWidget(leftPane);
    splitter->addWidget(rightSplitter);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    rightSplitter->setStretchFactor(0, 1);
    rightSplitter->setStretchFactor(1, 1);

    setStyleSheet(
        "QWidget#LeftPane { background: #f6f3ea; }"
        "QGroupBox {"
        "  border: 1px solid #dad7cf; border-radius: 10px;"
        "  margin-top: 10px; padding: 8px 10px 10px 10px;"
        "  background: rgba(255,255,255,0.55);"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; color: #6b7280; }"
        "QWidget#LeftPane QPushButton { border: 1px solid #5c4033; border-radius: 8px; padding: 6px 10px; background: #faf9f4 url(:/textures/paper.png); }"
        "QWidget#LeftPane QPushButton:hover { background-color: rgba(255,255,255,0.6); }"
        "QWidget#LeftPane QPushButton:pressed { background-color: rgba(230,230,230,0.6); }"
        "QWidget#LeftPane QPushButton:disabled { background: transparent; border: 1px dashed #d4d4d8; color: #9ca3af; }"
        "QTextEdit { background: #ffffff; selection-background-color: #e5e7eb; }"
        "QStatusBar { color: #374151; }"
    );

    auto dataPath = QCoreApplication::applicationDirPath() + "/../data";
    world_.LoadData(dataPath.toStdString());
    QFile vf(dataPath + "/_version");
    if (vf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        dataVersion_ = QString::fromUtf8(vf.readAll()).trimmed();
    }
    append(QStringLiteral("世界已加载。"), QColor("#6b7280"));
    refreshHud();

    auto actN = new QAction(this); actN->setShortcut(QKeySequence("W")); connect(actN, &QAction::triggered, this, &MainWindow::onMoveNorth); addAction(actN);
    auto actS = new QAction(this); actS->setShortcut(QKeySequence("S")); connect(actS, &QAction::triggered, this, &MainWindow::onMoveSouth); addAction(actS);
    auto actW = new QAction(this); actW->setShortcut(QKeySequence("A")); connect(actW, &QAction::triggered, this, &MainWindow::onMoveWest); addAction(actW);
    auto actE = new QAction(this); actE->setShortcut(QKeySequence("D")); connect(actE, &QAction::triggered, this, &MainWindow::onMoveEast); addAction(actE);
    auto actSave = new QAction(this); actSave->setShortcut(QKeySequence("F5")); connect(actSave, &QAction::triggered, this, &MainWindow::onSave); addAction(actSave);
    auto actLoad = new QAction(this); actLoad->setShortcut(QKeySequence("F9")); connect(actLoad, &QAction::triggered, this, &MainWindow::onLoad); addAction(actLoad);

    connect(btnN_, &QPushButton::clicked, this, &MainWindow::onMoveNorth);
    connect(btnS_, &QPushButton::clicked, this, &MainWindow::onMoveSouth);
    connect(btnW_, &QPushButton::clicked, this, &MainWindow::onMoveWest);
    connect(btnE_, &QPushButton::clicked, this, &MainWindow::onMoveEast);
    connect(map_, &MapWidget::npcClicked, this, &MainWindow::onNpcClicked);
    connect(btnChat_, &QPushButton::clicked, this, &MainWindow::onChat);
    connect(btnObserve_, &QPushButton::clicked, this, &MainWindow::onObserve);
    connect(btnTouch_, &QPushButton::clicked, this, &MainWindow::onTouch);
    connect(btnAttack_, &QPushButton::clicked, this, &MainWindow::onAttack);
    connect(btnTrade_, &QPushButton::clicked, this, &MainWindow::onTrade);
    connect(btnLeave_, &QPushButton::clicked, this, &MainWindow::onLeave);
    connect(btnInfo_, &QPushButton::clicked, this, &MainWindow::onInfo);
    connect(btnBag_, &QPushButton::clicked, this, &MainWindow::onBag);
    connect(btnSettings_, &QPushButton::clicked, this, &MainWindow::onSettings);
    connect(btnSave_, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(btnLoad_, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(btnClear_, &QPushButton::clicked, this, &MainWindow::onClear);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this] { world_.TickHours(1); ++tick_; refreshHud(); });
    timer_->start(500);
}

void MainWindow::append(const QString& s, const QColor& color) {
    QTextCursor c(log_->document());
    c.movePosition(QTextCursor::End);
    QTextCharFormat fmt;
    fmt.setForeground(color);
    c.insertText(s + "\n", fmt);
    auto* doc = log_->document();
    if (doc->blockCount() > 5000) {
        QTextCursor rc(doc);
        rc.movePosition(QTextCursor::Start);
        for (int i = 0; i < 100 && doc->blockCount() > 5000; ++i) {
            rc.select(QTextCursor::LineUnderCursor);
            rc.removeSelectedText();
            rc.deleteChar();
        }
    }
    auto* bar = log_->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::refreshHud() {
    QString msg;
    if (auto* p = world_.Find(world_.playerId())) {
        msg = QString("X:%1 Y:%2 | Tick:%3").arg(p->pos.x).arg(p->pos.y).arg(tick_);
    }
    if (!dataVersion_.isEmpty()) {
        msg += QString(" | Data v%1").arg(dataVersion_);
    }
    statusBar()->showMessage(msg);
    if (map_) map_->update();
}

void MainWindow::onMoveNorth() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "north", {}})), QColor("#6b7280"));
    refreshHud();
}

void MainWindow::onMoveSouth() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "south", {}})), QColor("#6b7280"));
    refreshHud();
}

void MainWindow::onMoveWest() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "west", {}})), QColor("#6b7280"));
    refreshHud();
}

void MainWindow::onMoveEast() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "east", {}})), QColor("#6b7280"));
    refreshHud();
}

void MainWindow::onNpcClicked(int id) {
    selectedNpc_ = id;
    for (auto btn : {btnChat_, btnObserve_, btnTouch_, btnAttack_, btnTrade_, btnLeave_}) btn->show();
}

void MainWindow::onChat() {
    if (selectedNpc_) append(QString::fromStdString(world_.Talk(world_.playerId(), selectedNpc_)), QColor("#2563eb"));
}

void MainWindow::onObserve() {
    if (selectedNpc_) append(QStringLiteral("你仔细地观察了对方。"), QColor("#2563eb"));
}

void MainWindow::onTouch() {
    if (selectedNpc_) append(QStringLiteral("你摸了摸对方。"), QColor("#2563eb"));
}

void MainWindow::onAttack() {
    if (selectedNpc_) append(QString::fromStdString(world_.Attack(world_.playerId(), selectedNpc_)), QColor("#dc2626"));
}

void MainWindow::onTrade() {
    if (selectedNpc_) append(QStringLiteral("目前没有可交易的物品。"), QColor("#2563eb"));
}

void MainWindow::onLeave() {
    clearInteraction();
}

void MainWindow::clearInteraction() {
    selectedNpc_ = 0;
    for (auto btn : {btnChat_, btnObserve_, btnTouch_, btnAttack_, btnTrade_, btnLeave_}) btn->hide();
}

void MainWindow::onSave() {
    append(QString::fromStdString(world_.Save("save1.json")), QColor("#6b7280"));
}

void MainWindow::onLoad() {
    append(QString::fromStdString(world_.Load("save1.json")), QColor("#6b7280"));
    refreshHud();
}

void MainWindow::onClear() {
    log_->clear();
}

void MainWindow::onInfo() {
    append(QStringLiteral("个人信息功能尚未实现。"), QColor("#6b7280"));
}

void MainWindow::onBag() {
    append(QStringLiteral("背包暂时为空。"), QColor("#6b7280"));
}

void MainWindow::onSettings() {
    append(QStringLiteral("设置界面暂未开放。"), QColor("#6b7280"));
}
