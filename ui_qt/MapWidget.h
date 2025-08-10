#pragma once
#include <QWidget>
#include <QRect>
#include <QColor>
#include "../core/World.h"

class MapWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor)
public:
    explicit MapWidget(World* world, QWidget* parent=nullptr);

    QColor highlightColor() const { return highlightColor_; }
    void setHighlightColor(const QColor& c) { highlightColor_ = c; update(); }

    void setSelectedNpc(int id) { selectedNpc_ = id; update(); }

signals:
    void npcClicked(int id);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    World* world_;
    std::vector<std::pair<QRect, int>> npcRects_;
    Vec2 lastPlayerPos_{};
    bool hasLastPos_ = false;
    QColor highlightColor_ { QColor("#fef9c3") };
    int selectedNpc_ = 0;
    int hoveredNpc_ = 0;
};
