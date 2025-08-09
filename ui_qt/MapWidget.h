#pragma once
#include <QWidget>
#include <QRect>
#include "../core/World.h"

class MapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MapWidget(World* world, QWidget* parent=nullptr);
signals:
    void moveNorth();
    void moveSouth();
    void moveWest();
    void moveEast();
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    World* world_;
    QRect rectN_, rectS_, rectW_, rectE_, rectC_;
    bool canN_=false, canS_=false, canW_=false, canE_=false;
};
