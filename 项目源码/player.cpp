#include "player.h"
#include "bullet.h"
#include <QGraphicsScene>

Player::Player(int playerId, bool isActive, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_playerId(playerId)
    , m_active(isActive)
    , m_hp(PLAYER_MAX_HP)
    , m_score(0)
    , m_bombs(PLAYER_MAX_BOMBS)
    , m_firePowerLevel(1)
    , m_invincibleTimer(0)
    , m_shootCooldown(0)
    , m_shieldTimer(0)
    , m_sprite(QString(":/assets/player%1.png").arg(playerId))
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(false);

    // Set key bindings based on player ID
    if (playerId == 1) {
        m_moveUp    = Qt::Key_W;
        m_moveDown  = Qt::Key_S;
        m_moveLeft  = Qt::Key_A;
        m_moveRight = Qt::Key_D;
        m_keyShoot  = Qt::Key_J;
        m_keyBomb   = Qt::Key_K;
    } else {
        m_moveUp    = Qt::Key_Up;
        m_moveDown  = Qt::Key_Down;
        m_moveLeft  = Qt::Key_Left;
        m_moveRight = Qt::Key_Right;
        m_keyShoot  = Qt::Key_1;      // 1 key
        m_keyBomb   = Qt::Key_2;      // 2 key
    }

    // Default position
    if (playerId == 1)
        setPos(WINDOW_WIDTH / 4, WINDOW_HEIGHT - 120);
    else
        setPos(WINDOW_WIDTH * 3 / 4, WINDOW_HEIGHT - 120);
}

QRectF Player::boundingRect() const {
    return QRectF(-48, -50, 96, 96);
}

QPainterPath Player::shape() const {
    QPainterPath path;
    path.addEllipse(-31, -33, 62, 68);
    return path;
}

void Player::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // Draw invincibility effect (flashing)
    if (isInvincible() && (m_invincibleTimer / 5) % 2 == 0) {
        painter->setOpacity(0.4);
    }

    // Draw shield
    if (m_shieldTimer > 0) {
        painter->setPen(QPen(QColor(100, 225, 255, 190), 3));
        painter->setBrush(QColor(0, 170, 255, 45));
        painter->drawEllipse(-46, -43, 92, 88);
    }

    painter->drawPixmap(QRectF(-44, -41, 88, 82), m_sprite,
                        QRectF(0, 0, m_sprite.width(), m_sprite.height()));

    // Player label
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 8, QFont::Bold));
    painter->drawText(QRectF(-18, -50, 36, 12), Qt::AlignCenter,
                      QString("P%1").arg(m_playerId));
    painter->setOpacity(1.0);
}

void Player::advance(int phase) {
    if (!m_active || phase == 0) return;

    movePlayer();

    if (m_invincibleTimer > 0) m_invincibleTimer--;
    if (m_shootCooldown > 0) m_shootCooldown--;
    if (m_shieldTimer > 0) m_shieldTimer--;

    // Auto-shoot when key held
    if (m_keysPressed.contains(m_keyShoot) && m_shootCooldown <= 0) {
        shoot();
    }
}

void Player::movePlayer() {
    qreal dx = 0, dy = 0;

    if (m_keysPressed.contains(m_moveUp))    dy -= PLAYER_SPEED;
    if (m_keysPressed.contains(m_moveDown))  dy += PLAYER_SPEED;
    if (m_keysPressed.contains(m_moveLeft))  dx -= PLAYER_SPEED;
    if (m_keysPressed.contains(m_moveRight)) dx += PLAYER_SPEED;

    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.707;
        dy *= 0.707;
    }

    qreal newX = qBound(44.0, x() + dx, (qreal)WINDOW_WIDTH - 44);
    qreal newY = qBound(42.0, y() + dy, (qreal)WINDOW_HEIGHT - 42);
    setPos(newX, newY);
}

void Player::shoot() {
    if (!m_active || !isAlive()) return;

    m_shootCooldown = 6;  // ~10 shots per second at 60fps

    QPointF bulletPos = pos() + QPointF(0, -45);
    Bullet* centerBullet = new Bullet(bulletPos, m_playerId, m_firePowerLevel);
    scene()->addItem(centerBullet);

    // Additional bullets based on fire power level
    for (int i = 1; i < m_firePowerLevel; ++i) {
        qreal offset = i * 8.0;
        Bullet* left  = new Bullet(bulletPos + QPointF(-offset, 0), m_playerId, m_firePowerLevel);
        Bullet* right = new Bullet(bulletPos + QPointF( offset, 0), m_playerId, m_firePowerLevel);
        scene()->addItem(left);
        scene()->addItem(right);
    }
}

void Player::takeDamage(int damage) {
    if (!m_active || !isAlive()) return;
    if (isInvincible()) return;
    if (m_shieldTimer > 0) {
        m_shieldTimer = 0;
        return;  // Shield absorbs one hit
    }

    m_hp -= damage;
    m_invincibleTimer = INVINCIBLE_FRAMES;

    if (m_hp <= 0) {
        m_hp = 0;
        emit playerDied(m_playerId);
    }
}

void Player::useBomb() {
    if (!m_active || !isAlive() || m_bombs <= 0) return;

    m_bombs--;
    emit bombUsed(m_playerId);
}

void Player::heal(int amount) {
    m_hp = qMin(m_hp + amount, PLAYER_MAX_HP);
}

void Player::addFirePower() {
    if (m_firePowerLevel < FIRE_POWER_MAX_LEVEL)
        m_firePowerLevel++;
}

void Player::activateShield() {
    m_shieldTimer = 300;  // 5 seconds
}

void Player::keyPress(int key) {
    m_keysPressed.insert(key);

    // Bomb on key press (not hold)
    if (key == m_keyBomb) {
        useBomb();
    }
}

void Player::keyRelease(int key) {
    m_keysPressed.remove(key);
}

void Player::clearKeys() {
    m_keysPressed.clear();
}

bool Player::canBeShotByPlayer() const {
    return m_shieldTimer <= 0 && !isInvincible();
}

QColor Player::playerColor() const {
    if (m_playerId == 1)
        return QColor(50, 150, 255);   // Blue
    else
        return QColor(255, 80, 80);    // Red
}
