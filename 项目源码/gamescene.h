#pragma once
#include "defs.h"

class Player;
class Enemy;
class PowerUp;

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(GameMode mode, QObject* parent = nullptr);
    ~GameScene();

    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();

    Player* player1() const { return m_player1; }
    Player* player2() const { return m_player2; }

    int wave() const { return m_wave; }
    GameMode gameMode() const { return m_mode; }

signals:
    void gameEnded(int p1Score, int p2Score, GameMode mode, bool p1Alive, bool p2Alive);
    void paused();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void gameLoop();
    void onEnemyDestroyed(Enemy* enemy);
    void onBombUsed(int playerId);
    void onPlayerDied(int playerId);

private:
    GameMode m_mode;
    Player* m_player1;
    Player* m_player2;
    QTimer* m_loopTimer;
    int m_frameCount;

    // Enemy spawning
    int m_wave;
    int m_spawnTimer;
    int m_enemiesSpawned;
    int m_enemiesInWave;
    bool m_bossSpawned;
    bool m_bossDefeated;
    int m_levelClearTimer;

    // Background
    QGraphicsPixmapItem* m_bg1;
    QGraphicsPixmapItem* m_bg2;
    qreal m_bgScrollSpeed;
    void updateBackground();

    // HUD
    QGraphicsTextItem* m_hudP1;
    QGraphicsTextItem* m_hudP2;
    QGraphicsTextItem* m_waveText;
    void updateHUD();

    // Collision
    void checkCollisions();

    // Spawning
    void spawnEnemy();
    void spawnBoss();
    void spawnPowerUp(const QPointF& pos);
    void updateDifficulty();

    // Helpers
    void dropPowerUp(const QPointF& pos);
    QList<Player*> activePlayers() const;

    // Background generation
    QPixmap generateBackground();
};
