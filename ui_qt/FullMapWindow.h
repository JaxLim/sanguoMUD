#pragma once
#include <QWidget>
#include "../core/World.h"

class FullMapWindow : public QWidget {
    Q_OBJECT
public:
    explicit FullMapWindow(World* world, QWidget* parent=nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    World* world_;
};

