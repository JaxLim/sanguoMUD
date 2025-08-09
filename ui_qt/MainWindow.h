#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QPlainTextEdit>
#include <QPushButton>
#include "../core/World.h"

class MapWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);

private:
    World world_;
    MapWidget* map_;
    QPlainTextEdit* log_;
    QPushButton *btnN_,*btnS_,*btnW_,*btnE_,*btnTalk_,*btnAttack_,*btnSave_,*btnLoad_,*btnClear_;
    QTimer* timer_;
    int tick_=0;
    QString dataVersion_;
    void append(const QString& s);
    void refreshHud();
    void onMoveNorth();
    void onMoveSouth();
    void onMoveWest();
    void onMoveEast();
    void onTalk();
    void onAttack();
    void onSave();
    void onLoad();
    void onClear();
};
