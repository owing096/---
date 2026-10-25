#include "powerup.h"
#include <QGraphicsScene>

PowerUp::PowerUp(PowerUpType type, const QPointF& pos, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_type(type)
    , m_frameCount(0)
    , m_speedY(1.5)
{
    setPos(pos);

    switch (type) {
    case PowerUpType::FirePower:
        m_color = QColor(255, 200, 50);
        m_label = "F";
        break;
    case PowerUpType::Bomb:
        m_color = QColor(255, 80, 80);
        m_label = "B";
        break;
    case PowerUpType::Health:
        m_color = QColor(50, 255, 100);
        m_label = "+";
        break;
    case PowerUpType::Shield:
        m_color = QColor(50, 150, 255);
        m_label = "S";
        break;
    }
}

QRectF PowerUp::boundingRect() const {
    return QRectF(-14, -14, 28, 28);
}

QPainterPath PowerUp::shape() const {
    QPainterPath path;
    path.addEllipse(-12, -12, 24, 24);
    return path;
}

void PowerUp::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    // Outer glow (pulsing)
    qreal pulse = 1.0 + 0.15 * qSin(m_frameCount * 0.08);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(m_color.red(), m_color.green(), m_color.blue(), 60));
    painter->drawEllipse(QPointF(0, 0), 14 * pulse, 14 * pulse);

    // Main circle
    painter->setBrush(m_color);
    painter->setPen(QPen(Qt::white, 1.5));
    painter->drawEllipse(-10, -10, 20, 20);

    // Label
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(QRectF(-10, -10, 20, 20), Qt::AlignCenter, m_label);
}

void PowerUp::advance(int phase) {
    if (phase == 0) return;
    m_frameCount++;
    setPos(x(), y() + m_speedY);

    // Remove if off screen
    if (y() > WINDOW_HEIGHT + 20) {
        scene()->removeItem(this);
        deleteLater();
    }
}
