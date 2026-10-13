#pragma once
#include "context.h"
#include "field.h"
#include "player.h"
#include "characters.h"
#include <QDebug>
#include "utility/random.h"
#include <memory>


class Game {
public:
    Game(AssetLoader& asset_loader, int w, int h): asset_loader_(asset_loader), w_(w), h_(h){
    }
    Game(int w, int h): w_(w), h_(h){
    }

    Game(Size s): w_(s.width), h_(s.height){}
    Game(): w_(0), h_(0){}

    void Reset(Size s){
        w_ = s.width;
        h_ = s.height;
        field_ = Field{w_, h_};
    }

    void SetAsset(AssetLoader& asset_loader){
        asset_loader_ = asset_loader;
    }

    GameContext& GetContext() {
        return context_;
    }

    Floor& AddFloor(int level){
        context_.field.AddFloor(level);
        return context_.field.GetFloor(level);
    }

    void SetPlayer(std::shared_ptr<Player> player){
        player_ = player;
    }

    Field& GetField(){
        return context_.field;
    }

    Player& GetPlayer(){
        return *player_;
    }

    Floor& GetCurrentFloor(){
        return field_.GetFloor(player_->GetPosition().z_pos);
    }

    void DrawFrame(DrawContext& draw_context){
        int level = GetCurrentFloor().GetLevel();
        context_.field.GetFloor(level).DrawFloor(draw_context);
        for (int i = 0; i < h_; ++i) {
            context_.field.GetFloor(level).DrawHWalls(draw_context, i);
            for (int k = 0; k < w_; ++k) {
                Coordinate pos = {i, k, level};
                context_.field.GetFloor(level).DrawVWall(draw_context, pos);
                const auto& obj_set = object_map_.Get(pos);
                for (auto* j:obj_set){
                    if (j->IsVisible() && (j->GetPosition() == pos) && (j != nullptr)){
                        j->Draw(draw_context);
                    }
                }
            }
        }
        player_->Draw(draw_context);
    }

    void AddVictim(const std::shared_ptr<Victim>& object){
        vect_of_victims_.push_back(object);
    }

    void AddStair(const std::shared_ptr<Stairs>& object){
        vect_of_stairs_.push_back(object);
    }

private:
    AssetLoader asset_loader_;
    int w_;
    int h_;
    ObjectMap<Object> object_map_;
    Field field_{w_, h_};
    RandomGen random_;
    Timer timer_;
    GameContext context_{.object_map = object_map_, .field = field_, .random = random_, .asset_loader = asset_loader_, .timer = timer_};
    std::shared_ptr<Player> player_;
    std::vector<std::shared_ptr<Victim>> vect_of_victims_;
    std::vector<std::shared_ptr<Stairs>> vect_of_stairs_;
};
