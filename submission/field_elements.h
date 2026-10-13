#pragma once
#include <QDebug>
#include "field.h"
#include "context.h"
#include "utility/random.h"
#include "character.h"

class RandomGen;

class FloorTile : public Tile{
public:
    FloorTile(const GameContext& context, const std::string& asset_name):Tile(context.asset_loader.LoadTile("floors", asset_name)){
        auto asset = GetAsset();
        asset.Rotate(context.random.GetInRange(0, 3));
    }
};

class EmptyTile : public Tile{
};


class Door : public Wall{
public:
    void Interact(Character& character, Direction ) override {
        if (character.IsActive()){
            is_opened_ = !is_opened_;
            qInfo() << "Opening door";
        }
    }

    bool CanPass(const Character& , Direction ) const override {
        if (is_opened_)
            return true;
        return false;
    }

    void Draw(DrawContext& context, Coordinate pos, Orientation dir) const override {
        const QColor door_color{100, 100, 100, 128};
        const QColor edge_color{50, 50, 100, 128};
        const int edge_width = 1;
        CoordinateF p11 = pos;
        CoordinateF p12 = p11 + CoordinateF{0, 0, 1};
        CoordinateF p13 = p12 + CoordinateF(Coordinate::FromOrientation(dir)) * 0.5;
        CoordinateF p21 = pos + Coordinate::FromOrientation(dir);
        CoordinateF p22 = p21 + CoordinateF{0, 0, 1};
        CoordinateF p23 = p22 - CoordinateF(Coordinate::FromOrientation(dir)) * 0.5;

        if (is_opened_){
            context.painter.DrawRect(p11, p12, p13, door_color, edge_color, edge_width, context.darkener.GetDarkness(pos));
            context.painter.DrawRect(p21, p22, p23, door_color, edge_color, edge_width, context.darkener.GetDarkness(pos));
        }
    }

private:
    bool is_opened_ = false;
};

class EmptyWall : public Wall{
public:
    bool CanPass(const Character& , Direction ) const override {
        return true;
    }

    void Draw(DrawContext& , Coordinate , Orientation) const override {}
};

class EdgeWall : public Wall{
public:
    EdgeWall(const GameContext& context):Wall(context.asset_loader.LoadTile("walls", "wall-white")){
    }

    bool CanPass(const Character& , Direction ) const override {
        return false;
    }
};
