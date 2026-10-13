#pragma once
#include <QJsonValue>
#include <qjsonobject.h>
#include "utility/geometry.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>
#include <game.h>
#include <QFile>

class Stairs;
class Door;

template<class T>
class Loader {
public:
    static std::shared_ptr<T> Load(const QJsonValue &json, GameContext& context, Floor* floor) {
        if (json.isObject()){
            auto type = json.toObject()["type"].toString(); // Определяем тип объекта.
            auto loader = loaders.at(type.toStdString());   // Выбираем нужный загрузчик.
            return loader(json.toObject(), context, floor); // Вызываем его.
        }
    }

    static std::shared_ptr<T> Load(const QString &string, GameContext& context, Floor* floor){
        auto loader = loaders.at(string.toStdString());   // Выбираем нужный загрузчик.
        QJsonObject json_object; // Создаём объект для хранения данных для JSON.
        json_object["type"] = string; // Заполняем его данными из структуры.
        return loader(json_object, context, floor); // Вызываем его.
    }

    using LoaderImpl = std::function<std::shared_ptr<T>(const QJsonObject &json, GameContext& context, Floor* floor)>; // Создание псевдонима для типа загрузчика.

    static void AddLoader(const std::string& type, LoaderImpl impl) {
        loaders[type] = impl;
    }


    private:
              // Словарь с загрузчиками.
        inline static std::map<std::string, LoaderImpl> loaders;
};

template<class T>
T LoadData(const QJsonValue &);

template<>
inline Coordinate LoadData<Coordinate>(const QJsonValue &json) {
    // Напишите функцию.
    if (json.isObject()) {

        auto obj = json.toObject();
        // Если в словаре есть нужные ключи.
        if (obj.contains("x") && obj.contains("y")&& obj.contains("z")) {
            return Coordinate{obj["x"].toInt(), obj["y"].toInt(),obj["z"].toInt()};
        }
        throw std::runtime_error("Wrong map keys for Coordinate");

    } else if (json.isArray()) {
        auto obj = json.toArray();

        // Если массив правильного размера.
        if (obj.size() == 3) {
            return Coordinate{obj[0].toInt(), obj[1].toInt(),obj[2].toInt()};
        }
        if (obj.size() == 2) {
            return Coordinate{obj[0].toInt(), obj[1].toInt(),0};
        }
        throw std::runtime_error("Wrong array length for Coordinate");
    }

    throw std::runtime_error("Expected array or object for Coordinate");
}

template<>
inline Size LoadData<Size>(const QJsonValue &json) {
    // Напишите функцию.

    if (json.isObject()) {
        auto obj = json.toObject();
        // Если в словаре есть нужные ключи.
        if (obj.contains("h") && obj.contains("w")) {
            return Size{obj["w"].toInt(), obj["h"].toInt()};
        }
        throw std::runtime_error("Wrong map keys for size");

    } else if (json.isArray()) {
        auto obj = json.toArray();
        // Если массив правильного размера.
        if (obj.size() == 2) {
            return Size{obj[0].toInt(), obj[1].toInt()};
        }
        throw std::runtime_error("Wrong array length for size");
    }
    throw std::runtime_error("Expected array or object for size");
}

template<>
inline Direction LoadData<Direction>(const QJsonValue &json) {
    // Напишите функцию.


    auto obj = json.toString();
    if (obj == "u") {
        return Direction::kUp;
    }
    if (obj == "d") {
        return Direction::kDown;
    }
    if (obj == "r") {
        return Direction::kRight;
    }
    if (obj == "l") {
        return Direction::kLeft;
    }
    throw std::runtime_error("Unknown directon " + json.toString().toStdString());

    return Direction::kLeft;
}


class LevelLoader{
public:
    bool LoadFromFile(Game& game, const QString& file_name){
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open saved file.";
            return false;
        }
        QByteArray data = file.readAll(); // Считываем все данные из файла.
        auto json_object = QJsonDocument::fromJson(data).object();

