#pragma once

#include "utility/geometry.h"
#include "game.h"
#include <QDebug>

class Controller {
public:
    Controller(Game& game) : game_(game) {
    }

    void OnMoveKey(Direction dir) {
        qInfo() << "<<< Moving in direction" << QString::fromStdString(ToString(dir));
        auto& player = game_.GetPlayer();
        auto dir_robot = player.GetDirection();

        if (dir_robot == dir){
            if (!player.NowComing())
                dir_ = dir;
            qInfo() << "Trying to go";
            MovePlayer();
        }
        else{
            player.SetDirection(dir);
            qInfo() << "Changing direction";
            OnReleaseMoveKey(player.GetDirection());
        }

        qInfo() << ">>> New player position:" << QString::fromStdString(ToString(player.GetPosition())) << "dir" << QString::fromStdString(ToString(player.GetDirection()));
    }

    void OnReleaseMoveKey(Direction ) {
        dir_.reset();
    }

    Game& GetGame(){
        return game_;
    }

    void SetRedrawCallback(const std::function<void()>& callback){
        callback_ = callback;
    }

    void Tick(){
        game_.GetContext().timer.UpdateTime(clock_.GetElapsedTime());
        if (callback_)
            callback_();
    }

private:
    void MovePlayer(){
        auto post_cb = [&](){
                MovePlayer();
        };

        if (dir_)
            game_.GetPlayer().GoCommand(*dir_, post_cb);
    }

    Game& game_;
    std::function<void()> callback_;
    Clock clock_;
    std::optional<Direction> dir_;
};
