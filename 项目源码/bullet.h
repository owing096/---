#pragma once
#include "defs.h"

// Player bullet — moves upward
class Bullet : public QGraphicsObject {
    Q_OBJECT
public:
    Bullet(const QPointF& pos, int playerId, int powerLevel, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    int type() const override { return TypePlayerBullet; }

    void advance(int phase) override;

    int playerId() const { return m_playerId; }
    int damage() const { return m_damage; }

private:
    int  m_playerId;
    int  m_damage;
    qreal m_speed;
};

// Enemy bullet — moves in various directions
class EnemyBullet : public QGraphicsObject {
    Q_OBJECT
public:
    EnemyBullet(const QPointF& pos, qreal dx, qreal dy, qreal speed,
                const QColor& color, int damage, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    int type() const override { return TypeEnemyBullet; }

    void advance(int phase) override;
    int damage() const { return m_damage; }

private:
    qreal m_dx, m_dy;
    qreal m_speed;
    QColor m_color;
    int m_damage;
};
