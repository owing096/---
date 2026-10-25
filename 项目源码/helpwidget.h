#pragma once
#include "defs.h"

class HelpWidget : public QWidget {
    Q_OBJECT
public:
    explicit HelpWidget(QWidget* parent = nullptr);

signals:
    void backToMenu();

private:
    void setupUI();
};
