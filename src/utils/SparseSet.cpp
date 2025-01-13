#include "utils/SparseSet.h"
#include <stdexcept>

SparseSet::SparseSet(size_t size) : sparse_(size, nullValue) {}

void SparseSet::insert(uint32_t v)
{
     if (v >= sparse_.size())
     {
          throw std::out_of_range("SparseSet::insert: value超出范围");
     }
     if (sparse_[v] == nullValue)
     {
          dense_.push_back(v);
          sparse_[v] = dense_.size() - 1;
     }
}

void SparseSet::erase(uint32_t v)
{
     if (v < sparse_.size() && sparse_[v] != nullValue)
     {
          sparse_[dense_.back()] = sparse_[v];
          std::swap(dense_[sparse_[v]], dense_.back());
          dense_.pop_back();
          sparse_[v] = nullValue;
     }
}

bool SparseSet::find(uint32_t v) const
{
     if (v >= sparse_.size())
     {
          return false;
     }
     return sparse_[v] != nullValue;
}
