#pragma once
#include "utility/assets.h"
#include "utility/painter.h"
#include "utility/utility.h"
#include "darkener.h"
#include "context.h"

class Character;
class Victim;
class Stairs;

class ElementWithAsset {
public:
    ElementWithAsset(const Asset& asset) : asset_(asset) {}
    ElementWithAsset() = default;

protected:
    const Asset& GetAsset() const {
        return asset_;
    }

private:
    Asset asset_;
};




class Tile: public ElementWithAsset {
    public:
        // Используем конструктор базового класса.
        using ElementWithAsset::ElementWithAsset;

    virtual void Draw(Coordinate pos, DrawContext& context) const {
        context.painter.DrawFloor(GetAsset(), pos, context.darkener.GetDarkness(pos));
    }

};


class Wall: public ElementWithAsset {
public:
    // Используем конструктор базового класса.
    using ElementWithAsset::ElementWithAsset;

    virtual void Interact(Character& , Direction ) {

    }

    virtual bool CanPass(const Character& , Direction ) const {
        return false;
    }

    virtual void Draw(DrawContext& context, Coordinate pos, Orientation dir) const {
        context.painter.DrawWall(GetAsset(), pos, dir, context.darkener.GetDarkness(pos));
    }
private:

};


struct Cell {
    std::shared_ptr<Tile> floor;
    std::shared_ptr<Wall> left_wall;
    std::shared_ptr<Wall> top_wall;
};


class Floor{
public:
    Floor(int level, int w, int h): w_(w), h_(h), cells_(w + 1, h + 1), level_(level){
    }

    int GetLevel() const{
        return level_;
    }

    void SetTile(Coordinate2D where, std::shared_ptr<Tile> tile){
        cells_.Get(where).floor = tile;
    }

    void SetWall(Coordinate2D where, Direction dir, std::shared_ptr<Wall> wall){
        GetWallPtr(where, dir) = wall;
    }

    std::shared_ptr<Wall>& GetWallPtr(Coordinate2D where, Direction dir){
        switch(dir){
        case Direction::kUp:
            return cells_.Get(where).top_wall;
        case Direction::kLeft:
            return cells_.Get(where).left_wall;
        case Direction::kDown:
            return cells_.Get(where + Coordinate::FromDirection(dir)).top_wall;
        case Direction::kRight:
            return cells_.Get(where + Coordinate::FromDirection(dir)).left_wall;
        }
    }

    std::shared_ptr<Wall> GetWall(Coordinate2D where, Direction dir){
        return GetWallPtr(where, dir);
    }

    void DrawFloor(DrawContext& context) const{
        auto level = GetLevel();
        for (int i = 0; i < h_; ++i) {
            for (int j = 0; j < w_; ++j) {
                Coordinate pos{j, i, level};
                std::shared_ptr<Tile> floor = cells_.Get({j, i}).floor;
                if (floor != nullptr)
                    floor->Draw(pos, context);
                // Нарисовать объект в позиции pos.
            }
        }
    }


    void DrawHWalls(DrawContext& context, int y) const {
        // Пройдитесь по клеткам ряда "y"
        // и для каждой клетки нарисуйте top_wall.
        for (int i = 0; i < w_; ++i) {
            Coordinate pos{i, y};
            if (cells_.Get(pos).top_wall != nullptr)
                cells_.Get(pos).top_wall->Draw(context, pos, Orientation::kHorizontal);
        }
    }

    void DrawVWall(DrawContext& context, Coordinate pos) const {
        // Нарисуйте left_wall у клетки в позиции pos.
        if (cells_.Get(pos).left_wall != nullptr)
            cells_.Get(pos).left_wall->Draw(context, pos, Orientation::kVertical);
    }


private:
    int w_;
    int h_;
    Array2D<Cell> cells_;
    int level_;
};

class Field{
public:
    Field(int w, int h) : w_(w), h_(h){}

    int GetWidth() const{
        return w_;
    }

    int GetHeight() const{
        return h_;
    }

    Size GetRect() const {
        return {GetWidth(), GetHeight()};
    }

    void AddFloor(int level){
        Floor floor(level, w_, h_);
        field_.emplace(level, floor);//insert({level, floor});
    }

    Floor& GetFloor(int floor){
        return field_.at(floor);
    }

    const Floor& GetFloor(int floor) const {
        return field_.at(floor);
    }

private:
    int w_;
    int h_;
    std::map<int, Floor> field_;
};
