#pragma once
#include "defs.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QGraphicsView>

class MenuWidget;
class GameOverWidget;
class GameScene;
class HelpWidget;

class Game : public QMainWindow {
    Q_OBJECT
public:
    explicit Game(QWidget* parent = nullptr);
    ~Game();

private slots:
    void onModeSelected(GameMode mode);
    void onHelpRequested();
    void onGameEnded(int p1Score, int p2Score, GameMode mode,
                     bool p1Alive, bool p2Alive);
    void onPaused();
    void onReturnToMenu();
    void onPlayAgain();

private:
    void setState(GameState state);
    void showMenu();
    void startGame(GameMode mode);
    void showGameOver(int p1Score, int p2Score, GameMode mode,
                      bool p1Alive, bool p2Alive);
    void setupUI();

    GameState m_state;
    GameMode  m_currentMode;
    int m_lastP1Score;
    int m_lastP2Score;
    bool m_lastP1Alive;
    bool m_lastP2Alive;

    QWidget*      m_centralWidget;
    QStackedWidget* m_stack;

    MenuWidget*    m_menuWidget;
    QGraphicsView* m_gameView;
    GameScene*     m_gameScene;
    GameOverWidget* m_gameOverWidget;
    HelpWidget*     m_helpWidget;
};
