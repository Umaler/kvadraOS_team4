#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "filter.hpp"

namespace chk {

class ISearcher {
public:

    virtual std::map<std::string, std::vector<std::filesystem::path>> getMediaFiles(std::filesystem::path dirToCheck, std::function<bool()> shouldStop = std::function<bool()>()) = 0;
    virtual ~ISearcher() = default;

};

template <bool RecursiveSearch = false>
class OneThreadSearcher : public ISearcher {
public:
    OneThreadSearcher(std::unique_ptr<IFilter> filter) :
        _filter(std::move(filter))
    {}

    std::map<std::string, std::vector<std::filesystem::path>> getMediaFiles(std::filesystem::path dirToCheck, std::function<bool()> shouldStop = std::function<bool()>()) override {
        if(!std::filesystem::is_directory(dirToCheck)) {
            throw std::invalid_argument("passed path is not directory");
        }

        std::map<std::string, std::vector<std::filesystem::path>> result;

        auto dirIter = [](std::filesystem::path dir) {
            if constexpr (RecursiveSearch) {
                return std::filesystem::recursive_directory_iterator{dir};
            }
            else {
                return std::filesystem::directory_iterator{dir};
            }
        }(dirToCheck);

        for (const auto& dir_entry : dirIter) {
            if(shouldStop()) break;
            auto path = dir_entry.path();
            std::optional<std::string_view> fileType = _filter->getFileType(path);
            if(fileType) {
                result[std::string(fileType->begin(), fileType->end())].push_back(path);
            }
        }

        return result;
    }

protected:
    std::unique_ptr<IFilter> _filter;

};

}

#endif // SEARCHER_HPP
