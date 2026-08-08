#include "Store.h"

Store::Store(size_t max_entries)
{
    size_t per_shard = max_entries / SHARDS;
    if (per_shard == 0) { per_shard = 1; }

    for (auto& shard : shards_)
    {
        shard.max_entries_ = max_entries;
    }
}

bool Store::set(const std::string& key, const std::string& value)
{
    Shard& shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);

    auto it = shard.storage_.find(key);
    if (it != shard.storage_.end())
    {
        it->second->value = value;
        shard.lru_list_.splice(shard.lru_list_.end(), shard.lru_list_, it->second);
        return true;
    }

    if (shard.storage_.size() >= shard.max_entries_)
    {
        auto& lru_node = shard.lru_list_.back();
        shard.storage_.erase(lru_node.key);
        shard.lru_list_.pop_back();
    }

    shard.lru_list_.push_front({key, value});
    shard.storage_[key] = shard.lru_list_.begin();
    return true;
}

std::optional<std::string> Store::get(const std::string& key) const
{
    const Shard& const_shard = shard_for(key);
    Shard& shard = const_cast<Shard&>(const_shard);
    std::lock_guard<std::mutex> lock(shard.mutex_);

    auto it = shard.storage_.find(key);
    if (it == shard.storage_.end())
    {
        return std::nullopt;
    }

    shard.lru_list_.splice(shard.lru_list_.begin(), shard.lru_list_, it->second);
    return it->second->value;
}

bool Store::del(const std::string& key)
{
    Shard& shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);

    auto it = shard.storage_.find(key);
    if (it == shard.storage_.end())
    {
        return false;
    }
    shard.lru_list_.erase(it->second);
    shard.storage_.erase(it);
    return true;
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
        for (const auto& [key, it] : shard.storage_)
        {
            result.emplace(key, it->value);
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
