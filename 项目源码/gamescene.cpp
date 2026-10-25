#include "gamescene.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "powerup.h"

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

GameScene::GameScene(GameMode mode, QObject* parent)
    : QGraphicsScene(parent)
    , m_mode(mode)
    , m_player1(nullptr)
    , m_player2(nullptr)
    , m_loopTimer(new QTimer(this))
    , m_frameCount(0)
    , m_wave(1)
    , m_spawnTimer(0)
    , m_enemiesSpawned(0)
    , m_enemiesInWave(8)
    , m_bossSpawned(false)
    , m_bossDefeated(false)
    , m_levelClearTimer(0)
    , m_bg1(nullptr)
    , m_bg2(nullptr)
    , m_bgScrollSpeed(1.5)
    , m_hudP1(nullptr)
    , m_hudP2(nullptr)
    , m_waveText(nullptr)
{
    setSceneRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    setBackgroundBrush(Qt::black);

    connect(m_loopTimer, &QTimer::timeout, this, &GameScene::gameLoop);
}

GameScene::~GameScene() {
    m_loopTimer->stop();
}

void GameScene::startGame() {
    // Clear any existing items
    clear();

    m_frameCount = 0;
    m_wave = 1;
    m_spawnTimer = 0;
    m_enemiesSpawned = 0;
    m_enemiesInWave = 8;
    m_bossSpawned = false;
    m_bossDefeated = false;
    m_levelClearTimer = 0;

    // Background
    QPixmap bg = generateBackground();
    m_bg1 = new QGraphicsPixmapItem(bg);
    m_bg1->setPos(0, 0);
    m_bg2 = new QGraphicsPixmapItem(bg);
    m_bg2->setPos(0, -WINDOW_HEIGHT);
    addItem(m_bg1);
    addItem(m_bg2);

    // Create players
    m_player1 = new Player(1, true);  // Player 1 always active
    addItem(m_player1);

    bool p2active = (m_mode == GameMode::COOP || m_mode == GameMode::VS);
    m_player2 = new Player(2, p2active);
    if (p2active)
        addItem(m_player2);

    connect(m_player1, &Player::bombUsed, this, &GameScene::onBombUsed);
    connect(m_player1, &Player::playerDied, this, &GameScene::onPlayerDied);
    connect(m_player2, &Player::bombUsed, this, &GameScene::onBombUsed);
    connect(m_player2, &Player::playerDied, this, &GameScene::onPlayerDied);

    // HUD
    m_hudP1 = new QGraphicsTextItem();
    m_hudP1->setDefaultTextColor(QColor(100, 180, 255));
    m_hudP1->setFont(QFont("Consolas", 11, QFont::Bold));
    m_hudP1->setPos(10, 5);
    m_hudP1->setZValue(100);
    addItem(m_hudP1);

    m_hudP2 = new QGraphicsTextItem();
    m_hudP2->setDefaultTextColor(QColor(255, 130, 130));
    m_hudP2->setFont(QFont("Consolas", 11, QFont::Bold));
    m_hudP2->setPos(WINDOW_WIDTH - 220, 5);
    m_hudP2->setZValue(100);
    addItem(m_hudP2);

    m_waveText = new QGraphicsTextItem();
    m_waveText->setDefaultTextColor(Qt::white);
    m_waveText->setFont(QFont("Arial", 28, QFont::Bold));
    m_waveText->setZValue(100);
    addItem(m_waveText);

    updateHUD();

    // Show wave text briefly
    m_waveText->setPlainText(QString("WAVE %1").arg(m_wave));
    m_waveText->setPos(WINDOW_WIDTH / 2.0 - 80, WINDOW_HEIGHT / 2.0 - 30);

    m_loopTimer->start(1000 / GAME_FPS);  // ~60 FPS
}

void GameScene::pauseGame() {
    m_loopTimer->stop();
}

void GameScene::resumeGame() {
    m_loopTimer->start(1000 / GAME_FPS);
}

void GameScene::stopGame() {
    m_loopTimer->stop();
}

