#include "utility.hpp"

#include <set>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace chk {

std::string toJSONString(const std::map<std::string, std::vector<std::filesystem::path>>& data) {
    return json(data).dump();
}

std::pair<std::string_view, std::optional<std::string_view>> getTwoParts(std::string_view str, char delim) {
    if(size_t delimPos = str.find(delim); delimPos != str.npos) {
        return {str.substr(0, delimPos), str.substr(delimPos+1)};
    }
    else {
        return {str, std::nullopt};
    }
}

}
