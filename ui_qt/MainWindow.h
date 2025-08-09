#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QPlainTextEdit>
#include <QPushButton>
#include "../core/World.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);

private:
    World world_;
    QPlainTextEdit* log_;
    QPushButton *btnN_,*btnS_,*btnW_,*btnE_,*btnTalk_,*btnAttack_,*btnSave_,*btnLoad_,*btnClear_;
    QTimer* timer_;
    int tick_=0;
    void append(const QString& s);
    void refreshHud();
};
