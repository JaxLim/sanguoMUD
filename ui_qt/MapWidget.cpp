#include "MapWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QStringList>

MapWidget::MapWidget(World* world, QWidget* parent)
    : QWidget(parent), world_(world) {
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
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
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            QRect cell(ox + (dx + 1) * cellSize, oy + (dy + 1) * cellSize, cellSize, cellSize);
            Vec2 np{ player->pos.x + dx, player->pos.y + dy };
            if (!world_->Walkable(np)) continue;
            if (dx == 0 && dy == 0) {
                p.fillRect(cell, QColor("#fef9c3"));
            }
            p.setPen(QColor("#d1d5db"));
            p.drawRect(cell);
            QString name = QString::fromStdString(world_->TagName(np));
            p.setPen(Qt::black);
            p.drawText(cell, Qt::AlignCenter, name);

            bool hasNpc = false;
            for (const auto& e : world_->entities()) {
                if (e.id != player->id && e.pos.x == np.x && e.pos.y == np.y) {
                    hasNpc = true;
                    break;
                }
            }
            if (hasNpc) {
                int r = cellSize / 8;
                QPoint center = cell.topLeft() + QPoint(r*2, r*2);
                p.setBrush(QColor("#2563eb"));
                p.setPen(Qt::NoPen);
                p.drawEllipse(center, r, r);
                p.setPen(Qt::black);
                p.setBrush(Qt::NoBrush);
            }
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

void MapWidget::mouseMoveEvent(QMouseEvent* e) {
    if (!world_) { setToolTip(QString()); return; }
    const int infoH = 32;
    QRect area = rect().adjusted(0, 0, 0, -infoH);
    auto* player = world_->Find(world_->playerId());
    if (!player) { setToolTip(QString()); return; }
    int cellSize = std::min(area.width(), area.height()) / 3;
    int ox = area.center().x() - cellSize * 3 / 2;
    int oy = area.center().y() - cellSize * 3 / 2;
    int cx = (e->pos().x() - ox) / cellSize - 1;
    int cy = (e->pos().y() - oy) / cellSize - 1;
    if (cx < -1 || cx > 1 || cy < -1 || cy > 1) { setToolTip(QString()); return; }
    Vec2 np{ player->pos.x + cx, player->pos.y + cy };
    if (!world_->Walkable(np)) { setToolTip(QString()); return; }
    QString tip = QString::fromStdString(world_->TagName(np));
    QStringList names;
    for (const auto& en : world_->entities()) {
        if (en.id != player->id && en.pos.x == np.x && en.pos.y == np.y) {
            names << QString::fromStdString(en.name);
        }
    }
    if (!names.isEmpty()) tip += QStringLiteral("\nNPC: ") + names.join(", ");
    setToolTip(tip);
}
