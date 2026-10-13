#pragma once
#include <algorithm>
#include "object.h"
#include "field.h"
#include <QDebug>
#include "utility/assets.h"
#include "utility/animation.h"

struct DrawContext;

class Character : public Object{
public:
    Character(GameContext& context, Coordinate position, Direction dir) : Object(context, position), dir_(dir){}



    void SetDirection(const Direction& dir){
        dir_ = dir;
    }

    bool CanPassWall(Direction dir) const{
        std::shared_ptr<Wall> wall = GetContext().field.GetFloor(GetPosition().z_pos).GetWall(GetPosition(), dir);
        if (wall != nullptr)
            return wall->CanPass(*this, dir);
        return true;
    }

    bool CanGo(Direction dir) const{
        ObjectMap<Object> obj_map = GetContext().object_map;
        if (std::all_of(obj_map.Get(GetPosition() + Coordinate::FromDirection(dir)).begin(), obj_map.Get(GetPosition() + Coordinate::FromDirection(dir)).end(), [&](Object* i){
                return this->CanCover(*i, dir);
            }))
            return true;
      return false;
    }

    Direction GetDirection() const{
        return dir_;
    }

    virtual bool IsActive() const {
        return false;
    }

    void Draw(DrawContext& context) const override {
        // Получите ассет из sprites_ на основе направления
        // взгляда персонажа и нарисуйте его.

        const Asset* asset = nullptr;
        if (sprites_.has_value()) {
            asset = &sprites_->Get(dir_);
        } else if (walk_sprites_.has_value()){
            if (transition_)
                asset = &walk_sprites_->Get(dir_).GetAt(transition_->GetProgress());
            else
                asset = &walk_sprites_->Get(dir_).GetAt(0);
        }
        if (asset != nullptr) {
            context.painter.DrawObject(*asset, GetExactPos(), context.darkener.GetDarkness(GetExactPos()));
        }
    }

    void DoTransition(Direction dir, Callback at_end = {}){
        if (NowComing()){
            qInfo() << "Уже задана транзииция, не можем двигаться дальше";
            transition_.value().StopNow();
        }

        auto cur_pos = GetPosition();
        auto dst_pos = cur_pos + Coordinate::FromDirection(dir);

        SetTargetPosition(dst_pos);

        auto post_cb = [this, at_end, dst_pos](){
            SetPosition(dst_pos);
            if (at_end)
                at_end();
        };

        transition_.emplace(
            GetContext().timer,
            cur_pos,
            dst_pos,
            GetStepDuration(),
            post_cb
            );

    }
    void SetPosition(Coordinate position) override{
        Object::SetPosition(position);
        if (transition_.has_value())
            transition_->StopNow();
        transition_.reset();
    }


    virtual int GetStepDuration() const {
        return 200;
    }

    CoordinateF GetExactPos() const override{
        if (transition_)
            return transition_->GetPosition();
        return GetPosition();
    }

    bool NowComing() const{
        return transition_.has_value();
    }

protected:
    void SetSprites(const AssetInDirections& sprites) {
        sprites_ = sprites;
    }

    void SetWalkSprites(const AnimatedAssetInDirections& sprites){
        walk_sprites_ = sprites;
    }

private:
    Direction dir_;
    std::optional<AssetInDirections> sprites_;
    std::optional<AnimatedAssetInDirections> walk_sprites_;
    std::optional<Transition<CoordinateF>> transition_;
};
