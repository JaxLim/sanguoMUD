#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
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
    QTextEdit* log_;
    QPushButton *btnN_,*btnS_,*btnW_,*btnE_,*btnTalk_,*btnAttack_,*btnSave_,*btnLoad_;
    QTimer* timer_;
    void append(const QString& s);
    void refreshHud();
};
