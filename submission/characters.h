#pragma once
#include "character.h"
#include "utility/random.h"
#include "utility/geometry.h"


enum Phase {
    BROWNIAN, // Ходит хаотично, пока не встретил робота.
    FOLLOW,   // Встретил робота, ходит за роботом.
};

class Victim : public Character{
public:
    Victim(GameContext& context, Coordinate position, Direction dir) : Character(context, position, dir){
        SetSprites(context.asset_loader.LoadWithDirection("characters", "victim"));
        NextStep();
    }

    void Interact(Character& character, Direction) override{
        player_ = &character;
        phase_ = Phase::FOLLOW;
    }

    void MakeBrownianStep(){
        auto post_cb = [this](){
            remain_steps_--;
            NextStep();
        };

        if (remain_steps_ > 0 && CanPassWall(GetDirection()) && CanGo(GetDirection())){
                DoTransition(GetDirection(), post_cb);        }
        else{
            auto dirs = std::vector{
                Direction::kDown, Direction::kUp, Direction::kLeft, Direction::kRight};

            // Удаляем текущее направление.
            dirs.erase(std::find(dirs.begin(), dirs.end(), GetDirection()));

            // Выбираем случайный элемент вектора.
            Direction dir = GetContext().random.GetRandomElem(dirs);
            SetDirection(dir);
            remain_steps_ = GetContext().random.GetInRange(2, 8);
            Wait(post_cb);
        }
    }

    void MakeFollowStep(){
        if (GetDistance(GetPosition(), player_->GetPosition()) > 5){
            phase_ = Phase::BROWNIAN;
            MakeBrownianStep();
        }


        auto post_cb = [&](){
            NextStep();
        };

        Coordinate victim_pos = GetPosition();
        Coordinate player_pos = player_->GetPrevPosition();
        std::optional<Direction> robot_dir = GetDirectionToward(victim_pos, player_pos);

        if (robot_dir == std::nullopt){
            Wait(post_cb);
        }
        else{
            SetDirection(robot_dir.value());
            if (CanPassWall(robot_dir.value()) && CanGo(robot_dir.value())){
                DoTransition(GetDirection(), post_cb);
            }
            else{
                    Wait(post_cb);
                }
        }
    }

    void Wait(Callback cb){
        auto& timer = GetContext().timer;
        Time time = 200;
        guard_ = timer.PlanCallback(timer.Now() + time, std::move(cb));
    }

    void NextStep(){
        switch(phase_){
        case BROWNIAN:
            MakeBrownianStep();
            break;
        case FOLLOW:
            MakeFollowStep();
            break;
        }
    }

private:
    Phase phase_ = Phase::BROWNIAN;
    Character* player_ = nullptr;
    int remain_steps_ = GetContext().random.GetInRange(2, 8);
    Timer::Guard guard_;
    Clock clock_;
};
