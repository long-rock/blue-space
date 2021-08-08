#include "explorer/explorer.h"

#include <cassert>
#include <iostream>

using namespace explorer;
using namespace miner::common;

Explorer::Explorer(Storage::ptr storage, Coordinate origin) : storage_(storage), origin_(origin)
{
}

void Explorer::set_origin(Coordinate coord)
{
    origin_ = coord;
}

SpiralExplorer::SpiralExplorer(Storage::ptr storage, Coordinate origin) : Explorer(storage, origin), current_(origin)
{
}

namespace
{
bool is_origin(const Coordinate &origin, const Coordinate &curr)
{
    return origin.x == curr.x && origin.y == curr.y;
}

bool is_top_left(const Coordinate &origin, const Coordinate &curr)
{
    return (curr.y - curr.x > origin.y - origin.x) && (curr.y + curr.x >= origin.y + origin.x);
}

bool is_top_right(const Coordinate &origin, const Coordinate &curr)
{
    return (curr.x + curr.y > origin.x + origin.y) && (curr.y - curr.x <= origin.y - origin.x);
}

bool is_bottom_left(const Coordinate &origin, const Coordinate &curr)
{
    return (curr.x + curr.y < origin.x + origin.y) && (curr.y - curr.x >= origin.y - origin.x);
}

bool is_bottom_right(const Coordinate &origin, const Coordinate &curr)
{
    return (curr.x + curr.y <= origin.x + origin.y) && (curr.y - curr.x < origin.y - origin.x);
}
} // namespace

std::optional<Coordinate> SpiralExplorer::next()
{
    auto next = next_coordinate();
    while (storage()->get(next)) {
        next = next_coordinate();
    }
    return next;
}

Coordinate SpiralExplorer::next_coordinate()
{
    auto orig = origin();

    Coordinate next(current_.x, current_.y);

    if (is_origin(orig, current_))
    {
        current_.y += 1;
    }
    else if (is_top_left(orig, current_))
    {
        // break the circle
        if (current_.x + current_.y == orig.x + orig.y)
        {
            current_.y += 1;
        }
        else
        {
            current_.x += 1;
        }
    }
    else if (is_top_right(orig, current_))
    {
        current_.y -= 1;
    }
    else if (is_bottom_right(orig, current_))
    {
        current_.x -= 1;
    }
    else
    {
        assert(is_bottom_left(orig, current_));
        current_.y += 1;
    }

    return next;
}
