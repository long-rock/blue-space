#include "explorer/storage.h"

#include "miner/common/miner.h"

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

std::optional<miner::common::WorkItem> FileStorage::get(const miner::common::Coordinate &coord) const
{
    return {};
}

void FileStorage::store(miner::common::WorkItem item)
{
}