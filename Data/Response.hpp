#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>

class Response {
public:
    std::string version = "HTTP/1.1";
    int status_code = 200;
    std::string status_message = "OK";
    
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    Response() = default;

    // Helper to easily add headers
    void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    // Helper to set the body which automatically sets the Content-Length header
    void set_body(const std::string& content) {
        body = content;
        set_header("Content-Length", std::to_string(body.length()));
    }

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        buffer.reserve(2048 + body.size());

        auto append = [&buffer](const std::string& str) {
            buffer.insert(buffer.end(), str.begin(), str.end());
        };

        append(version);
        append(" ");
        append(std::to_string(status_code));
        append(" ");
        append(status_message);
        append("\r\n");

        for (const auto& [key, value] : headers) {
            append(key);
            append(": ");
            append(value);
            append("\r\n");
        }
        append("\r\n");
        append(body);
        
        return buffer;
    }

    std::string to_string() const {
        std::vector<uint8_t> bytes_serialized = serialize();
        return std::string(bytes_serialized.begin(), bytes_serialized.end());
    }
};