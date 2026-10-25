#include "gameoverwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

GameOverWidget::GameOverWidget(QWidget* parent)
    : QWidget(parent)
    , m_titleLabel(nullptr)
    , m_scoreLabel(nullptr)
{
    setupUI();
}

void GameOverWidget::setupUI() {
    setStyleSheet("background-color: rgb(10, 10, 35);");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    m_titleLabel = new QLabel("GAME OVER");
    m_titleLabel->setFont(QFont("Arial", 42, QFont::Bold));
    m_titleLabel->setStyleSheet("color: rgb(255, 200, 50);");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_scoreLabel = new QLabel();
    m_scoreLabel->setFont(QFont("Consolas", 18));
    m_scoreLabel->setStyleSheet("color: white;");
    m_scoreLabel->setAlignment(Qt::AlignCenter);

    auto styleBtn = [](QPushButton* btn, const QString& bgColor, const QString& hoverColor) {
        btn->setMinimumSize(220, 50);
        btn->setFont(QFont("Arial", 15, QFont::Bold));
        btn->setStyleSheet(
            QString("QPushButton {"
                    "  background-color: %1;"
                    "  color: white;"
                    "  border: 2px solid %2;"
                    "  border-radius: 8px;"
                    "  padding: 10px;"
                    "}"
                    "QPushButton:hover { background-color: %2; }").arg(bgColor, hoverColor));
    };

    auto* btnRetry = new QPushButton("PLAY AGAIN");
    styleBtn(btnRetry, "rgb(40, 130, 60)", "rgb(60, 180, 80)");
    connect(btnRetry, &QPushButton::clicked, this, &GameOverWidget::playAgain);

    auto* btnMenu = new QPushButton("MAIN MENU");
    styleBtn(btnMenu, "rgb(50, 80, 150)", "rgb(70, 110, 200)");
    connect(btnMenu, &QPushButton::clicked, this, &GameOverWidget::returnToMenu);

    layout->addStretch(2);
    layout->addWidget(m_titleLabel);
    layout->addSpacing(20);
    layout->addWidget(m_scoreLabel);
    layout->addSpacing(40);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignCenter);
    btnLayout->setSpacing(20);
    btnLayout->addWidget(btnRetry);
    btnLayout->addWidget(btnMenu);
    layout->addLayout(btnLayout);

    layout->addStretch(2);
}

void GameOverWidget::setResults(int p1Score, int p2Score, GameMode mode,
                                 bool p1Alive, bool p2Alive) {
    QString resultText;

    if (mode == GameMode::SINGLE) {
        m_titleLabel->setText("GAME OVER");
        m_titleLabel->setStyleSheet("color: rgb(255, 100, 80);");
        resultText = QString("Final Score: %1").arg(p1Score);
    } else if (mode == GameMode::COOP) {
        m_titleLabel->setText("MISSION COMPLETE");
        m_titleLabel->setStyleSheet("color: rgb(100, 255, 150);");
        resultText = QString("Player 1 Score: %1\nPlayer 2 Score: %2\nTotal: %3")
                         .arg(p1Score).arg(p2Score).arg(p1Score + p2Score);
    } else {  // VS mode
        if (p1Alive && !p2Alive) {
            m_titleLabel->setText("PLAYER 1 WINS!");
            m_titleLabel->setStyleSheet("color: rgb(80, 150, 255);");
        } else if (p2Alive && !p1Alive) {
            m_titleLabel->setText("PLAYER 2 WINS!");
            m_titleLabel->setStyleSheet("color: rgb(255, 100, 100);");
        } else {
            m_titleLabel->setText("DRAW!");
            m_titleLabel->setStyleSheet("color: rgb(200, 200, 200);");
        }
        resultText = QString("Player 1 Score: %1\nPlayer 2 Score: %2")
                         .arg(p1Score).arg(p2Score);
    }

    m_scoreLabel->setText(resultText);
}
