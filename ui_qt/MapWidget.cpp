#include "MapWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QStringList>
#include <QPropertyAnimation>

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

    if (!hasLastPos_ || player->pos.x != lastPlayerPos_.x || player->pos.y != lastPlayerPos_.y) {
        lastPlayerPos_ = player->pos;
        hasLastPos_ = true;
        QPropertyAnimation* anim = new QPropertyAnimation(this, "highlightColor");
        anim->setDuration(300);
        anim->setStartValue(QColor("#ffffff"));
        anim->setEndValue(QColor("#fef9c3"));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    const int gap = 12;
    int cellW = (area.width() - gap * 2) / 3;
    int cellH = (area.height() - gap * 2) / 3;
    int totalW = cellW * 3 + gap * 2;
    int totalH = cellH * 3 + gap * 2;
    int ox = area.left() + (area.width() - totalW) / 2;
    int oy = area.top() + (area.height() - totalH) / 2;
    QRect cells[3][3];
    bool walkable[3][3] = {};
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int idx = dx + 1;
            int idy = dy + 1;
            QRect cell(ox + idx * (cellW + gap), oy + idy * (cellH + gap), cellW, cellH);
            cells[idx][idy] = cell;
            Vec2 np{ player->pos.x + dx, player->pos.y + dy };
            walkable[idx][idy] = world_->Walkable(np);
            if (!walkable[idx][idy]) continue;
            if (dx == 0 && dy == 0) {
                p.fillRect(cell, highlightColor_);
            }
            QPen pen(QColor("#cbd5e1"));
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(cell, 6, 6);
            QString name = QString::fromStdString(world_->TagName(np));
            p.setPen(Qt::black);
            p.drawText(cell, Qt::AlignCenter, name);
        }
    }

    QPen connPen(QColor("#d1d5db"));
    connPen.setWidth(1);
    p.setPen(connPen);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            if (!walkable[x][y]) continue;
            QRect c = cells[x][y];
            if (x < 2 && walkable[x + 1][y]) {
                int x1 = c.right();
                int x2 = cells[x + 1][y].left();
                int yy = (c.top() + c.bottom()) / 2;
                p.drawLine(QPoint(x1, yy), QPoint(x2, yy));
            }
            if (y < 2 && walkable[x][y + 1]) {
                int y1 = c.bottom();
                int y2 = cells[x][y + 1].top();
                int xx = (c.left() + c.right()) / 2;
                p.drawLine(QPoint(xx, y1), QPoint(xx, y2));
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

    // draw current time at top-right of map area
    auto hourText = [idx = world_->clock().shichen()]() -> QString {
        static const char* names[] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
        int i = (idx % 12 + 12) % 12;
        return QString::fromUtf8(names[i]) + QStringLiteral("时");
    }();
    p.setPen(QColor("#374151"));
    p.drawText(QRect(area.right() - 60, area.top(), 56, infoH), Qt::AlignRight | Qt::AlignTop, hourText);

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
    const int gap = 12;
    int cellW = (area.width() - gap * 2) / 3;
    int cellH = (area.height() - gap * 2) / 3;
    int totalW = cellW * 3 + gap * 2;
    int totalH = cellH * 3 + gap * 2;
    int ox = area.left() + (area.width() - totalW) / 2;
    int oy = area.top() + (area.height() - totalH) / 2;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            QRect cell(ox + (dx + 1) * (cellW + gap), oy + (dy + 1) * (cellH + gap), cellW, cellH);
            if (!cell.contains(e->pos())) continue;
            Vec2 np{ player->pos.x + dx, player->pos.y + dy };
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
            return;
        }
    }
    setToolTip(QString());
}
