#include "MainWindow.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QWidget>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QFont>
#include <QScrollBar>
#include <QStatusBar>
#include "../core/Command.h"


MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    resize(1100,720);
    setMinimumSize(1000,640);

    auto* central = new QWidget(this);
    auto* h = new QHBoxLayout(central);

    auto* grpAct = new QGroupBox(QStringLiteral("行动"), central);
    auto* g = new QGridLayout();
    btnN_ = new QPushButton(QStringLiteral("北(W)"));
    btnS_ = new QPushButton(QStringLiteral("南(S)"));
    btnW_ = new QPushButton(QStringLiteral("西(A)"));
    btnE_ = new QPushButton(QStringLiteral("东(D)"));
    btnTalk_ = new QPushButton(QStringLiteral("对话(J)"));
    btnAttack_ = new QPushButton(QStringLiteral("攻击(K)"));
    btnSave_ = new QPushButton(QStringLiteral("存档(F5)"));
    btnLoad_ = new QPushButton(QStringLiteral("读档(F9)"));

    g->addWidget(btnN_,0,1);
    g->addWidget(btnW_,1,0);
    g->addWidget(btnS_,1,1);
    g->addWidget(btnE_,1,2);
    g->addWidget(btnTalk_,2,0);
    g->addWidget(btnAttack_,2,1);
    g->addWidget(btnSave_,3,0);
    g->addWidget(btnLoad_,3,1);
    grpAct->setLayout(g);

    log_ = new QPlainTextEdit(central);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);
    log_->setFont(QFont("Consolas",11));

    h->addWidget(grpAct);
    h->addWidget(log_,1);
    setCentralWidget(central);
    setStyleSheet("QPushButton{min-width:120px;min-height:48px;font-size:14px;border-radius:6px;} QPushButton:hover{background-color:#e0e0e0;}");

    btnN_->setShortcut(Qt::Key_W);
    btnS_->setShortcut(Qt::Key_S);
    btnW_->setShortcut(Qt::Key_A);
    btnE_->setShortcut(Qt::Key_D);
    btnTalk_->setShortcut(Qt::Key_J);
    btnAttack_->setShortcut(Qt::Key_K);
    btnSave_->setShortcut(Qt::Key_F5);
    btnLoad_->setShortcut(Qt::Key_F9);

    world_.LoadData((QCoreApplication::applicationDirPath()+"/../data").toStdString());
    append("世界已加载。");
    refreshHud();

    connect(btnN_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","north",{}}))); refreshHud(); });
    connect(btnS_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","south",{}}))); refreshHud(); });
    connect(btnW_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","west",{}}))); refreshHud(); });
    connect(btnE_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","east",{}}))); refreshHud(); });

    connect(btnTalk_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"talk","nearest",{}}))); refreshHud(); });
    connect(btnAttack_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"attack","nearest",{}}))); refreshHud(); });

    connect(btnSave_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(world_.Save("save1.json"))); });
    connect(btnLoad_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(world_.Load("save1.json"))); refreshHud(); });

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

void MainWindow::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
    case Qt::Key_W: btnN_->animateClick(); break;
    case Qt::Key_S: btnS_->animateClick(); break;
    case Qt::Key_A: btnW_->animateClick(); break;
    case Qt::Key_D: btnE_->animateClick(); break;
    case Qt::Key_J: btnTalk_->animateClick(); break;
    case Qt::Key_K: btnAttack_->animateClick(); break;
    case Qt::Key_F5: btnSave_->animateClick(); break;
    case Qt::Key_F9: btnLoad_->animateClick(); break;
    default:
        QMainWindow::keyPressEvent(e);
    }
}