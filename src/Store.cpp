#include "Store.h"

bool Store::set(const std::string& key, const std::string& value)
{
    Shard& shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);
    shard.storage_.try_emplace(key, value);
    return true;
}

std::optional<std::string> Store::get(const std::string& key) const
{
    const Shard& shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);

    auto it = shard.storage_.find(key);
    if (it == shard.storage_.end())
    {
        return std::nullopt;
    }
    return it->second;
}

bool Store::del(const std::string& key)
{
    Shard& shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);
    return shard.storage_.erase(key) > 0;
}

void Store::load(const std::unordered_map<std::string, std::string>& data)
{
    for (const auto& [key, value] : data)
    {
        set(key, value);
    }
}

std::unordered_map<std::string, std::string> Store::save()
{
    std::unordered_map<std::string, std::string> result;
    for (auto& shard : shards_)
    {
        std::lock_guard<std::mutex> lock(shard.mutex_);
        for (const auto& [key, value] : shard.storage_)
        {
            result.emplace(key, value);
        }
    }
    return result;
}

Store::Shard& Store::shard_for(const std::string& key)
{
    return shards_[std::hash<std::string>{}(key) % SHARDS];
}

const Store::Shard& Store::shard_for(const std::string& key) const
{
    return shards_[std::hash<std::string>{}(key) % SHARDS];
}
