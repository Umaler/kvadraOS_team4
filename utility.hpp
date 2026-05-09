#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <filesystem>
#include <functional>
#include <map>
#include <stack>
#include <set>
#include <string>
#include <queue>
#include <vector>

namespace chk {

// deep/breadth first search
template<typename T, bool deepSearch = true>
void dbfs(T firstNode, std::function<std::vector<T>(T)> onNode) {
    // onNode must accept node and return list of connected nodes

    std::set<T> foundNodes;
    auto nodesToVisit = [](){
        if constexpr (deepSearch) {
            return std::stack<T>();
        }
        else {
            return std::queue<T>();
        }
    }();
    nodesToVisit.push(firstNode);

    while(!nodesToVisit.empty()) {
        T anotherNode = nodesToVisit.pop();
        if(foundNodes.find(anotherNode) != foundNodes.end()) {
            continue;
        }
        foundNodes.insert(anotherNode);

        auto newNodes = onNode(anotherNode);
        for(const auto& node : newNodes) {
            nodesToVisit.push(node);
        }
    }
}

std::string toJSONString(const std::map<std::string, std::vector<std::filesystem::path>>& data);

}

#endif // UTILITY_HPP
