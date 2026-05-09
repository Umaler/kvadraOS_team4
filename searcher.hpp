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

    virtual std::map<std::string, std::vector<std::filesystem::path>> getMediaFiles(std::filesystem::path dirToCheck) = 0;
    virtual ~ISearcher() = default;

};

template <bool RecursiveSearch = false>
class OneThreadSearcher : public ISearcher {
public:
    OneThreadSearcher(std::unique_ptr<IFilter> filter) :
        _filter(std::move(filter))
    {}

    std::map<std::string, std::vector<std::filesystem::path>> getMediaFiles(std::filesystem::path dirToCheck) override {
        if(!std::filesystem::is_directory(dirToCheck)) {
            throw std::invalid_argument("passed path is not directory");
        }

        std::map<std::string, std::vector<std::filesystem::path>> result;

        if constexpr (RecursiveSearch) {
            //auto onFile = [&result, this](std::filesystem::path file) -> std::vector<std::filesystem::path> {
            //    dbfs
            //}

            return result;
        }
        else { // not recutrsive search
            for (const auto& dir_entry : std::filesystem::directory_iterator{dirToCheck}) {
                auto path = dir_entry.path();
                std::optional<std::string_view> fileType = _filter->getFileType(path);
                if(fileType) {
                    result[std::string(fileType->begin(), fileType->end())].push_back(path);
                }
            }
        }

        return result;
    }

protected:
    std::unique_ptr<IFilter> _filter;

};

}

#endif // SEARCHER_HPP
