#include "ecs/EntitySet.h"
#include "ecs/utils.h"
#include <stdexcept>

ecs::EntitySet::EntitySet(size_t size) : sparse_(size, nullValue) {}

void ecs::EntitySet::insert(ecs::Entity v)
{
     uint32_t index = ecs::entityToIndex(v);
     if (index >= sparse_.size())
     {
          sparse_.resize(std::max((size_t)index + 1, sparse_.size() * 2), nullValue);
     }
     if (sparse_[index] == nullValue)
     {
          dense_.push_back(v);
          sparse_[index] = dense_.size() - 1;
     }
}

void ecs::EntitySet::erase(ecs::Entity v)
{
     v = ecs::entityToIndex(v);
     if (v < sparse_.size() && sparse_[v] != nullValue)
     {
          uint32_t index = ecs::entityToIndex(dense_.back());
          sparse_[index] = sparse_[v];
          std::swap(dense_[sparse_[v]], dense_.back());
          dense_.pop_back();
          sparse_[v] = nullValue;
     }
}

bool ecs::EntitySet::find(ecs::Entity v) const
{
     v = ecs::entityToIndex(v);
     if (v >= sparse_.size())
     {
          return false;
     }
     return sparse_[v] != nullValue;
}
