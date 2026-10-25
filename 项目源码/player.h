#pragma once
#include "defs.h"
#include <QPixmap>

class Player : public QGraphicsObject {
    Q_OBJECT
public:
    Player(int playerId, bool isActive, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    int type() const override { return TypePlayer; }

    void advance(int phase) override;

    // State
    int playerId() const { return m_playerId; }
    int hp() const { return m_hp; }
    int maxHp() const { return PLAYER_MAX_HP; }
    int score() const { return m_score; }
    int bombs() const { return m_bombs; }
    int firePowerLevel() const { return m_firePowerLevel; }
    bool isAlive() const { return m_hp > 0; }
    bool isActive() const { return m_active; }
    bool isInvincible() const { return m_invincibleTimer > 0; }

    void setActive(bool active) { m_active = active; }
    void addScore(int pts) { m_score += pts; }
    void addBomb() { if (m_bombs < PLAYER_MAX_BOMBS) m_bombs++; }
    void heal(int amount);
    void addFirePower();
    void activateShield();

    void takeDamage(int damage);
    void useBomb();
    void shoot();

    // Keyboard input
    void keyPress(int key);
    void keyRelease(int key);
    void clearKeys();

    // VS mode: can be shot by other player
    bool canBeShotByPlayer() const;

signals:
    void bombUsed(int playerId);
    void playerDied(int playerId);

private:
    int  m_playerId;   // 1 or 2
    bool m_active;
    int  m_hp;
    int  m_score;
    int  m_bombs;
    int  m_firePowerLevel;
    int  m_invincibleTimer;
    int  m_shootCooldown;
    int  m_shieldTimer;
    QPixmap m_sprite;

    // Movement keys
    QSet<int> m_keysPressed;
    int m_moveUp;
    int m_moveDown;
    int m_moveLeft;
    int m_moveRight;
    int m_keyShoot;
    int m_keyBomb;

    void movePlayer();
    QColor playerColor() const;
};
