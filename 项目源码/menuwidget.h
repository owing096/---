#pragma once
#include "defs.h"

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget* parent = nullptr);

signals:
    void modeSelected(GameMode mode);
    void helpRequested();
    void quitRequested();

private:
    void setupUI();
};
