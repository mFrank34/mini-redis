#ifndef STORE_H
#define STORE_H
#include <array>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>

class Store
{
public:
    explicit Store(size_t max_entries);
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

    /**
     * take an input to populate the key register
     * @param data data loaded from file
     */
    void load(const std::unordered_map<std::string, std::string>& data);

    /**
     * system design to create a snap shot of memory register
     * @return returns file ready to exported
     */
    std::unordered_map<std::string, std::string> save();

private:
    /* Creating Sharding support for read access */

    /** creating a limit on how many shards can be accessed at time */
    static constexpr size_t SHARDS = 16;

    /** struct for handling shards */
    struct Shard
    {
        struct Node
        {
            std::string key;
            std::string value;
        };

        mutable std::mutex mutex_;
        std::list<Node> lru_list_;
        std::unordered_map<std::string, std::list<Node>::iterator> storage_;
        size_t max_entries_;
    };

    std::array<Shard, SHARDS> shards_;

    /**
     * looking information within a shards
     * @param key instance searching within the decentralized shard containers
     * @return the value that key holds in container
     */
    Shard& shard_for(const std::string& key);

    /**
     * looking information within a shards just as static function
     * @param key instance searching within the decentralized shard containers
     * @return the value that key holds in container
     */
    const Shard& shard_for(const std::string& key) const;
};

#endif //STORE_H
