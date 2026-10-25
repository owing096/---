#pragma once
#include "defs.h"
#include <QPixmap>

class Player;

class Enemy : public QGraphicsObject {
    Q_OBJECT
public:
    Enemy(EnemyType type, const QPointF& pos, int wave, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    int type() const override { return TypeEnemy; }

    void advance(int phase) override;

    EnemyType enemyType() const { return m_enemyType; }
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int scoreValue() const { return m_scoreValue; }
    bool isAlive() const { return m_hp > 0; }

    void takeDamage(int damage);
    void setPlayers(const QList<Player*>& players) { m_players = players; }
    void setMovementPath(const QList<QPointF>& path);

signals:
    void enemyDestroyed(Enemy* enemy);

private:
    EnemyType m_enemyType;
    int m_hp;
    int m_maxHp;
    int m_scoreValue;
    int m_wave;
    int m_frameCount;
    int m_shootTimer;
    int m_shootInterval;
    QColor m_color;
    qreal m_amplitude;
    qreal m_baseX;
    qreal m_speedY;
    qreal m_phaseOffset;
    QPixmap m_sprite;

    // Path following
    QList<QPointF> m_movementPath;
    int m_pathIndex;

    QList<Player*> m_players;

    void moveBasic();
    void moveElite();
    void moveBoss();
    void shoot();
    void shootBasic();
    void shootElite();
    void shootBoss();
    void bossRadialBullets(int count, qreal offsetAngle);
    void bossAimedBullets();
    void bossSpiralBullets();
};
