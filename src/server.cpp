#include "crow_all.h"
#include "json.hpp"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>

using json = nlohmann::json;

// CacheEntry structure to hold key-value pairs and tags
struct CacheEntry {
    std::string key;
    std::string value;
    std::vector<std::string> tags;

    // Default constructor
    CacheEntry() : key(""), value(""), tags({}) {}

    CacheEntry(const std::string& k, const std::string& v, const std::vector<std::string>& t)
        : key(k), value(v), tags(t) {}
};

// Cache class to handle cache operations
class Cache {
public:
    void store(const CacheEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_[entry.key] = entry;
    }

    std::shared_ptr<CacheEntry> get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return std::make_shared<CacheEntry>(it->second);
        }
        return nullptr; // Return nullptr if key not found
    }

    void clearByTags(const std::vector<std::string>& tags) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = cache_.begin(); it != cache_.end();) {
            for (const auto& tag : it->second.tags) {
                if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
                    it = cache_.erase(it); // Erase returns the next iterator
                    break; // Exit the inner loop
                }
            }
            if (it != cache_.end()) {
                ++it; // Move to the next item
            }
        }
    }

private:
    std::unordered_map<std::string, CacheEntry> cache_; // Store cache entries
    std::mutex mutex_; // Protect shared resource
};

int main() {
    // Initialize Crow and the Cache
    crow::SimpleApp app;
    auto cache = std::make_shared<Cache>();

    // Endpoint to store cache data
    CROW_ROUTE(app, "/cache").methods("POST"_method)([cache](const crow::request& req) {
        auto json_data = json::parse(req.body);
        CacheEntry entry(json_data["key"], json_data["value"], json_data["tags"].get<std::vector<std::string>>());
        cache->store(entry);
        return crow::response(200, "Data stored successfully");
    });

    // Endpoint to retrieve cache data
    CROW_ROUTE(app, "/cache/<string>").methods("GET"_method)([cache](const std::string& key) {
        auto entry = cache->get(key);
        if (entry) {
            json response_json = {
                {"key", entry->key},
                {"value", entry->value},
                {"tags", entry->tags}
            };
            return crow::response(200, response_json.dump());
        }
        return crow::response(404, "Key not found");
    });

    // Endpoint to clear cache by tags
    CROW_ROUTE(app, "/cache/revalidate").methods("POST"_method)([cache](const crow::request& req) {
        auto json_data = json::parse(req.body);
        auto tags = json_data["tags"].get<std::vector<std::string>>();
        cache->clearByTags(tags);
        return crow::response(200, "Cache cleared successfully");
    });

    // Start the server
    app.port(5000).multithreaded().run();
    return 0;
}