void GameScene::gameLoop() {
    m_frameCount++;

    updateBackground();
    advance();  // Advances all QGraphicsItems

    // Spawn enemies
    m_spawnTimer++;
    updateDifficulty();

    if (!m_bossSpawned && m_enemiesSpawned >= m_enemiesInWave) {
        // Check if all regular enemies are dead
        bool allEnemiesDead = true;
        for (auto* item : items()) {
            if (item->type() == TypeEnemy) {
                allEnemiesDead = false;
                break;
            }
        }
        if (allEnemiesDead && m_enemiesSpawned >= m_enemiesInWave) {
            spawnBoss();
            m_bossSpawned = true;
        }
    }

    // Spawn regular enemies
    if (!m_bossSpawned && m_enemiesSpawned < m_enemiesInWave) {
        int interval = qMax(30, 90 - m_wave * 3);
        if (m_spawnTimer >= interval) {
            m_spawnTimer = 0;
            spawnEnemy();
            m_enemiesSpawned++;
        }
    }

    // After boss is defeated, advance wave
    if (m_bossDefeated && m_levelClearTimer == 0) {
        m_levelClearTimer = 180;  // 3 seconds delay
    }
    if (m_levelClearTimer > 0) {
        m_levelClearTimer--;
        if (m_levelClearTimer == 0) {
            m_wave++;
            m_enemiesSpawned = 0;
            m_enemiesInWave = 8 + m_wave * 2;
            m_bossSpawned = false;
            m_bossDefeated = false;
            m_bgScrollSpeed += 0.2;

            m_waveText->setPlainText(QString("WAVE %1").arg(m_wave));
            m_waveText->setPos(WINDOW_WIDTH / 2.0 - 80, WINDOW_HEIGHT / 2.0 - 30);
        }
    }

    // Clear wave text after 2 seconds
    if (m_frameCount > 120 && m_levelClearTimer == 0 && !m_waveText->toPlainText().isEmpty()) {
        // Only clear if wave transition is done
        if (m_enemiesSpawned > 0 || m_frameCount > 180) {
            m_waveText->setPlainText("");
        }
    }

    checkCollisions();
    updateHUD();

    // Check game over
    bool p1Alive = m_player1->isActive() && m_player1->isAlive();
    bool p2Alive = m_player2->isActive() && m_player2->isAlive();

    if (m_mode == GameMode::SINGLE) {
        if (!p1Alive) {
            stopGame();
            emit gameEnded(m_player1->score(), 0, m_mode, false, false);
        }
    } else if (m_mode == GameMode::COOP) {
        if (!p1Alive && !p2Alive) {
            stopGame();
            emit gameEnded(m_player1->score(), m_player2->score(), m_mode, false, false);
        }
    } else {  // VS mode
        if (!p1Alive || !p2Alive) {
            stopGame();
            emit gameEnded(m_player1->score(), m_player2->score(), m_mode, p1Alive, p2Alive);
        }
    }
}

void GameScene::updateBackground() {
    qreal y1 = m_bg1->y() + m_bgScrollSpeed;
    qreal y2 = m_bg2->y() + m_bgScrollSpeed;

    if (y1 >= WINDOW_HEIGHT) y1 = y2 - WINDOW_HEIGHT;
    if (y2 >= WINDOW_HEIGHT) y2 = y1 - WINDOW_HEIGHT;

    m_bg1->setPos(0, y1);
    m_bg2->setPos(0, y2);
}

void GameScene::updateHUD() {
    auto makeHudText = [](Player* p) -> QString {
        if (!p->isActive()) return "INACTIVE";
        if (!p->isAlive()) return "DESTROYED";

        // HP bar (ASCII style)
        int hpBars = (p->hp() * 20) / p->maxHp();
        QString hpBar = QString("[").leftJustified(hpBars + 1, '|') +
                        QString(hpBars == 20 ? "]" : QString(" ").repeated(20 - hpBars) + "]");

        return QString("P%1 HP:%2/%3  Score:%4  Bombs:%5  Power:Lv%6")
            .arg(p->playerId())
            .arg(p->hp(), 3)
            .arg(p->maxHp())
            .arg(p->score(), 6)
            .arg(p->bombs())
            .arg(p->firePowerLevel());
    };

    if (m_player1->isActive())
        m_hudP1->setPlainText(makeHudText(m_player1));
    else
        m_hudP1->setPlainText("");

    if (m_player2->isActive())
        m_hudP2->setPlainText(makeHudText(m_player2));
    else
        m_hudP2->setPlainText("");
}

