#pragma once
#include <QWidget>
#include <QRect>
#include "../core/World.h"

class MapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MapWidget(World* world, QWidget* parent=nullptr);
signals:
    void npcClicked(int id);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    World* world_;
    std::vector<std::pair<QRect, int>> npcRects_;
};
