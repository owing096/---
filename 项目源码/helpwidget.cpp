#include "helpwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

HelpWidget::HelpWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void HelpWidget::setupUI() {
    setStyleSheet("background-color: rgb(10, 10, 35);");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);

    // Title
    auto* title = new QLabel("HOW TO PLAY");
    title->setFont(QFont("Arial", 36, QFont::Bold));
    title->setStyleSheet("color: rgb(255, 220, 60);");
    title->setAlignment(Qt::AlignCenter);
    outerLayout->addWidget(title);
    outerLayout->addSpacing(25);

    // Scroll area for content
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical {"
        "  background: rgb(20, 20, 50); width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: rgb(80, 80, 150); border-radius: 5px; min-height: 30px;"
        "}"
    );

    auto* content = new QWidget();
    content->setStyleSheet("background: transparent;");
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(18);

    auto sectionStyle = "color: rgb(255, 200, 100); font-size: 18px; font-weight: bold;";
    auto textStyle    = "color: rgb(200, 200, 230); font-size: 13px;";
    auto keyStyle     = "color: rgb(255, 255, 150); font-size: 13px;";

    auto addSection = [&](const QString& title, const QString& body) {
        auto* secLabel = new QLabel(title);
        secLabel->setFont(QFont("Arial", 15, QFont::Bold));
        secLabel->setStyleSheet(sectionStyle);
        contentLayout->addWidget(secLabel);

        auto* bodyLabel = new QLabel(body);
        bodyLabel->setFont(QFont("Consolas", 12));
        bodyLabel->setStyleSheet(textStyle);
        bodyLabel->setWordWrap(true);
        contentLayout->addWidget(bodyLabel);
    };

    // Controls
    addSection("CONTROLS",
        "  Player 1:\n"
        "    Move       W / A / S / D\n"
        "    Shoot      J (hold for auto-fire)\n"
        "    Bomb       K (clears all enemy bullets)\n\n"
        "  Player 2:\n"
        "    Move       Arrow Keys  \n"
        "    Shoot      1 (hold for auto-fire)\n"
        "    Bomb       2 (clears all enemy bullets)\n\n"
        "  Both:\n"
        "    Pause      ESC or P");

    // Game modes
    addSection("GAME MODES",
        "  Single Player — Fight through waves of enemies alone.\n"
        "    Score as high as you can and defeat the boss.\n\n"
        "  Two Player Co-op — Team up with a friend.\n"
        "    Share the screen, combine firepower, and\n"
        "    survive together. Scores are individual.\n\n"
        "  Two Player VS — Battle against each other!\n"
        "    Players can damage each other while neutral\n"
        "    enemies attack both. Last one standing wins.");

    // Enemies
    addSection("ENEMIES",
        "  Basic (red)     Small, flies straight down.\n"
        "                  Fires aimed shots at players.\n\n"
        "  Elite (orange)  Medium, moves in sine-wave patterns.\n"
        "                  Fires 3-way fan shots.\n\n"
        "  Boss (purple)   Large, appears at end of each wave.\n"
        "                  Multi-phase attack patterns including\n"
        "                  radial bursts, aimed spreads, and spiral\n"
        "                  bullet patterns. Watch the HP bar!");

    // Power-ups
    addSection("POWER-UPS (dropped by enemies)",
        "  F (Yellow)  Fire Power Up  — More bullets per shot (Lv 1-4)\n"
        "  B (Red)     Extra Bomb     — +1 bomb (max 3)\n"
        "  + (Green)   Health Restore — Recovers 30 HP\n"
        "  S (Blue)    Shield         — Absorbs one hit, lasts 5 sec");

    // Scoring
    addSection("SCORING",
        "  Basic enemy destroyed   100 pts\n"
        "  Elite enemy destroyed   300 pts\n"
        "  Boss enemy destroyed    5000+ pts\n"
        "  Hitting an enemy        10 pts\n"
        "  VS: hitting other player 50 pts");

    // Tips
    addSection("TIPS",
        "  - Save bombs for boss fights or emergency situations.\n"
        "  - Collect Fire Power power-ups to increase your\n"
        "    bullet spread — up to Level 4.\n"
        "  - In VS mode, neutral enemies drop power-ups too.\n"
        "  - The boss changes attack pattern every few seconds.\n"
        "    Learn the patterns to dodge effectively.\n"
        "  - Diagonal movement is normalized — you move at\n"
        "    the same speed in all directions.\n"
        "  - After being hit, you have brief invincibility\n"
        "    (flashing sprite) — use it to escape danger.");

    contentLayout->addStretch();
    scroll->setWidget(content);
    outerLayout->addWidget(scroll);
    outerLayout->addSpacing(15);

    // Back button
    auto* btnBack = new QPushButton("BACK TO MENU");
    btnBack->setMinimumSize(250, 50);
    btnBack->setFont(QFont("Arial", 14, QFont::Bold));
    btnBack->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(50, 80, 150);"
        "  color: white;"
        "  border: 2px solid rgb(70, 110, 200);"
        "  border-radius: 8px;"
        "  padding: 10px;"
        "}"
        "QPushButton:hover { background-color: rgb(70, 110, 200); }"
    );
    connect(btnBack, &QPushButton::clicked, this, &HelpWidget::backToMenu);

    auto* btnRow = new QHBoxLayout();
    btnRow->setAlignment(Qt::AlignCenter);
    btnRow->addWidget(btnBack);
    outerLayout->addLayout(btnRow);
    outerLayout->addStretch(1);
}
