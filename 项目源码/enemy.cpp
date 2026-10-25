#define _USE_MATH_DEFINES
#include <cmath>
#include "enemy.h"
#include "bullet.h"
#include "player.h"
#include <QGraphicsScene>

Enemy::Enemy(EnemyType type, const QPointF& pos, int wave, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_enemyType(type)
    , m_wave(wave)
    , m_frameCount(0)
    , m_shootTimer(0)
    , m_amplitude(0)
    , m_baseX(pos.x())
    , m_sprite(type == EnemyType::Basic ? QPixmap(":/assets/enemy1.png")
               : type == EnemyType::Elite ? QPixmap(":/assets/enemy2.png")
                                          : QPixmap(":/assets/boss.png"))
    , m_pathIndex(0)
{
    setPos(pos);

    switch (type) {
    case EnemyType::Basic:
        m_hp = BASIC_ENEMY_HP + wave * 3;
        m_scoreValue = BASIC_ENEMY_SCORE;
        m_shootInterval = 120;
        m_color = QColor(180, 60, 60);
        m_speedY = 1.5 + wave * 0.1;
        break;
    case EnemyType::Elite:
        m_hp = ELITE_ENEMY_HP + wave * 8;
        m_scoreValue = ELITE_ENEMY_SCORE;
        m_shootInterval = 70;
        m_color = QColor(200, 120, 30);
        m_amplitude = 80 + wave * 2;
        m_speedY = 1.0 + wave * 0.05;
        m_phaseOffset = QRandomGenerator::global()->bounded(6.28);
        break;
    case EnemyType::Boss:
        m_hp = BOSS_HP + wave * 100;
        m_scoreValue = BOSS_SCORE + wave * 1000;
        m_shootInterval = 30;
        m_color = QColor(200, 40, 200);
        m_speedY = 0;
        break;
    }
    m_maxHp = m_hp;
    m_shootTimer = QRandomGenerator::global()->bounded(m_shootInterval);
}

QRectF Enemy::boundingRect() const {
    if (m_enemyType == EnemyType::Boss)
        return QRectF(-112, -112, 224, 218);
    if (m_enemyType == EnemyType::Elite)
        return QRectF(-37, -39, 74, 78);
    return QRectF(-38, -29, 76, 58);
}

QPainterPath Enemy::shape() const {
    QPainterPath path;
    if (m_enemyType == EnemyType::Boss)
        path.addEllipse(-84, -76, 168, 156);
    else if (m_enemyType == EnemyType::Elite)
        path.addEllipse(-27, -31, 54, 62);
    else
        path.addEllipse(-28, -21, 56, 42);
    return path;
}

void Enemy::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    if (m_enemyType == EnemyType::Boss) {
        painter->drawPixmap(QRectF(-110, -103, 220, 206), m_sprite,
                            QRectF(0, 0, m_sprite.width(), m_sprite.height()));

        // HP bar
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(40, 40, 40));
        painter->drawRect(QRectF(-110, -111, 220, 7));
        painter->setBrush(QColor(255, 60, 60));
        qreal hpRatio = (qreal)m_hp / m_maxHp;
        painter->drawRect(QRectF(-110, -111, 220 * hpRatio, 7));

    } else if (m_enemyType == EnemyType::Elite) {
        painter->drawPixmap(QRectF(-37, -39, 74, 78), m_sprite,
                            QRectF(0, 0, m_sprite.width(), m_sprite.height()));

    } else {
        painter->drawPixmap(QRectF(-38, -29, 76, 58), m_sprite,
                            QRectF(0, 0, m_sprite.width(), m_sprite.height()));
    }
}

void Enemy::advance(int phase) {
    if (phase == 0) return;
    if (!isAlive()) return;

    m_frameCount++;
    m_shootTimer++;

    switch (m_enemyType) {
    case EnemyType::Basic:  moveBasic();  break;
    case EnemyType::Elite:  moveElite();  break;
    case EnemyType::Boss:   moveBoss();   break;
    }

    if (m_shootTimer >= m_shootInterval) {
        m_shootTimer = 0;
        shoot();
    }

    // Remove if off screen (non-boss enemies)
    if (m_enemyType != EnemyType::Boss && y() > WINDOW_HEIGHT + 60) {
        scene()->removeItem(this);
        deleteLater();
    }
}

void Enemy::moveBasic() {
    setPos(x(), y() + m_speedY);
}

