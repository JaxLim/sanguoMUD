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

    int cellW = area.width() / 3;
    int cellH = area.height() / 3;
    int ox = area.left() + (area.width() - cellW * 3) / 2;
    int oy = area.top() + (area.height() - cellH * 3) / 2;
    QRect cells[3][3];
    bool walkable[3][3] = {};
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int idx = dx + 1;
            int idy = dy + 1;
            QRect cell(ox + idx * cellW, oy + idy * cellH, cellW, cellH);
            cells[idx][idy] = cell;
            Vec2 np{ player->pos.x + dx, player->pos.y + dy };
            walkable[idx][idy] = world_->Walkable(np);
            if (!walkable[idx][idy]) continue;
            if (dx == 0 && dy == 0) {
                p.fillRect(cell, QColor("#fef9c3"));
            }
            QPen pen(Qt::black);
            pen.setWidth(2);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(cell, cellW / 6, cellH / 6);
            QString name = QString::fromStdString(world_->TagName(np));
            p.setPen(Qt::black);
            p.drawText(cell, Qt::AlignCenter, name);
        }
    }

    QPen connPen(Qt::black);
    connPen.setWidth(2);
    p.setPen(connPen);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            if (!walkable[x][y]) continue;
            QPoint c1 = cells[x][y].center();
            if (x < 2 && walkable[x + 1][y]) p.drawLine(c1, cells[x + 1][y].center());
            if (y < 2 && walkable[x][y + 1]) p.drawLine(c1, cells[x][y + 1].center());
            if (x < 2 && y < 2 && walkable[x + 1][y + 1]) p.drawLine(c1, cells[x + 1][y + 1].center());
            if (x < 2 && y > 0 && walkable[x + 1][y - 1]) p.drawLine(c1, cells[x + 1][y - 1].center());
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
    int cellW = area.width() / 3;
    int cellH = area.height() / 3;
    int ox = area.left() + (area.width() - cellW * 3) / 2;
    int oy = area.top() + (area.height() - cellH * 3) / 2;
    int cx = (e->pos().x() - ox) / cellW - 1;
    int cy = (e->pos().y() - oy) / cellH - 1;
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
