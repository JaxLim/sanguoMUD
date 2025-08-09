#pragma once
#include <QWidget>
#include "../core/World.h"

class MapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MapWidget(World* world, QWidget* parent=nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    World* world_;
};
