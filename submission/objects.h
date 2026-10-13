#pragma once
#include "object.h"

class Stairs : public Object{
public:
    Stairs(GameContext& context, Coordinate position, Direction dir, bool down) : Object(context, position), dir_(dir), down_(down){
        SetSprites(context.asset_loader.LoadWithDirection("objects", down_ ? "dstair" : "stair"));
    }

    void Interact(Character& character, Direction dir) override {
        if (dir_ == Invert(dir)){
            Coordinate position = character.GetPosition();
            position = position + Coordinate::FromDirection(dir) * 2;
            character.SetPosition(position);
            qInfo() << QString::fromStdString(ToString(character.GetPosition()));
            if (down_){
                position = position + Coordinate(0, 0, -1);
                character.SetPosition(position);
                qInfo() << "Go stairs down";
            }
            else{
                position = position + Coordinate(0, 0, 1);
                character.SetPosition(position);
                qInfo() << "Go stairs up";
            }

        }
        else{
            qInfo() << "Trying to go stairs in the wrong dir";
        }
    }

    void Draw(DrawContext& context) const override {
        // Получите ассет из sprites_ на основе направления
        // взгляда персонажа и нарисуйте его.

        if (sprites_)
            context.painter.DrawObject(sprites_->Get(down_? Direction::kDown : Direction::kUp), GetPosition(), context.darkener.GetDarkness(GetPosition()));

    }

protected:
    void SetSprites(const AssetInDirections& sprites) {
        sprites_ = sprites;
    }

private:
    Direction dir_;
    std::optional<AssetInDirections> sprites_;
    bool down_;
};
