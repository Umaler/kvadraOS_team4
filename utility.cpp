#include "utility.hpp"

#include <set>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace chk {

std::string toJSONString(const std::map<std::string, std::vector<std::filesystem::path>>& data) {
    return json{data}.dump();
}

}
