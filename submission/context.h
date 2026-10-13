#pragma once
#include "utility/utility.h"
#include "darkeners.h"
#include "utility/timer.h"

class Object;
class Field;
class RandomGen;
class AssetLoader;

struct GameContext {
    ObjectMap<Object>& object_map;
    Field& field;
    RandomGen& random;         // Для внесения случайных изменений.
    AssetLoader& asset_loader; // Для загрузки ассетов.
    Timer& timer;
};

class Painter;
class FlashlightDarkener;

struct DrawContext {
    Painter& painter;
    const FlashlightDarkener& darkener;
};