        return Load(game, json_object);
    }

    bool Load(Game& game, const QJsonObject &json){
        AssetLoader loader;
        game.Reset(LoadData<Size>(json["size"]));
        game.SetAsset(loader);
        game.SetPlayer(Loader<Player>::Load(json["player"], game.GetContext(), nullptr));
        std::vector<std::shared_ptr<Victim>> victims;
        std::vector<std::shared_ptr<Stairs>> stairs;
        int floor_number = json["floors"].toArray().size();
        std::vector<Floor*> floor_vector;
        for (int i = 0; i < floor_number; i++){
            floor_vector.push_back(&game.AddFloor(i));
            AddFloor(game, json["floors"][i].toObject(), *floor_vector[i], victims, stairs);
        }
        return true;
    }

private:
    void AddFloor(Game& game, const QJsonObject &json, Floor& floor_object, std::vector<std::shared_ptr<Victim>>& victims, std::vector<std::shared_ptr<Stairs>>& stairs){

        int regions_number = json["regions"].toArray().size();
        for (int i = 0; i < regions_number; i++)
            AddRegions(game, json["regions"][i].toObject(), floor_object);

        int walls_number = json["walls"].toArray().size();
        for (int i = 0; i < walls_number; i++){
            auto position = LoadData<Coordinate>(json["walls"][i]["pos"]);
            auto dir = LoadData<Direction>(json["walls"][i]["dir"]);
            AddWalls(game, floor_object, position, 1, dir, dir, json["walls"][i]["wall"]);
        }
        int object_number = json["objects"].toArray().size();
        for (int i = 0; i < object_number; i++){
            if (json["objects"][i]["type"].toString() == "victim")
                game.AddVictim(Loader<Victim>::Load(json["objects"][i].toObject(), game.GetContext(), &floor_object));
                //victims.push_back(Loader<Victim>::Load(json["objects"][i].toObject(), game.GetContext(), &floor_object));
            else
                game.AddStair(Loader<Stairs>::Load(json["objects"][i].toObject(), game.GetContext(), &floor_object));
                //stairs.push_back(Loader<Stairs>::Load(json["objects"][i].toObject(), game.GetContext(), &floor_object));
        }


    }


    void AddRegions(Game& game, const QJsonObject &region, Floor& floor_object){
        auto position = LoadData<Coordinate>(region["pos"].toArray());
        auto size = LoadData<Size>(region["size"].toArray());

        for (int i = position.x_pos; i < position.x_pos + size.width; i++) {
            for (int j = position.y_pos; j < position.y_pos + size.height; j++) {
                QString string = "marble-black";
                floor_object.SetTile({i, j}, Loader<Tile>::Load(string, game.GetContext(), &floor_object));
            }
        }
        if (region["tile"].toString() == "marble-black"){
            // Для wall_l:
            if (!region["wall_l"].toString().isEmpty())
                AddWalls(game, floor_object, position, size.height, Direction::kDown, Direction::kLeft, region["wall_l"]);
            // Для wall_r:

            if (!region["wall_r"].toString().isEmpty())
                AddWalls(game, floor_object, position + Coordinate2D{size.width, 0}, size.height, Direction::kDown, Direction::kLeft, region["wall_r"]);
            // Для wall_u:

            if (!region["wall_u"].toString().isEmpty())
                AddWalls(game, floor_object, position, size.width, Direction::kRight, Direction::kUp, region["wall_u"]);
            // Для wall_d:

            if (!region["wall_d"].toString().isEmpty())
                AddWalls(game, floor_object, position + Coordinate2D{0, size.height}, size.width, Direction::kRight, Direction::kUp, region["wall_d"]);
        }
        else
            AddSafeSpace(game, region, floor_object);
    }
    void AddWalls(Game& game, Floor& floor_object, Coordinate2D pos, int length, Direction line_dir, Direction wall_dir, const QJsonValue& data) {
        for (int i = 0; i < length; i++){
            Coordinate2D where = pos + Coordinate2D(Coordinate::FromDirection(line_dir)) * i;
            floor_object.SetWall(where, wall_dir, Loader<Wall>::Load(data.toString(), game.GetContext(), &floor_object));
        }
    }
    void AddSafeSpace(Game& game, const QJsonObject& data, Floor& floor_object) {
        auto position = LoadData<Coordinate>(data["pos"].toArray());
                QString string = "grass";
                floor_object.SetTile(position, Loader<Tile>::Load(string, game.GetContext(), &floor_object));
    }
};
