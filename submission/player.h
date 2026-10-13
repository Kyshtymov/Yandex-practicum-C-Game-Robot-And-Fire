#pragma once
#include "character.h"
#include <QDebug>

class Player : public Character{
public:
    Player(GameContext& context, Coordinate position, Direction dir) : Character(context, position, dir){
        SetWalkSprites(context.asset_loader.LoadAnimatedWithDirection("characters", "robot-walk", 19));

        qInfo() << "Player spawned on"<< QString::fromStdString(ToString(position)) << "dir" << QString::fromStdString(ToString(dir));
    }
    void GoCommand(Direction dir, std::function<void()> callback){

        if (!CanPassWall(dir)){
            std::shared_ptr<Wall> wall = GetContext().field.GetFloor(GetPosition().z_pos).GetWall(GetPosition(), dir);
            wall->Interact(*this, dir);
            qInfo() << "Player stoped on wall";
            return;
        }

        if (!CanGo(dir)){
            ObjectMap<Object> obj_map = GetContext().object_map;
            for (auto i:obj_map.Get(GetPosition() + Coordinate::FromDirection(dir)))
                i->Interact(*this, dir);
            qInfo() << "Player stoped on object";
            return;
        }


        DoTransition(dir, callback);
        qInfo() << "Player moves";

    }

    bool IsActive() const override {
        return true;
    }


};
