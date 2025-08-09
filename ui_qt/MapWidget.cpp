#include "MapWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>

MapWidget::MapWidget(World* world, QWidget* parent)
    : QWidget(parent), world_(world) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MapWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::white);
    npcRects_.clear();
    if (!world_) return;

    const int infoH = 32;
    QRect area = rect().adjusted(0, 0, 0, -infoH);

    auto* player = world_->Find(world_->playerId());
    if (!player) return;

    int cellSize = std::min(area.width(), area.height()) / 3;
    int ox = area.center().x() - cellSize * 3 / 2;
    int oy = area.center().y() - cellSize * 3 / 2;

    p.setPen(QColor("#d1d5db"));
    for (int i = 0; i <= 3; ++i) {
        p.drawLine(ox, oy + i * cellSize, ox + 3 * cellSize, oy + i * cellSize);
        p.drawLine(ox + i * cellSize, oy, ox + i * cellSize, oy + 3 * cellSize);
    }

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            QRect cell(ox + (dx + 1) * cellSize, oy + (dy + 1) * cellSize, cellSize, cellSize);
            Vec2 np{ player->pos.x + dx, player->pos.y + dy };
            QString name = QString::fromStdString(world_->TagName(np));
            p.setPen(Qt::black);
            p.drawText(cell, Qt::AlignCenter, name);
        }
    }

    QStringList names;
    QList<int> ids;
    for (const auto& e : world_->entities()) {
        if (e.id != player->id && e.pos.x == player->pos.x && e.pos.y == player->pos.y) {
            names << QString::fromStdString(e.name);
            ids << e.id;
        }
    }

    int y = height() - infoH + 4;
    int x = 4;
    if (names.isEmpty()) {
        p.setPen(Qt::black);
        p.drawText(QRect(x, y, width() - 8, infoH - 4), Qt::AlignVCenter | Qt::AlignLeft,
                   QStringLiteral("这里空无一人"));
    } else {
        QFontMetrics fm(p.font());
        for (int i = 0; i < names.size(); ++i) {
            const QString& nm = names[i];
            int w = fm.horizontalAdvance(nm) + 16;
            QRect tag(x, y, w, infoH - 8);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#e5e7eb"));
            p.drawRoundedRect(tag, (infoH - 8) / 2, (infoH - 8) / 2);
            p.setPen(QColor("#374151"));
            p.drawText(tag, Qt::AlignCenter, nm);
            npcRects_.push_back({ tag, ids[i] });
            x += w + 6;
        }
    }

    p.setPen(QColor("#d1d5db"));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* e) {
    QPoint pt = e->pos();
    for (auto& pr : npcRects_) {
        if (pr.first.contains(pt)) {
            emit npcClicked(pr.second);
            break;
        }
    }
}
