/*
 * MapWidget now draws a room-style panel showing the current location and
 * exits. The center cell is highlighted; reachable directions are rendered as
 * buttons. Clicking a direction emits a corresponding signal.
 */
#include "MapWidget.h"
#include <QPainter>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStringList>

MapWidget::MapWidget(World* world, QWidget* parent)
    : QWidget(parent), world_(world) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MapWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    if (!world_) return;

    // bottom info line height
    const int infoH = 24;
    QRect area = rect().adjusted(0, 0, 0, -infoH);

    int cell = std::min(area.width(), area.height()) / 3;
    if (cell <= 0) return;
    int offX = area.left() + (area.width() - cell * 3) / 2;
    int offY = area.top() + (area.height() - cell * 3) / 2;

    rectC_ = QRect(offX + cell, offY + cell, cell, cell);
    rectN_ = QRect(offX + cell, offY, cell, cell);
    rectS_ = QRect(offX + cell, offY + 2 * cell, cell, cell);
    rectW_ = QRect(offX, offY + cell, cell, cell);
    rectE_ = QRect(offX + 2 * cell, offY + cell, cell, cell);

    auto* player = world_->Find(world_->playerId());
    if (!player) return;

    QColor highlight("#3b82f6");
    p.fillRect(rectC_, QColor("#fef3c7"));
    p.setPen(highlight);
    p.drawRect(rectC_);

    QString name = QString("%1,%2").arg(player->pos.x).arg(player->pos.y);
    p.drawText(rectC_, Qt::AlignCenter, name);

    struct Dir { QRect* r; Vec2 d; const char* text; };
    Dir dirs[4] = {
        { &rectN_, {0,-1}, u8"北" },
        { &rectS_, {0,1},  u8"南" },
        { &rectW_, {-1,0}, u8"西" },
        { &rectE_, {1,0},  u8"东" }
    };

    for (auto& d : dirs) {
        Vec2 np{ player->pos.x + d.d.x, player->pos.y + d.d.y };
        if (world_->Walkable(np)) {
            p.setPen(Qt::black);
            p.drawRect(*d.r);
            p.drawText(*d.r, Qt::AlignCenter, QString::fromUtf8(d.text));
        } else {
            p.fillRect(*d.r, Qt::black);
            p.setPen(Qt::black);
            p.drawRect(*d.r);
        }
    }

    // bottom info
    QStringList names;
    for (const auto& e : world_->entities()) {
        if (e.id != player->id && e.pos.x == player->pos.x && e.pos.y == player->pos.y) {
            names << QString::fromStdString(e.name);
        }
    }
    QString info = names.isEmpty() ? QStringLiteral("这里空无一人")
                                   : QStringLiteral("这里有") + names.join(QStringLiteral("、"));
    p.setPen(Qt::black);
    p.drawText(QRect(4, height() - infoH, width() - 8, infoH), Qt::AlignVCenter | Qt::AlignLeft, info);

    // bottom separator line
    p.setPen(QColor("#d1d5db"));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* e) {
    QPoint pt = e->pos();
    if (rectN_.contains(pt)) emit moveNorth();
    else if (rectS_.contains(pt)) emit moveSouth();
    else if (rectW_.contains(pt)) emit moveWest();
    else if (rectE_.contains(pt)) emit moveEast();
}
