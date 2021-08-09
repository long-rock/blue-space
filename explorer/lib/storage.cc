#include "explorer/storage.h"

#include "miner/common/miner.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <leveldb/db.h>

#include <memory>
#include <sstream>
#include <stdexcept>

using namespace explorer;

InMemoryStorage::InMemoryStorage() : explored_()
{
}

std::optional<miner::common::WorkItem> InMemoryStorage::get(const miner::common::Coordinate &coord) const
{
    auto item = explored_.find(coord);
    if (item == explored_.end())
    {
        return {};
    }
    return item->second;
}

void InMemoryStorage::store(miner::common::WorkItem item)
{
    auto coord = miner::common::Coordinate(item.x, item.y);
    explored_[coord] = item;
}

FileStorage::FileStorage(const std::string &filename)
{
    leveldb::Options db_options;
    db_options.create_if_missing = true;
    auto status = leveldb::DB::Open(db_options, filename, &db_);
    if (!status.ok())
    {
        throw std::runtime_error("Could not create file storage");
    }
}

FileStorage::~FileStorage()
{
    delete db_;
}

namespace
{

template <class Archive> void serialize_coordinate(Archive &ar, const miner::common::Coordinate &coord)
{
    ar(coord.x, coord.y);
}

leveldb::Slice coordinate_slice(const miner::common::Coordinate &coord)
{
    std::stringstream ss(std::ios::binary | std::ios::out | std::ios::in);
    cereal::BinaryOutputArchive archive(ss);
    serialize_coordinate(archive, coord);
    return leveldb::Slice(ss.str());
}

leveldb::Slice coordinate_slice(int64_t x, int64_t y)
{
    miner::common::Coordinate coord(x, y);
    return coordinate_slice(coord);
}

template <class Archive> void serialize_work_item(Archive &ar, const miner::common::WorkItem &item)
{
    ar(item.x, item.y, item.is_planet, item.hash);
}

template <class Archive> void serialize_work_item(Archive &ar, miner::common::WorkItem &item)
{
    ar(item.x, item.y, item.is_planet, item.hash);
}


leveldb::Slice work_item_slice(const miner::common::WorkItem &item)
{
    std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
    cereal::BinaryOutputArchive archive(ss);
    serialize_work_item(archive, item);
    return leveldb::Slice(ss.str());
}

} // namespace

std::optional<miner::common::WorkItem> FileStorage::get(const miner::common::Coordinate &coord) const
{
    std::string value;
    auto key = coordinate_slice(coord);
    auto status = db_->Get(leveldb::ReadOptions(), key, &value);
    if (status.IsNotFound())
    {
        return {};
    }
    std::stringstream ss(std::ios::binary | std::ios::out | std::ios::out);
    ss << value;
    cereal::BinaryInputArchive archive(ss);
    miner::common::WorkItem item;
    serialize_work_item(archive, item);
    return item;
}

void FileStorage::store(miner::common::WorkItem item)
{
    auto key = coordinate_slice(item.x, item.y);
    auto value = work_item_slice(item);
    db_->Put(leveldb::WriteOptions(), key, value);
}