void Enemy::moveElite() {
    setPos(x(), y() + m_speedY);
    // Sinusoidal horizontal movement
    qreal newX = m_baseX + m_amplitude * qSin(m_frameCount * 0.02 + m_phaseOffset);
    setPos(newX, y());
}

void Enemy::moveBoss() {
    // Boss enters from top, then hovers
    if (m_frameCount < 120) {
        // Enter screen
        setPos(x(), y() + 1.5);
    } else {
        // Hover with slight oscillation
        setPos(WINDOW_WIDTH / 2.0 + qSin(m_frameCount * 0.015) * 100,
               120 + qSin(m_frameCount * 0.02) * 20);
    }
}

void Enemy::shoot() {
    switch (m_enemyType) {
    case EnemyType::Basic:  shootBasic();  break;
    case EnemyType::Elite:  shootElite();  break;
    case EnemyType::Boss:   shootBoss();   break;
    }
}

void Enemy::shootBasic() {
    // Aim at nearest player
    for (auto* player : m_players) {
        if (!player->isActive() || !player->isAlive()) continue;
        QPointF dir = player->pos() - pos();
        qreal len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (len < 1) continue;
        dir /= len;

        auto* bullet = new EnemyBullet(pos(), dir.x(), dir.y(), ENEMY_BULLET_SPEED,
                                       QColor(255, 100, 100), 10);
        scene()->addItem(bullet);
    }
}

void Enemy::shootElite() {
    // Shoot a fan of 3 bullets aimed at players
    for (auto* player : m_players) {
        if (!player->isActive() || !player->isAlive()) continue;
        QPointF dir = player->pos() - pos();
        qreal len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (len < 1) continue;
        dir /= len;
        qreal angle = qAtan2(dir.y(), dir.x());

        for (int i = -1; i <= 1; ++i) {
            qreal a = angle + i * 0.25;
            auto* bullet = new EnemyBullet(pos(), qCos(a), qSin(a), ENEMY_BULLET_SPEED + 1,
                                           QColor(255, 180, 50), 15);
            scene()->addItem(bullet);
        }
    }
}

void Enemy::shootBoss() {
    // Boss has multiple attack patterns that cycle
    int phase = (m_frameCount / 300) % 4;

    switch (phase) {
    case 0: bossRadialBullets(12, m_frameCount * 0.05); break;
    case 1: bossAimedBullets(); break;
    case 2: bossSpiralBullets(); break;
    case 3:
        bossRadialBullets(8, m_frameCount * 0.07);
        bossAimedBullets();
        break;
    }
}

void Enemy::bossRadialBullets(int count, qreal offsetAngle) {
    for (int i = 0; i < count; ++i) {
        qreal angle = (2.0 * M_PI * i / count) + offsetAngle;
        auto* bullet = new EnemyBullet(pos(), qCos(angle), qSin(angle),
                                       ENEMY_BULLET_SPEED - 1,
                                       QColor(255, 100, 255), 12);
        scene()->addItem(bullet);
    }
}

void Enemy::bossAimedBullets() {
    for (auto* player : m_players) {
        if (!player->isActive() || !player->isAlive()) continue;
        QPointF dir = player->pos() - pos();
        qreal len = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (len < 1) continue;
        dir /= len;

        for (int i = -2; i <= 2; ++i) {
            qreal spread = i * 0.1;
            qreal angle = qAtan2(dir.y(), dir.x()) + spread;
            auto* bullet = new EnemyBullet(pos(), qCos(angle), qSin(angle),
                                           ENEMY_BULLET_SPEED + 2,
                                           QColor(200, 80, 255), 12);
            scene()->addItem(bullet);
        }
    }
}

void Enemy::bossSpiralBullets() {
    for (int i = 0; i < 3; ++i) {
        qreal angle = m_frameCount * 0.08 + i * 2.094;  // 120° apart
        auto* bullet = new EnemyBullet(pos(), qCos(angle), qSin(angle),
                                       ENEMY_BULLET_SPEED,
                                       QColor(180, 60, 255), 10);
        scene()->addItem(bullet);
    }
}

void Enemy::takeDamage(int damage) {
    if (!isAlive()) return;
    m_hp -= damage;
    if (m_hp <= 0) {
        m_hp = 0;
        emit enemyDestroyed(this);
        scene()->removeItem(this);
        deleteLater();
    }
}

void Enemy::setMovementPath(const QList<QPointF>& path) {
    m_movementPath = path;
    m_pathIndex = 0;
}
