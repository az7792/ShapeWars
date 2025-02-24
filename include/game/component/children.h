#pragma once
#include <vector>
#include <ecs/fwd.h>
struct Children
{
     std::vector<ecs::Entity> children;
     Children &operator=(const Children &other) = default;
};
