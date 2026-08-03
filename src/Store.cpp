//
// Created by frank on 02/08/2026.
//

#include "Store.h"

bool Store::set(const std::string &key, const std::string &value) {
    Shard &shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);
    storage.try_emplace(key, value);
    return true;
}

std::optional<std::string> Store::get(const std::string &key) const {
    const Shard &shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);

    auto it = shard.storage_.find(key);
    if (it == shard.storage_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool Store::del(const std::string &key) {
    Shard &shard = shard_for(key);
    std::lock_guard<std::mutex> lock(shard.mutex_);
    return storage.erase(key) > 0;
}

Store::Shard& Store::shard_for(const std::string& key)
{
    return shards_[std::hash<std::string>{}(key) % SHARDS];
}

const Store::Shard& Store::shard_for(const std::string& key) const
{
    return shards_[std::hash<std::string>{}(key) % SHARDS];
}



