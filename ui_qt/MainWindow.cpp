#include "MainWindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QWidget>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QFont>
#include "../core/Command.h"


MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    resize(1280,960);

    auto* central = new QWidget(this);
    auto* v = new QVBoxLayout(central);
    log_ = new QTextEdit(central);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);
    log_->setFont(QFont("Consolas",11));

    auto* grpMove = new QGroupBox(QStringLiteral("移动"), central);
    auto* gMove = new QGridLayout();
    btnN_=new QPushButton(QStringLiteral("北(W)"));
    btnS_=new QPushButton(QStringLiteral("南(S)"));
    btnW_=new QPushButton(QStringLiteral("西(A)"));
    btnE_=new QPushButton(QStringLiteral("东(D)"));
    gMove->addWidget(btnN_,0,1);
    gMove->addWidget(btnW_,1,0);
    gMove->addWidget(btnE_,1,2);
    gMove->addWidget(btnS_,2,1);
    grpMove->setLayout(gMove);

    auto* grpAct = new QGroupBox(QStringLiteral("动作"), central);
    auto* gAct = new QGridLayout();
    btnTalk_=new QPushButton(QStringLiteral("对话(Enter)"));
    btnAttack_=new QPushButton(QStringLiteral("攻击(Space)"));
    btnSave_=new QPushButton(QStringLiteral("存档"));
    btnLoad_=new QPushButton(QStringLiteral("读档"));
    gAct->addWidget(btnTalk_,0,0);
    gAct->addWidget(btnAttack_,0,1);
    gAct->addWidget(btnSave_,1,0);
    gAct->addWidget(btnLoad_,1,1);
    grpAct->setLayout(gAct);

    v->addWidget(log_);
    v->addWidget(grpMove);
    v->addWidget(grpAct);
    setCentralWidget(central);
    setStyleSheet("QPushButton{min-width:120px;min-height:48px;font-size:14px;border-radius:6px;} QPushButton:hover{background-color:#e0e0e0;}");

    world_.LoadData((QCoreApplication::applicationDirPath()+"/../data").toStdString());
    append("世界已加载。");

    connect(btnN_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","north",{}}))); });
    connect(btnS_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","south",{}}))); });
    connect(btnW_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","west",{}}))); });
    connect(btnE_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"go","east",{}}))); });

    connect(btnTalk_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"talk","nearest",{}}))); });
    connect(btnAttack_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(Execute(world_, world_.playerId(), {"attack","nearest",{}}))); });

    connect(btnSave_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(world_.Save("save1.json"))); });
    connect(btnLoad_, &QPushButton::clicked, this, [this]{ append(QString::fromStdString(world_.Load("save1.json"))); });

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this]{ world_.TickHours(1); });
    timer_->start(500);
}

void MainWindow::append(const QString& s){ log_->append(s); }
void MainWindow::refreshHud(){}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
    case Qt::Key_W: append(QString::fromStdString(Execute(world_, world_.playerId(), { "go","north",{} }))); break;
    case Qt::Key_S: append(QString::fromStdString(Execute(world_, world_.playerId(), { "go","south",{} }))); break;
    case Qt::Key_A: append(QString::fromStdString(Execute(world_, world_.playerId(), { "go","west",{} })));  break;
    case Qt::Key_D: append(QString::fromStdString(Execute(world_, world_.playerId(), { "go","east",{} })));  break;
    case Qt::Key_Space: append(QString::fromStdString(Execute(world_, world_.playerId(), { "attack","nearest",{} }))); break;
    case Qt::Key_Return:
    case Qt::Key_Enter: append(QString::fromStdString(Execute(world_, world_.playerId(), { "talk","nearest",{} }))); break;
    default:
        QMainWindow::keyPressEvent(e);
    }
}