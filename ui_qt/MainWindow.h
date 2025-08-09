#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QPushButton>
#include <QColor>
#include <QGroupBox>
#include <QGridLayout>
#include "../core/World.h"

class MapWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);

private:
    World world_;
    MapWidget* map_;
    QTextEdit* log_;
    QPushButton *btnN_,*btnS_,*btnW_,*btnE_,*btnInfo_,*btnBag_,*btnSettings_,*btnSave_,*btnLoad_,*btnClear_;
    QGroupBox* grpInteract_;
    QGridLayout* interactLayout_;
    QPushButton *btnChat_,*btnObserve_,*btnTouch_,*btnAttack_,*btnTrade_,*btnLeave_;
    EntityId selectedNpc_ = 0;
    QTimer* timer_;
    int tick_=0;
    int lastShichen_ = -1;
    QString dataVersion_;
    void append(const QString& s, const QColor& color=Qt::black);
    void refreshHud();
    void onMoveNorth();
    void onMoveSouth();
    void onMoveWest();
    void onMoveEast();
    void onNpcClicked(int id);
    void onChat();
    void onObserve();
    void onTouch();
    void onAttack();
    void onTrade();
    void onLeave();
    void clearInteraction();
    void onInfo();
    void onBag();
    void onSettings();
    void onSave();
    void onLoad();
    void onClear();
};
