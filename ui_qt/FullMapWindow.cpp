#include "FullMapWindow.h"
#include <QPainter>
#include <algorithm>

FullMapWindow::FullMapWindow(World* world, QWidget* parent)
    : QWidget(parent), world_(world) {
    setWindowTitle(QStringLiteral("地图"));
    resize(400, 400);
}

void FullMapWindow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    if (!world_) return;
    int w = world_->width();
    int h = world_->height();
    if (w <= 0 || h <= 0) return;
    int cell = std::min(width() / w, height() / h);
    if (cell <= 0) return;
    int ox = (width() - cell * w) / 2;
    int oy = (height() - cell * h) / 2;
    auto* player = world_->Find(world_->playerId());
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            QRect r(ox + x * cell, oy + y * cell, cell, cell);
            bool walk = world_->Walkable({x, y});
            p.fillRect(r, walk ? Qt::white : QColor("#e5e7eb"));
            if (player && player->pos.x == x && player->pos.y == y) {
                p.fillRect(r, QColor("#facc15"));
            }
            p.setPen(QColor("#d1d5db"));
            p.drawRect(r);
        }
    }
}

