#include "MainWindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QAction>
#include <QCoreApplication>
#include <QScrollBar>
#include <QStatusBar>
#include <QFontDatabase>
#include <QFile>
#include <QTextCursor>
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
    leftLayout->setContentsMargins(12, 12, 12, 12);
    leftLayout->setSpacing(10);

    auto* grpMove = new QGroupBox(QStringLiteral("移动"), leftPane);
    auto* g = new QGridLayout();
    g->setHorizontalSpacing(8);
    g->setVerticalSpacing(8);
    btnN_ = new QPushButton();
    btnS_ = new QPushButton();
    btnW_ = new QPushButton();
    btnE_ = new QPushButton();
    auto* btnC = new QPushButton();
    for (auto btn : {btnN_, btnS_, btnW_, btnE_, btnC}) {
        btn->setFixedSize(64, 64);
        btn->setIconSize(QSize(28, 28));
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    QIcon icUp(":/icons/arrow_up.svg");
    if (icUp.isNull()) btnN_->setText(QString::fromUtf8("▲")); else btnN_->setIcon(icUp);
    QIcon icDown(":/icons/arrow_down.svg");
    if (icDown.isNull()) btnS_->setText(QString::fromUtf8("▼")); else btnS_->setIcon(icDown);
    QIcon icLeft(":/icons/arrow_left.svg");
    if (icLeft.isNull()) btnW_->setText(QString::fromUtf8("◀")); else btnW_->setIcon(icLeft);
    QIcon icRight(":/icons/arrow_right.svg");
    if (icRight.isNull()) btnE_->setText(QString::fromUtf8("▶")); else btnE_->setIcon(icRight);
    QIcon icCenter(":/icons/dot_center.svg");
    if (icCenter.isNull()) btnC->setText(QString::fromUtf8("•")); else btnC->setIcon(icCenter);
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

    auto* grpInteract = new QGroupBox(QStringLiteral("交互"), leftPane);
    auto* hbInteract = new QHBoxLayout();
    hbInteract->setSpacing(8);
    btnTalk_ = new QPushButton();
    btnAttack_ = new QPushButton();
    for (auto btn : {btnTalk_, btnAttack_}) {
        btn->setMinimumSize(96, 44);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    btnTalk_->setText(QStringLiteral("对话 <span style='color:#9ca3af'>(J)</span>"));
    btnAttack_->setText(QStringLiteral("攻击 <span style='color:#9ca3af'>(K)</span>"));
    hbInteract->addWidget(btnTalk_);
    hbInteract->addWidget(btnAttack_);
    grpInteract->setLayout(hbInteract);
    leftLayout->addWidget(grpInteract);

    auto* grpSystem = new QGroupBox(QStringLiteral("系统"), leftPane);
    auto* hbSystem = new QHBoxLayout();
    hbSystem->setSpacing(8);
    btnSave_ = new QPushButton();
    btnLoad_ = new QPushButton();
    btnClear_ = new QPushButton();
    for (auto btn : {btnSave_, btnLoad_, btnClear_}) {
        btn->setMinimumSize(84, 36);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    btnSave_->setText(QStringLiteral("存档 <span style='color:#9ca3af'>(F5)</span>"));
    btnLoad_->setText(QStringLiteral("读档 <span style='color:#9ca3af'>(F9)</span>"));
    btnClear_->setText(QStringLiteral("清屏"));
    hbSystem->addWidget(btnSave_);
    hbSystem->addWidget(btnLoad_);
    hbSystem->addWidget(btnClear_);
    grpSystem->setLayout(hbSystem);
    leftLayout->addWidget(grpSystem);

    log_ = new QPlainTextEdit(splitter);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);
    log_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    splitter->addWidget(leftPane);
    splitter->addWidget(log_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    setStyleSheet(
        "QWidget#LeftPane { background: #f6f3ea; }"
        "QGroupBox {"
        "  border: 1px solid #dad7cf; border-radius: 10px;"
        "  margin-top: 10px; padding: 8px 10px 10px 10px;"
        "  background: rgba(255,255,255,0.55);"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; color: #6b7280; }"
        "QPushButton { border: 1px solid #d4d4d8; border-radius: 8px; padding: 6px 10px; }"
        "QPushButton:hover { background: #eef2ff; }"
        "QPushButton:pressed { background: #e0e7ff; }"
        "QPushButton:disabled { background: transparent; border: 1px dashed #d4d4d8; color: #9ca3af; }"
        "QPlainTextEdit { background: #ffffff; selection-background-color: #e5e7eb; }"
        "QStatusBar { color: #374151; }"
    );

    auto dataPath = QCoreApplication::applicationDirPath() + "/../data";
    world_.LoadData(dataPath.toStdString());
    QFile vf(dataPath + "/_version");
    if (vf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        dataVersion_ = QString::fromUtf8(vf.readAll()).trimmed();
    }
    append(QStringLiteral("世界已加载。"));
    refreshHud();

    auto actN = new QAction(this); actN->setShortcut(QKeySequence("W")); connect(actN, &QAction::triggered, this, &MainWindow::onMoveNorth); addAction(actN);
    auto actS = new QAction(this); actS->setShortcut(QKeySequence("S")); connect(actS, &QAction::triggered, this, &MainWindow::onMoveSouth); addAction(actS);
    auto actW = new QAction(this); actW->setShortcut(QKeySequence("A")); connect(actW, &QAction::triggered, this, &MainWindow::onMoveWest); addAction(actW);
    auto actE = new QAction(this); actE->setShortcut(QKeySequence("D")); connect(actE, &QAction::triggered, this, &MainWindow::onMoveEast); addAction(actE);
    auto actTalk = new QAction(this); actTalk->setShortcut(QKeySequence("J")); connect(actTalk, &QAction::triggered, this, &MainWindow::onTalk); addAction(actTalk);
    auto actAttack = new QAction(this); actAttack->setShortcut(QKeySequence("K")); connect(actAttack, &QAction::triggered, this, &MainWindow::onAttack); addAction(actAttack);
    auto actSave = new QAction(this); actSave->setShortcut(QKeySequence("F5")); connect(actSave, &QAction::triggered, this, &MainWindow::onSave); addAction(actSave);
    auto actLoad = new QAction(this); actLoad->setShortcut(QKeySequence("F9")); connect(actLoad, &QAction::triggered, this, &MainWindow::onLoad); addAction(actLoad);

    connect(btnN_, &QPushButton::clicked, this, &MainWindow::onMoveNorth);
    connect(btnS_, &QPushButton::clicked, this, &MainWindow::onMoveSouth);
    connect(btnW_, &QPushButton::clicked, this, &MainWindow::onMoveWest);
    connect(btnE_, &QPushButton::clicked, this, &MainWindow::onMoveEast);
    connect(btnTalk_, &QPushButton::clicked, this, &MainWindow::onTalk);
    connect(btnAttack_, &QPushButton::clicked, this, &MainWindow::onAttack);
    connect(btnSave_, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(btnLoad_, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(btnClear_, &QPushButton::clicked, this, &MainWindow::onClear);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this] { world_.TickHours(1); ++tick_; refreshHud(); });
    timer_->start(500);
}

void MainWindow::append(const QString& s) {
    log_->appendPlainText(s);
    auto* doc = log_->document();
    if (doc->blockCount() > 5000) {
        QTextCursor c(doc);
        c.movePosition(QTextCursor::Start);
        for (int i = 0; i < 100 && doc->blockCount() > 5000; ++i) {
            c.select(QTextCursor::LineUnderCursor);
            c.removeSelectedText();
            c.deleteChar();
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
}

void MainWindow::onMoveNorth() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "north", {}})));
    refreshHud();
}

void MainWindow::onMoveSouth() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "south", {}})));
    refreshHud();
}

void MainWindow::onMoveWest() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "west", {}})));
    refreshHud();
}

void MainWindow::onMoveEast() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"go", "east", {}})));
    refreshHud();
}

void MainWindow::onTalk() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"talk", "nearest", {}})));
    refreshHud();
}

void MainWindow::onAttack() {
    append(QString::fromStdString(Execute(world_, world_.playerId(), {"attack", "nearest", {}})));
    refreshHud();
}

void MainWindow::onSave() {
    append(QString::fromStdString(world_.Save("save1.json")));
}

void MainWindow::onLoad() {
    append(QString::fromStdString(world_.Load("save1.json")));
    refreshHud();
}

void MainWindow::onClear() {
    log_->clear();
}