void GameScene::checkCollisions() {
    // Collect all items by type
    QList<QGraphicsItem*> playerBullets;
    QList<QGraphicsItem*> enemyBullets;
    QList<QGraphicsItem*> enemies;
    QList<QGraphicsItem*> powerUps;

    for (auto* item : items()) {
        switch (item->type()) {
        case TypePlayerBullet: playerBullets.append(item); break;
        case TypeEnemyBullet:  enemyBullets.append(item);  break;
        case TypeEnemy:        enemies.append(item);       break;
        case TypePowerUp:      powerUps.append(item);      break;
        }
    }

    QList<Player*> players;
    if (m_player1->isActive() && m_player1->isAlive()) players.append(m_player1);
    if (m_player2->isActive() && m_player2->isAlive()) players.append(m_player2);

    // Player bullets vs enemies
    for (auto* pbItem : playerBullets) {
        Bullet* bullet = static_cast<Bullet*>(pbItem);
        for (auto* eItem : enemies) {
            Enemy* enemy = static_cast<Enemy*>(eItem);
            if (!enemy->isAlive()) continue;
            if (bullet->collidesWithItem(enemy)) {
                enemy->takeDamage(bullet->damage());
                removeItem(bullet);
                delete bullet;

                // Score for the player who shot
                for (auto* p : players) {
                    if (p->playerId() == bullet->playerId()) {
                        p->addScore(10);  // Hit score
                        break;
                    }
                }
                goto nextBullet;
            }
        }
        // In VS mode: player bullets can hit other player
        if (m_mode == GameMode::VS) {
            Player* target = (bullet->playerId() == 1) ? m_player2 : m_player1;
            if (target->isActive() && target->isAlive() && target->canBeShotByPlayer()) {
                if (bullet->collidesWithItem(target)) {
                    target->takeDamage(bullet->damage());
                    removeItem(bullet);
                    delete bullet;
                    Player* shooter = (bullet->playerId() == 1) ? m_player1 : m_player2;
                    if (shooter->isActive()) shooter->addScore(50);
                    continue;
                }
            }
        }
        nextBullet:;
    }

    // Enemy bullets vs players
    for (auto* ebItem : enemyBullets) {
        EnemyBullet* eb = static_cast<EnemyBullet*>(ebItem);
        for (auto* player : players) {
            if (eb->collidesWithItem(player)) {
                player->takeDamage(eb->damage());
                removeItem(eb);
                delete eb;
                break;
            }
        }
    }

    // Players vs enemies (ramming)
    for (auto* eItem : enemies) {
        Enemy* enemy = static_cast<Enemy*>(eItem);
        if (!enemy->isAlive()) continue;
        for (auto* player : players) {
            if (enemy->collidesWithItem(player)) {
                player->takeDamage(30);
                enemy->takeDamage(50);
            }
        }
    }

    // Players vs power-ups
    for (auto* puItem : powerUps) {
        PowerUp* pu = static_cast<PowerUp*>(puItem);
        for (auto* player : players) {
            if (pu->collidesWithItem(player)) {
                switch (pu->powerUpType()) {
                case PowerUpType::FirePower: player->addFirePower();  break;
                case PowerUpType::Bomb:      player->addBomb();       break;
                case PowerUpType::Health:    player->heal(30);        break;
                case PowerUpType::Shield:    player->activateShield(); break;
                }
                removeItem(pu);
                delete pu;
                break;
            }
        }
    }
}

void GameScene::spawnEnemy() {
    qreal x = QRandomGenerator::global()->bounded(50, WINDOW_WIDTH - 50);
    EnemyType type = (QRandomGenerator::global()->bounded(100) < 20 + m_wave * 2)
                         ? EnemyType::Elite : EnemyType::Basic;

    auto* enemy = new Enemy(type, QPointF(x, -40), m_wave);
    QList<Player*> plist = activePlayers();
    enemy->setPlayers(plist);
    connect(enemy, &Enemy::enemyDestroyed, this, &GameScene::onEnemyDestroyed);
    addItem(enemy);
}

void GameScene::spawnBoss() {
    auto* boss = new Enemy(EnemyType::Boss, QPointF(WINDOW_WIDTH / 2.0, -80), m_wave);
    QList<Player*> plist = activePlayers();
    boss->setPlayers(plist);
    connect(boss, &Enemy::enemyDestroyed, this, [this](Enemy* enemy) {
        m_bossDefeated = true;
        onEnemyDestroyed(enemy);
    });
    addItem(boss);

    // Show boss warning
    m_waveText->setPlainText("WARNING!\n  BOSS");
    m_waveText->setPos(WINDOW_WIDTH / 2.0 - 100, WINDOW_HEIGHT / 2.0 - 50);
}

void GameScene::onEnemyDestroyed(Enemy* enemy) {
    // Award score to both players (in co-op) or individually
    if (m_mode == GameMode::COOP) {
        // Split score
        m_player1->addScore(enemy->scoreValue() / 2);
        m_player2->addScore(enemy->scoreValue() / 2);
    } else {
        // Both get score in VS (whoever gets last hit gets bonus, simplified)
        m_player1->addScore(enemy->scoreValue() / 3);
        if (m_player2->isActive())
            m_player2->addScore(enemy->scoreValue() / 3);
    }

    dropPowerUp(enemy->pos());
}

