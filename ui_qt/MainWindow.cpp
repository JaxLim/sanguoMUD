#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QCoreApplication>
#include <QKeyEvent>
#include "../core/Command.h"


MainWindow::MainWindow(QWidget* parent):QMainWindow(parent){
    auto* central = new QWidget(this);
    auto* v = new QVBoxLayout(central);
    log_ = new QTextEdit(central);
    log_->setReadOnly(true);
    log_->setFocusPolicy(Qt::NoFocus);

    auto* hMove = new QHBoxLayout();
    btnW_=new QPushButton("西"); btnN_=new QPushButton("北");
    btnS_=new QPushButton("南"); btnE_=new QPushButton("东");
    hMove->addWidget(btnW_); hMove->addWidget(btnN_); hMove->addWidget(btnS_); hMove->addWidget(btnE_);

    auto* hAct = new QHBoxLayout();
    btnTalk_=new QPushButton("对话"); btnAttack_=new QPushButton("攻击");
    btnSave_=new QPushButton("存档"); btnLoad_=new QPushButton("读档");
    hAct->addWidget(btnTalk_); hAct->addWidget(btnAttack_); hAct->addWidget(btnSave_); hAct->addWidget(btnLoad_);

    v->addWidget(log_); v->addLayout(hMove); v->addLayout(hAct);
    setCentralWidget(central);

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