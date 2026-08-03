//
// Created by frank on 02/08/2026.
//

#include "Store.h"

bool Store::set(const std::string &key, const std::string &value) {
    storage.try_emplace(key, value);
    return true;
}

std::optional<std::string> Store::get(const std::string &key) const {
    auto it = storage.find(key);
    if (it == storage.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool Store::del(const std::string &key) {
    return storage.erase(key) > 0;
}
