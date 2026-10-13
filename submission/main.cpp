#include "characters.h"
#include "field_elements.h"
#include "mainwindow.h"

#include "objects.h"
#include "player.h"
#include "characters.h"
#include <memory>
#include <level_loader.h>

#include <QApplication>

void MakeRoom(Floor& floor, GameContext& context, int x1, int x2, int y1, int y2) {

    // Лямбда для создания стены.
    auto make_wall = [&]{ return std::make_shared<EdgeWall>(context); };

    for(int i = x1; i <= x2; ++i) {
        for(int j = y1; j <= y2; ++j) {
            auto tile = std::make_shared<FloorTile>(context, "grass");
            floor.SetTile({i, j}, tile);
            if (i == x1) {
                floor.SetWall({i, j}, Direction::kLeft, make_wall());
            }
            if (i == x2) {
                floor.SetWall({i, j}, Direction::kRight, make_wall());
            }
            if (j == y1) {
                floor.SetWall({i, j}, Direction::kUp, make_wall());
            }
            if (j == y2) {
                floor.SetWall({i, j}, Direction::kDown, make_wall());
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LevelLoader level_loader;
    Game game;
    level_loader.LoadFromFile(game, QString(":/resources/levels/main_level.json"));
    Controller controller{game};

    MainWindow w{game, controller};
    w.show();
    return a.exec();
}