void GameScene::dropPowerUp(const QPointF& pos) {
    int roll = QRandomGenerator::global()->bounded(100);
    if (roll < 35) {
        // 35% chance to drop a power-up
        int typeRoll = QRandomGenerator::global()->bounded(100);
        PowerUpType type;
        if (typeRoll < 35)       type = PowerUpType::FirePower;
        else if (typeRoll < 60)  type = PowerUpType::Bomb;
        else if (typeRoll < 85)  type = PowerUpType::Health;
        else                     type = PowerUpType::Shield;
        spawnPowerUp(pos);
    }
}

void GameScene::spawnPowerUp(const QPointF& pos) {
    PowerUpType type = PowerUpType::FirePower;  // Default, will be overridden by dropPowerUp
    // Create a random power-up
    int roll = QRandomGenerator::global()->bounded(100);
    if (roll < 35)       type = PowerUpType::FirePower;
    else if (roll < 60)  type = PowerUpType::Bomb;
    else if (roll < 85)  type = PowerUpType::Health;
    else                 type = PowerUpType::Shield;

    auto* pu = new PowerUp(type, pos);
    addItem(pu);
}

void GameScene::onBombUsed(int playerId) {
    // Clear all enemy bullets on screen
    for (auto* item : items()) {
        if (item->type() == TypeEnemyBullet) {
            removeItem(item);
            delete item;
        }
    }

    // Damage all enemies
    for (auto* item : items()) {
        if (item->type() == TypeEnemy) {
            Enemy* enemy = static_cast<Enemy*>(item);
            enemy->takeDamage(BOMB_DAMAGE);
            // Award score
            Player* p = (playerId == 1) ? m_player1 : m_player2;
            if (p->isActive()) p->addScore(20);
        }
    }

    // Visual effect: flash
    QGraphicsRectItem* flash = new QGraphicsRectItem(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    flash->setBrush(QColor(255, 255, 255, 180));
    flash->setPen(Qt::NoPen);
    flash->setZValue(200);
    addItem(flash);

    // Remove flash after short delay
    QTimer::singleShot(150, this, [this, flash]() {
        removeItem(flash);
        delete flash;
    });
}

void GameScene::onPlayerDied(int playerId) {
    Q_UNUSED(playerId);
    // Game over check is in gameLoop
}

void GameScene::updateDifficulty() {
    // Difficulty scales with wave number
    m_bgScrollSpeed = 1.5 + m_wave * 0.15;
}

QList<Player*> GameScene::activePlayers() const {
    QList<Player*> list;
    if (m_player1->isActive() && m_player1->isAlive()) list.append(m_player1);
    if (m_player2->isActive() && m_player2->isAlive()) list.append(m_player2);
    return list;
}

QPixmap GameScene::generateBackground() {
    QPixmap bg(WINDOW_WIDTH, WINDOW_HEIGHT);
    bg.fill(QColor(5, 5, 25));

    QPainter painter(&bg);
    painter.setRenderHint(QPainter::Antialiasing);

    // Stars
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < 60; ++i) {
        int sx = QRandomGenerator::global()->bounded(WINDOW_WIDTH);
        int sy = QRandomGenerator::global()->bounded(WINDOW_HEIGHT);
        int size = QRandomGenerator::global()->bounded(1, 3);
        int brightness = QRandomGenerator::global()->bounded(100, 255);
        painter.setBrush(QColor(brightness, brightness, brightness));
        painter.drawEllipse(sx, sy, size, size);
    }

    // Distant clouds / nebula
    for (int i = 0; i < 5; ++i) {
        int cx = QRandomGenerator::global()->bounded(WINDOW_WIDTH);
        int cy = QRandomGenerator::global()->bounded(WINDOW_HEIGHT);
        QRadialGradient grad(cx, cy, QRandomGenerator::global()->bounded(40, 100));
        QColor c(30, 40, 80, 40);
        grad.setColorAt(0, c);
        grad.setColorAt(1, QColor(0, 0, 0, 0));
        painter.setBrush(grad);
        painter.drawEllipse(QPointF(cx, cy),
                            QRandomGenerator::global()->bounded(60, 150),
                            QRandomGenerator::global()->bounded(60, 150));
    }

    painter.end();
    return bg;
}

void GameScene::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_P) {
        pauseGame();
        emit paused();
        return;
    }

    if (m_player1->isActive())
        m_player1->keyPress(event->key());
    if (m_player2->isActive())
        m_player2->keyPress(event->key());

    QGraphicsScene::keyPressEvent(event);
}

void GameScene::keyReleaseEvent(QKeyEvent* event) {
    if (m_player1->isActive())
        m_player1->keyRelease(event->key());
    if (m_player2->isActive())
        m_player2->keyRelease(event->key());

    QGraphicsScene::keyReleaseEvent(event);
}
