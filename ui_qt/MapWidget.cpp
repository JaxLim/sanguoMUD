#include "MapWidget.h"
#include <QPainter>
#include <QSizePolicy>

MapWidget::MapWidget(World* world, QWidget* parent)
    : QWidget(parent), world_(world) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MapWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    if (!world_) return;
    int w = world_->width();
    int h = world_->height();
    if (w <= 0 || h <= 0) return;
    int cellW = width() / w;
    int cellH = height() / h;
    int cell = std::min(cellW, cellH);
    if (cell <= 0) return;

    p.fillRect(rect(), Qt::white);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            QRect r(x * cell, y * cell, cell, cell);
            QColor color = world_->Walkable({x, y}) ? QColor("#f0f0f0") : QColor("#9ca3af");
            p.fillRect(r, color);
            p.drawRect(r);
        }
    }
    for (const auto& e : world_->entities()) {
        QRect r(e.pos.x * cell, e.pos.y * cell, cell, cell);
        QColor color = e.isPlayer ? QColor("#ef4444") : QColor("#3b82f6");
        p.fillRect(r, color);
        p.drawRect(r);
    }
}
