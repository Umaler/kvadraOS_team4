#ifndef FILTER_HPP
#define FILTER_HPP

#include <filesystem>
#include <flat_map>
#include <optional>
#include <string>
#include <string_view>

namespace chk {

class IFilter {
public:

    virtual std::optional<std::string_view> getFileType(std::filesystem::path file) const = 0;

};

template<typename mappingContainerT = std::flat_map<std::string_view, std::string_view>>
class BasicFilter : public IFilter {
public:

    std::optional<std::string_view> getFileType(std::filesystem::path file) const override {
        if(!std::filesystem::is_regular_file(file)) {
            return std::nullopt;
        }

        std::string ext = file.extension();

        std::string_view extVal;

        if(ext.size() > 0 &&
           ext[0] == '.'
          ) {
            // because path.extension return extension with dot
            extVal = std::string_view(++(ext.begin()), ext.end());
        }
        else {
            extVal = std::string_view(ext.begin(), ext.end());
        }

        auto typeIter = suffixToType.find(extVal);
        if(typeIter == suffixToType.end()) {
            return std::nullopt;
        }
        else {
            return typeIter->second;
        }
    }

protected:
    const mappingContainerT suffixToType {
        {"mp3", "audio"},
        {"wav", "audio"},
        {"mpg", "video"},
        {"jpeg", "image"},
        {"png", "image"}
    };

};

}

#endif // FILTER_HPP
