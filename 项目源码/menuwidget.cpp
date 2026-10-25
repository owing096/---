#include "menuwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MenuWidget::MenuWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void MenuWidget::setupUI() {
    setStyleSheet("background-color: rgb(10, 10, 35);");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(15);

    // Title
    auto* title = new QLabel("AIR WAR");
    title->setFont(QFont("Arial", 48, QFont::Bold));
    title->setStyleSheet("color: rgb(255, 220, 60);");
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel("2D Vertical Scrolling Shooter");
    subtitle->setFont(QFont("Arial", 14));
    subtitle->setStyleSheet("color: rgb(180, 180, 220);");
    subtitle->setAlignment(Qt::AlignCenter);

    mainLayout->addStretch(2);
    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);
    mainLayout->addSpacing(30);

    // Mode buttons
    auto styleBtn = [](QPushButton* btn, const QString& color) {
        btn->setMinimumSize(250, 60);
        btn->setFont(QFont("Arial", 16, QFont::Bold));
        btn->setStyleSheet(
            QString("QPushButton {"
                    "  background-color: %1;"
                    "  color: white;"
                    "  border: 2px solid %2;"
                    "  border-radius: 10px;"
                    "  padding: 10px;"
                    "}"
                    "QPushButton:hover {"
                    "  background-color: %2;"
                    "}"
                    "QPushButton:pressed {"
                    "  background-color: %3;"
                    "}").arg(color).arg(color).arg(color));
    };

    auto* btnSingle = new QPushButton("SINGLE PLAYER");
    styleBtn(btnSingle, "rgb(40, 100, 180)");
    connect(btnSingle, &QPushButton::clicked, this, [this]() {
        emit modeSelected(GameMode::SINGLE);
    });

    auto* btnCoop = new QPushButton("TWO PLAYER CO-OP");
    styleBtn(btnCoop, "rgb(40, 160, 80)");
    connect(btnCoop, &QPushButton::clicked, this, [this]() {
        emit modeSelected(GameMode::COOP);
    });

    auto* btnVs = new QPushButton("TWO PLAYER VS");
    styleBtn(btnVs, "rgb(200, 60, 60)");
    connect(btnVs, &QPushButton::clicked, this, [this]() {
        emit modeSelected(GameMode::VS);
    });

    auto* btnQuit = new QPushButton("QUIT");
    btnQuit->setMinimumSize(250, 50);
    btnQuit->setFont(QFont("Arial", 14, QFont::Bold));
    btnQuit->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(60, 60, 60);"
        "  color: white;"
        "  border: 2px solid rgb(80, 80, 80);"
        "  border-radius: 10px;"
        "  padding: 8px;"
        "}"
        "QPushButton:hover { background-color: rgb(80, 80, 80); }"
    );
    connect(btnQuit, &QPushButton::clicked, this, &MenuWidget::quitRequested);

    auto centerBtn = [](QPushButton* btn, QVBoxLayout* layout) {
        auto* hbox = new QHBoxLayout();
        hbox->setAlignment(Qt::AlignCenter);
        hbox->addWidget(btn);
        layout->addLayout(hbox);
    };

    centerBtn(btnSingle, mainLayout);
    centerBtn(btnCoop, mainLayout);
    centerBtn(btnVs, mainLayout);
    mainLayout->addSpacing(15);

    // Help button
    auto* btnHelp = new QPushButton("HOW TO PLAY");
    btnHelp->setMinimumSize(250, 50);
    btnHelp->setFont(QFont("Arial", 14, QFont::Bold));
    btnHelp->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(100, 80, 40);"
        "  color: white;"
        "  border: 2px solid rgb(180, 150, 60);"
        "  border-radius: 10px;"
        "  padding: 8px;"
        "}"
        "QPushButton:hover { background-color: rgb(140, 110, 50); }"
    );
    connect(btnHelp, &QPushButton::clicked, this, &MenuWidget::helpRequested);
    centerBtn(btnHelp, mainLayout);

    mainLayout->addSpacing(10);
    centerBtn(btnQuit, mainLayout);

    // Controls quick reference
    mainLayout->addSpacing(30);
    auto* controlsLabel = new QLabel(
        "P1: WASD + J/K    P2: Arrows + 1/2    ESC: Pause");
    controlsLabel->setFont(QFont("Consolas", 10));
    controlsLabel->setStyleSheet("color: rgb(140, 140, 180);");
    controlsLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(controlsLabel);

    mainLayout->addStretch(1);
}
