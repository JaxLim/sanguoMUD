#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QKeyEvent>
#include "../core/World.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);

protected:
    void keyPressEvent(QKeyEvent* e) override;
private:
    World world_;
    QPlainTextEdit* log_;
    QPushButton *btnN_,*btnS_,*btnW_,*btnE_,*btnTalk_,*btnAttack_,*btnSave_,*btnLoad_;
    QTimer* timer_;
    int tick_=0;
    void append(const QString& s);
    void refreshHud();
};
