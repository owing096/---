#include "game.h"
#include "menuwidget.h"
#include "gameoverwidget.h"
#include "helpwidget.h"
#include "gamescene.h"
#include "player.h"

#include <QStackedWidget>
#include <QGraphicsView>

Game::Game(QWidget* parent)
    : QMainWindow(parent)
    , m_state(GameState::MENU)
    , m_currentMode(GameMode::SINGLE)
    , m_lastP1Score(0)
    , m_lastP2Score(0)
    , m_lastP1Alive(false)
    , m_lastP2Alive(false)
    , m_menuWidget(nullptr)
    , m_gameView(nullptr)
    , m_gameScene(nullptr)
    , m_gameOverWidget(nullptr)
    , m_helpWidget(nullptr)
{
    setupUI();
    showMenu();
}

Game::~Game() {
    if (m_gameScene) {
        m_gameScene->stopGame();
    }
}

void Game::setupUI() {
    setWindowTitle("Air War — 2D STG");
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setStyleSheet("background-color: rgb(10, 10, 35);");

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);
}

void Game::setState(GameState state) {
    m_state = state;
}

void Game::showMenu() {
    setState(GameState::MENU);

    if (!m_menuWidget) {
        m_menuWidget = new MenuWidget();
        connect(m_menuWidget, &MenuWidget::modeSelected, this, &Game::onModeSelected);
        connect(m_menuWidget, &MenuWidget::quitRequested, this, &QMainWindow::close);
        connect(m_menuWidget, &MenuWidget::helpRequested, this, &Game::onHelpRequested);
        m_stack->addWidget(m_menuWidget);
    }

    // Clean up game scene if exists
    if (m_gameScene) {
        m_gameScene->stopGame();
        m_stack->removeWidget(m_gameView);
        delete m_gameView;
        m_gameView = nullptr;
        m_gameScene = nullptr;
    }

    // Clean up game over if exists
    if (m_gameOverWidget) {
        m_stack->removeWidget(m_gameOverWidget);
        delete m_gameOverWidget;
        m_gameOverWidget = nullptr;
    }

    // Clean up help widget if exists
    if (m_helpWidget) {
        m_stack->removeWidget(m_helpWidget);
        delete m_helpWidget;
        m_helpWidget = nullptr;
    }

    m_stack->setCurrentWidget(m_menuWidget);
}

void Game::onModeSelected(GameMode mode) {
    m_currentMode = mode;
    startGame(mode);
}

void Game::startGame(GameMode mode) {
    setState(GameState::PLAYING);

    m_gameScene = new GameScene(mode);
    connect(m_gameScene, &GameScene::gameEnded, this, &Game::onGameEnded);
    connect(m_gameScene, &GameScene::paused, this, &Game::onPaused);

    m_gameView = new QGraphicsView(m_gameScene);
    m_gameView->setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gameView->setRenderHint(QPainter::Antialiasing);
    m_gameView->setStyleSheet("border: none; background: black;");
    m_gameView->setFocus();

    m_stack->addWidget(m_gameView);
    m_stack->setCurrentWidget(m_gameView);

    m_gameScene->startGame();
}

void Game::onGameEnded(int p1Score, int p2Score, GameMode mode,
                        bool p1Alive, bool p2Alive) {
    m_lastP1Score = p1Score;
    m_lastP2Score = p2Score;
    m_lastP1Alive = p1Alive;
    m_lastP2Alive = p2Alive;
    showGameOver(p1Score, p2Score, mode, p1Alive, p2Alive);
}

void Game::onPaused() {
    // Show pause overlay could go here
    // For now, just bring up the menu on ESC
    showMenu();
}

void Game::showGameOver(int p1Score, int p2Score, GameMode mode,
                         bool p1Alive, bool p2Alive) {
    setState(GameState::GAME_OVER);

    if (!m_gameOverWidget) {
        m_gameOverWidget = new GameOverWidget();
        connect(m_gameOverWidget, &GameOverWidget::returnToMenu, this, &Game::onReturnToMenu);
        connect(m_gameOverWidget, &GameOverWidget::playAgain, this, &Game::onPlayAgain);
        m_stack->addWidget(m_gameOverWidget);
    }

    m_gameOverWidget->setResults(p1Score, p2Score, mode, p1Alive, p2Alive);
    m_stack->setCurrentWidget(m_gameOverWidget);
}

void Game::onHelpRequested() {
    if (!m_helpWidget) {
        m_helpWidget = new HelpWidget();
        connect(m_helpWidget, &HelpWidget::backToMenu, this, &Game::onReturnToMenu);
        m_stack->addWidget(m_helpWidget);
    }
    m_stack->setCurrentWidget(m_helpWidget);
}

void Game::onReturnToMenu() {
    showMenu();
}

void Game::onPlayAgain() {
    startGame(m_currentMode);
}
