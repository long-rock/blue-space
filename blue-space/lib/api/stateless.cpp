#include "application/api/stateless.h"

using namespace application::api;

std::vector<miner::common::WorkItem> StatelessApi::mine_single(int64_t x, int64_t y, int64_t size, int64_t rarity,
                                                               int64_t key)
{

    std::size_t size_u = size;
    bool inplace = size_u == batch_.size();

    std::size_t idx = 0;
    if (!inplace) {
        batch_ = {};
    }
    for (std::size_t i = 0; i < size_u; ++i)
    {
        for (std::size_t j = 0; j < size_u; ++j)
        {
            if (inplace)
            {
                batch_[idx].x = x + i;
                batch_[idx].y = y + i;
                batch_[idx].is_planet = false;
                idx += 1;
            }
            else
            {
                miner::common::WorkItem item;
                item.x = x + i;
                item.y = y + j;
                batch_.push_back(item);
            }
        }
    }

    miner_->mine_batch(batch_, rarity, key);

    std::vector<miner::common::WorkItem> planets;
    for (auto &item: batch_)
    {
        if (item.is_planet)
        {
            planets.push_back(item);
        }
    }
    return planets;
}