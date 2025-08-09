#include "MainWindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QStyle>
#include <QAction>
#include <QCoreApplication>
#include <QFont>
#include <QScrollBar>
#include <QStatusBar>
#include "../core/Command.h"

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    resize(1100,720);
    setMinimumSize(1000,640);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    auto* leftPane = new QWidget(splitter);
    leftPane->setMaximumWidth(320);
    auto* leftLayout = new QVBoxLayout(leftPane);
    leftLayout->setContentsMargins(12,12,12,12);
    leftLayout->setSpacing(8);

    auto* grpMove = new QGroupBox(QStringLiteral("移动"), leftPane);
    auto* g = new QGridLayout();
    btnN_ = new QPushButton();
    btnS_ = new QPushButton();
    btnW_ = new QPushButton();
    btnE_ = new QPushButton();
    for(auto btn : {btnN_,btnS_,btnW_,btnE_}){
        btn->setFixedSize(64,64);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    btnN_->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    btnS_->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    btnW_->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    btnE_->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    btnN_->setToolTip(QStringLiteral("热键: W"));
    btnS_->setToolTip(QStringLiteral("热键: S"));
    btnW_->setToolTip(QStringLiteral("热键: A"));
    btnE_->setToolTip(QStringLiteral("热键: D"));
    g->addWidget(btnN_,0,1);
    g->addWidget(btnW_,1,0);
    g->addWidget(btnE_,1,2);
    g->addWidget(btnS_,2,1);
    grpMove->setLayout(g);
    leftLayout->addWidget(grpMove);

    auto* grpInteract = new QGroupBox(QStringLiteral("交互"), leftPane);
    auto* hbInteract = new QHBoxLayout();
    btnTalk_ = new QPushButton(QStringLiteral("对话 J"));
    btnAttack_ = new QPushButton(QStringLiteral("攻击 K"));
    for(auto btn : {btnTalk_,btnAttack_}){
        btn->setMinimumSize(96,44);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    hbInteract->addWidget(btnTalk_);
    hbInteract->addWidget(btnAttack_);
    grpInteract->setLayout(hbInteract);
    leftLayout->addWidget(grpInteract);

    auto* grpSystem = new QGroupBox(QStringLiteral("系统"), leftPane);
    auto* hbSystem = new QHBoxLayout();
    btnSave_ = new QPushButton(QStringLiteral("存档 F5"));
    btnLoad_ = new QPushButton(QStringLiteral("读档 F9"));
    btnClear_ = new QPushButton(QStringLiteral("清屏"));
    for(auto btn : {btnSave_,btnLoad_,btnClear_}){
        btn->setMinimumSize(84,36);
        btn->setAutoDefault(false);
        btn->setDefault(false);
    }
    hbSystem->addWidget(btnSave_);
    hbSystem->addWidget(btnLoad_);
    hbSystem->addWidget(btnClear_);
    grpSystem->setLayout(hbSystem);
    leftLayout->addWidget(grpSystem);

    log_ = new QPlainTextEdit(splitter);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);
    log_->setFont(QFont("Consolas",11));

    splitter->addWidget(leftPane);
    splitter->addWidget(log_);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);

    setStyleSheet(
        "QGroupBox { border: 1px solid #d9d9e3; border-radius: 8px; margin-top: 8px; padding: 8px 8px 4px 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #555; }"
        "QPushButton { border-radius: 6px; padding: 6px 10px; }"
        "QPushButton:hover { background: #f2f3f7; }"
        "QPushButton:pressed { background: #e6e8ef; }"
    );

    world_.LoadData((QCoreApplication::applicationDirPath()+"/../data").toStdString());
    append("世界已加载。");
    refreshHud();

    auto moveNorth = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","north",{}}))); refreshHud(); };
    auto moveSouth = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","south",{}}))); refreshHud(); };
    auto moveWest  = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","west",{}}))); refreshHud(); };
    auto moveEast  = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","east",{}}))); refreshHud(); };
    auto talk      = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"talk","nearest",{}}))); refreshHud(); };
    auto attack    = [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"attack","nearest",{}}))); refreshHud(); };
    auto save      = [this]{ append(QString::fromStdString(world_.Save("save1.json"))); };
    auto load      = [this]{ append(QString::fromStdString(world_.Load("save1.json"))); refreshHud(); };
    auto clearLog  = [this]{ log_->clear(); };

    connect(btnN_, &QPushButton::clicked, this, moveNorth);
    connect(btnS_, &QPushButton::clicked, this, moveSouth);
    connect(btnW_, &QPushButton::clicked, this, moveWest);
    connect(btnE_, &QPushButton::clicked, this, moveEast);
    connect(btnTalk_, &QPushButton::clicked, this, talk);
    connect(btnAttack_, &QPushButton::clicked, this, attack);
    connect(btnSave_, &QPushButton::clicked, this, save);
    connect(btnLoad_, &QPushButton::clicked, this, load);
    connect(btnClear_, &QPushButton::clicked, this, clearLog);

    auto* actN = new QAction(this); actN->setShortcut(Qt::Key_W); connect(actN, &QAction::triggered, this, moveNorth);
    auto* actS = new QAction(this); actS->setShortcut(Qt::Key_S); connect(actS, &QAction::triggered, this, moveSouth);
    auto* actW = new QAction(this); actW->setShortcut(Qt::Key_A); connect(actW, &QAction::triggered, this, moveWest);
    auto* actE = new QAction(this); actE->setShortcut(Qt::Key_D); connect(actE, &QAction::triggered, this, moveEast);
    auto* actTalk = new QAction(this); actTalk->setShortcut(Qt::Key_J); connect(actTalk, &QAction::triggered, this, talk);
    auto* actAttack = new QAction(this); actAttack->setShortcut(Qt::Key_K); connect(actAttack, &QAction::triggered, this, attack);
    auto* actSave = new QAction(this); actSave->setShortcut(Qt::Key_F5); connect(actSave, &QAction::triggered, this, save);
    auto* actLoad = new QAction(this); actLoad->setShortcut(Qt::Key_F9); connect(actLoad, &QAction::triggered, this, load);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this]{ world_.TickHours(1); ++tick_; refreshHud(); });
    timer_->start(500);
}

void MainWindow::append(const QString& s){
    log_->appendPlainText(s);
    auto* bar = log_->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::refreshHud(){
    if(auto* p = world_.Find(world_.playerId())){
        statusBar()->showMessage(QString("X:%1 Y:%2 | Tick:%3").arg(p->pos.x).arg(p->pos.y).arg(tick_));
    }
}

