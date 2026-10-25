#include "game.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Air War");

    Game game;
    game.show();

    return app.exec();
}
