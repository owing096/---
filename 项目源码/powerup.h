#pragma once
#include "defs.h"

class PowerUp : public QGraphicsObject {
    Q_OBJECT
public:
    PowerUp(PowerUpType type, const QPointF& pos, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    int type() const override { return TypePowerUp; }

    void advance(int phase) override;

    PowerUpType powerUpType() const { return m_type; }

private:
    PowerUpType m_type;
    QColor m_color;
    QString m_label;
    int m_frameCount;
    qreal m_speedY;
};
