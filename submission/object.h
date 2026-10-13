#pragma once
#include "context.h"
#include <qDebug>

class Character;

class Object{
    public:
    Object(GameContext& context, Coordinate position) : context_(context), position_(position), visibility_(true), prev_position_(position){
            PlaceToMap(position_, this);
        }

        ~Object(){
            RemoveFromMap(position_, this);
        }

        virtual void Interact(Object& , Direction ) {}
        virtual void Interact(Character& character, Direction) {}
        virtual bool CanCover(const Object& object, Direction ) const {
            return !object.IsVisible();
        }

        public:
        Coordinate GetPosition() const{
            return position_;
        }

        virtual void SetPosition(Coordinate position){
            RemoveFromMap(position_, this);
            if (target_position_)
                RemoveFromMap(*target_position_, this);
            target_position_.reset();
            prev_position_ = position_;
            position_ = position;
            PlaceToMap(position_, this);
        }

        void Disappear(){
            visibility_ = false;
            RemoveFromMap(position_, this);
        }

        bool IsVisible() const{
            return visibility_;
        }

        GameContext& GetContext() const{
            return context_;
        }


        virtual void Draw(DrawContext&) const = 0;

        void SetTargetPosition(Coordinate position){
            if (target_position_)
                RemoveFromMap(*target_position_, this);
            target_position_ = position;
            PlaceToMap(*target_position_, this);
        }

        virtual CoordinateF GetExactPos() const{
            return GetPosition();
        }

        Coordinate GetPrevPosition() const{
            return prev_position_;
        }


    private:
        void RemoveFromMap(Coordinate position, Object* object){
            context_.object_map.Remove(position, object);
        }

        void PlaceToMap(Coordinate position, Object* object){
            context_.object_map.Place(position, object);
        }

        std::optional<Coordinate> target_position_;
        GameContext& context_;
        Coordinate position_;
        Coordinate prev_position_;
        bool visibility_;
};
