#ifndef STORE_H
#define STORE_H
#include <string>
#include <unordered_map>
#include <optional>

class Store {
public:
    Store() = default;
    ~Store() = default;

    /**
     * Set an item within storage.
     * @param key location at which item is stored
     * @param value content of key
     * @return true if the operation succeeded
     */
    bool set(const std::string& key, const std::string& value);

    /**
     * Fetch content of key.
     * @param key element to be inspected
     * @return value if the key exists, std::nullopt otherwise
     */
    std::optional<std::string> get(const std::string& key) const;

    /**
     * Delete key and content.
     * @param key element to be deleted
     * @return true if the key existed and was deleted
     */
    bool del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> storage;
};

#endif //STORE_H