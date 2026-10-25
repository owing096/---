#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QSet>
#include <QRandomGenerator>

// Game states
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

// Game modes
enum class GameMode { SINGLE, COOP, VS };

// Object types for collision identification
enum class ObjectType {
    Player,
    PlayerBullet,
    Enemy,
    EnemyBullet,
    PowerUp
};

// Power-up types
enum class PowerUpType {
    FirePower,  // Increase firepower (more bullets)
    Bomb,       // Extra bomb
    Health,     // Restore health
    Shield      // Temporary invincibility
};

// Enemy types
enum class EnemyType {
    Basic,      // Small, weak, straight movement
    Elite,      // Medium, sinusoidal movement
    Boss        // Large, multi-phase
};

// Constants
constexpr int WINDOW_WIDTH  = 900;
constexpr int WINDOW_HEIGHT = 1050;

constexpr int PLAYER_SPEED     = 5;
constexpr int PLAYER_BULLET_SPEED = 12;
constexpr int ENEMY_BULLET_SPEED  = 6;

constexpr int PLAYER_MAX_HP    = 100;
constexpr int PLAYER_MAX_BOMBS = 3;

constexpr int BASIC_ENEMY_HP   = 20;
constexpr int BASIC_ENEMY_SCORE = 100;
constexpr int ELITE_ENEMY_HP   = 60;
constexpr int ELITE_ENEMY_SCORE = 300;
constexpr int BOSS_HP          = 500;
constexpr int BOSS_SCORE       = 5000;

constexpr int GAME_FPS         = 60;
constexpr int INVINCIBLE_FRAMES = 90;  // ~1.5 seconds
constexpr int BOMB_DAMAGE      = 50;
constexpr int FIRE_POWER_MAX_LEVEL = 4;

// Object type accessor for QGraphicsItem subclasses
static constexpr int TypePlayer       = QGraphicsItem::UserType + 1;
static constexpr int TypePlayerBullet = QGraphicsItem::UserType + 2;
static constexpr int TypeEnemy        = QGraphicsItem::UserType + 3;
static constexpr int TypeEnemyBullet  = QGraphicsItem::UserType + 4;
static constexpr int TypePowerUp      = QGraphicsItem::UserType + 5;
