#include "bullet.h"

// ============ Player Bullet ============

Bullet::Bullet(const QPointF& pos, int playerId, int powerLevel, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_playerId(playerId)
    , m_damage(10 + powerLevel * 3)
    , m_speed(PLAYER_BULLET_SPEED)
{
    setPos(pos);
}

QRectF Bullet::boundingRect() const {
    return QRectF(-5, -10, 10, 20);
}

QPainterPath Bullet::shape() const {
    QPainterPath path;
    path.addRect(-3, -8, 6, 16);
    return path;
}

void Bullet::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QColor color = (m_playerId == 1) ? QColor(100, 200, 255) : QColor(255, 150, 100);
    QLinearGradient grad(0, -8, 0, 8);
    grad.setColorAt(0.0, Qt::white);
    grad.setColorAt(0.3, color.lighter(150));
    grad.setColorAt(1.0, color);
    painter->setBrush(grad);

    // Bullet shape: elongated ellipse
    painter->drawRoundedRect(-3, -8, 6, 16, 2, 2);

    // Glow
    painter->setBrush(QColor(color.red(), color.green(), color.blue(), 60));
    painter->drawEllipse(QPointF(0, 0), 6, 9);
}

void Bullet::advance(int phase) {
    if (phase == 0) return;
    setPos(x(), y() - m_speed);

    // Remove if off screen
    if (y() < -20) {
        scene()->removeItem(this);
        deleteLater();
    }
}

// ============ Enemy Bullet ============

EnemyBullet::EnemyBullet(const QPointF& pos, qreal dx, qreal dy, qreal speed,
                         const QColor& color, int damage, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_dx(dx)
    , m_dy(dy)
    , m_speed(speed)
    , m_color(color)
    , m_damage(damage)
{
    setPos(pos);
}

QRectF EnemyBullet::boundingRect() const {
    return QRectF(-6, -6, 12, 12);
}

QPainterPath EnemyBullet::shape() const {
    QPainterPath path;
    path.addEllipse(-4, -4, 8, 8);
    return path;
}

void EnemyBullet::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QRadialGradient grad(0, 0, 5);
    grad.setColorAt(0.0, Qt::white);
    grad.setColorAt(0.4, m_color);
    grad.setColorAt(1.0, m_color.darker(200));
    painter->setBrush(grad);
    painter->drawEllipse(-4, -4, 8, 8);

    // Outer glow
    painter->setBrush(QColor(m_color.red(), m_color.green(), m_color.blue(), 50));
    painter->drawEllipse(-6, -6, 12, 12);
}

void EnemyBullet::advance(int phase) {
    if (phase == 0) return;
    setPos(x() + m_dx * m_speed, y() + m_dy * m_speed);

    // Remove if off screen
    if (y() > WINDOW_HEIGHT + 20 || y() < -20 ||
        x() < -20 || x() > WINDOW_WIDTH + 20) {
        scene()->removeItem(this);
        deleteLater();
    }
}
