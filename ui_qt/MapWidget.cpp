/*
 * MapWidget now renders a central card for the current location with
 * capsule-style buttons around it indicating walkable directions and
 * destination coordinates. Unreachable directions are greyed out and
 * disabled. A row of NPC tags is displayed at the bottom.
 */
#include "MapWidget.h"
#include <QPainter>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStringList>
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
    if (!world_) return;

    // bottom tag row height
    const int infoH = 32;
    QRect area = rect().adjusted(0, 0, 0, -infoH);

    auto* player = world_->Find(world_->playerId());
    if (!player) return;

    // geometry for center card and direction buttons
    int cardW = std::min(160, area.width() / 2);
    int cardH = std::min(100, area.height() / 2);
    QPoint c = area.center();
    rectC_ = QRect(c.x() - cardW / 2, c.y() - cardH / 2, cardW, cardH);

    int btnW = 120;
    int btnH = 32;
    int gap = 20;
    rectN_ = QRect(c.x() - btnW / 2, rectC_.top() - btnH - gap, btnW, btnH);
    rectS_ = QRect(c.x() - btnW / 2, rectC_.bottom() + gap, btnW, btnH);
    rectW_ = QRect(rectC_.left() - btnW - gap, c.y() - btnH / 2, btnW, btnH);
    rectE_ = QRect(rectC_.right() + gap, c.y() - btnH / 2, btnW, btnH);

    // current location card
    p.setPen(QColor("#3b82f6"));
    p.setBrush(Qt::white);
    p.drawRoundedRect(rectC_, 8, 8);
    QString name = QString("%1,%2").arg(player->pos.x).arg(player->pos.y);
    p.setPen(Qt::black);
    p.drawText(rectC_, Qt::AlignCenter, name);

    struct Dir { QRect* r; Vec2 d; const char* text; bool* flag; };
    Dir dirs[4] = {
        { &rectN_, {0,-1}, "北", &canN_ },
        { &rectS_, {0,1},  "南", &canS_ },
        { &rectW_, {-1,0}, "西", &canW_ },
        { &rectE_, {1,0},  "东", &canE_ }
    };

    for (auto& d : dirs) {
        Vec2 np{ player->pos.x + d.d.x, player->pos.y + d.d.y };
        QString loc = QString("%1,%2").arg(np.x).arg(np.y);
        bool can = world_->Walkable(np);
        *d.flag = can;
        if (can) {
            p.setPen(QColor("#3b82f6"));
            p.setBrush(QColor("#e0e7ff"));
        } else {
            p.setPen(QColor("#9ca3af"));
            p.setBrush(QColor("#f3f4f6"));
        }
        p.drawRoundedRect(*d.r, btnH / 2, btnH / 2);
        p.setPen(can ? QColor("#1e293b") : QColor("#9ca3af"));
        QString text = QString("%1 %2").arg(QString::fromUtf8(d.text)).arg(loc);
        p.drawText(*d.r, Qt::AlignCenter, text);
        QPoint start = rectC_.center();
        if (d.d.y < 0) start.setY(rectC_.top());
        else if (d.d.y > 0) start.setY(rectC_.bottom());
        else if (d.d.x < 0) start.setX(rectC_.left());
        else start.setX(rectC_.right());
        QPoint end = d.r->center();
        p.setPen(can ? QColor("#3b82f6") : QColor("#d1d5db"));
        p.drawLine(start, end);
    }

    // NPC tags at bottom
    QStringList names;
    for (const auto& e : world_->entities()) {
        if (e.id != player->id && e.pos.x == player->pos.x && e.pos.y == player->pos.y) {
            names << QString::fromStdString(e.name);
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
        for (const auto& nm : names) {
            int w = fm.horizontalAdvance(nm) + 16;
            QRect tag(x, y, w, infoH - 8);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#e5e7eb"));
            p.drawRoundedRect(tag, (infoH - 8) / 2, (infoH - 8) / 2);
            p.setPen(QColor("#374151"));
            p.drawText(tag, Qt::AlignCenter, nm);
            x += w + 6;
        }
    }

    // bottom separator line
    p.setPen(QColor("#d1d5db"));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* e) {
    QPoint pt = e->pos();
    if (canN_ && rectN_.contains(pt)) emit moveNorth();
    else if (canS_ && rectS_.contains(pt)) emit moveSouth();
    else if (canW_ && rectW_.contains(pt)) emit moveWest();
    else if (canE_ && rectE_.contains(pt)) emit moveEast();
}
