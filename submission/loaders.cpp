// Напишите код
#include <memory>
#include <level_loader.h>
#include <objects.h>
#include <field_elements.h>

class Wall;
class Door;
class EdgeWall;
class EmptyWall;

struct PlayerLoader {
    PlayerLoader(){ // Объявляем конструктор
        Loader<Player>::AddLoader("player", [](const QJsonObject &json, GameContext& context, Floor*){
            Coordinate pos = LoadData<Coordinate>(json["pos"]);
            Direction dir = LoadData<Direction>(json["dir"]);
            return std::make_shared<Player>(context, pos, dir);
        });
    }
} player_loader; // Создаём глобальную переменную.

struct GrassLoader{
    GrassLoader(){
        Loader<Tile>::AddLoader("grass",[](const QJsonObject &, GameContext& context, Floor*) {
            return std::make_shared<FloorTile>(context, "grass");
        });
    }
}grass_loader;

struct VictimLoader {
    VictimLoader(){ // Объявляем конструктор
        Loader<Victim>::AddLoader("victim", [](const QJsonObject& json_object, GameContext& context, Floor* floor){
            Coordinate level = {0, 0, floor->GetLevel()};
            Coordinate pos = LoadData<Coordinate>(json_object["pos"]);
            pos = pos + level;
            Direction dir = LoadData<Direction>(json_object["dir"]);
            return std::make_shared<Victim>(context, pos, dir);
        });
    }
}victim_loader; // Создаём глобальную переменную.



struct StairLoader {
    StairLoader(){ // Объявляем конструктор
        Loader<Stairs>::AddLoader("stair", [](const QJsonObject& json_object, GameContext& context, Floor* floor){
            Coordinate level = {0, 0, floor->GetLevel()};
            Coordinate pos = LoadData<Coordinate>(json_object["pos"]);
            pos = pos + level;Direction dir = LoadData<Direction>(json_object["dir"]);
            bool down = json_object["down"].toBool();
            return std::make_shared<Stairs>(context, pos, dir, down);
        });
    }
}stair_loader; // Создаём глобальную переменную.


struct DoorLoader{
    DoorLoader(){
        Loader<Wall>::AddLoader("door",[](const QJsonObject &json, GameContext& context, Floor* floor) {
            return std::make_shared<Door>();
        });
    }
}door_loader;


struct MarbleLoader{
    MarbleLoader(){
        Loader<Tile>::AddLoader("marble-black",[](const QJsonObject &, GameContext& context, Floor*) {
            return std::make_shared<FloorTile>(context, "floor4");
        });
    }
}marble_loader;


struct EdgeWallLoader{
    EdgeWallLoader(){
        Loader<Wall>::AddLoader("edge",[](const QJsonObject &json, GameContext& context, Floor* floor) {
            return std::make_shared<EdgeWall>(context);
        });
    }
}edge_wall_loader;


struct EmptyWallLoader{
    EmptyWallLoader(){
        Loader<Wall>::AddLoader("empty",[](const QJsonObject &json, GameContext& context, Floor* floor) {
            return std::make_shared<EmptyWall>();
        });
    }
}empty_wall_loader;


