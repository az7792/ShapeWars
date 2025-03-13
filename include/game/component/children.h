// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include <ecs/fwd.h>
struct Children
{
     std::vector<ecs::Entity> children;
     Children &operator=(const Children &other) = default;
};
