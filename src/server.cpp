#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>
#include "json.hpp"

using json = nlohmann::json;
namespace beast = boost::beast;           
namespace http = beast::http;             
namespace net = boost::asio;               

// CacheEntry structure to hold key-value pairs and tags
struct CacheEntry {
    std::string key;
    std::string value;
    std::vector<std::string> tags;

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

// Function to handle requests
void handle_request(Cache& cache, beast::tcp_stream& stream, http::request<http::string_body> req) {
    http::response<http::string_body> res;
    if (req.method() == http::verb::post && req.target() == "/cache") {
        auto json_data = json::parse(req.body());
        CacheEntry entry(json_data["key"], json_data["value"], json_data["tags"].get<std::vector<std::string>>());
        cache.store(entry);
        res = http::response<http::string_body>(http::status::ok, req.version());
        res.set(http::field::content_type, "text/plain");
        res.body() = "Data stored successfully";
        res.prepare_payload();
    } else if (req.method() == http::verb::get && req.target().starts_with("/cache/")) {
        std::string key = req.target().substr(7); // Remove "/cache/" prefix
        auto entry = cache.get(key);
        if (entry) {
            json response_json = {
                {"key", entry->key},
                {"value", entry->value},
                {"tags", entry->tags}
            };
            res = http::response<http::string_body>(http::status::ok, req.version());
            res.set(http::field::content_type, "application/json");
            res.body() = response_json.dump();
            res.prepare_payload();
        } else {
            res = http::response<http::string_body>(http::status::not_found, req.version());
            res.set(http::field::content_type, "text/plain");
            res.body() = "Key not found";
            res.prepare_payload();
        }
    } else {
        res = http::response<http::string_body>(http::status::bad_request, req.version());
        res.set(http::field::content_type, "text/plain");
        res.body() = "Invalid request";
        res.prepare_payload();
    }
    beast::http::write(stream, res);
}

// Main function to run the server
int main() {
    try {
        net::io_context ioc;
        beast::tcp_acceptor acceptor(ioc, {net::ip::make_address("0.0.0.0"), 5000});
        Cache cache;

        for (;;) {
            beast::tcp_stream stream(ioc);
            acceptor.accept(stream);
            http::request<http::string_body> req;
            beast::flat_buffer buffer;
            http::read(stream, buffer, req);
            handle_request(cache, stream, std::move(req));
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
