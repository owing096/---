#pragma once
#include "defs.h"

class GameOverWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget* parent = nullptr);

    void setResults(int p1Score, int p2Score, GameMode mode,
                    bool p1Alive, bool p2Alive);

signals:
    void returnToMenu();
    void playAgain();

private:
    QLabel* m_titleLabel;
    QLabel* m_scoreLabel;
    void setupUI();
};
