#ifndef FILTER_HPP
#define FILTER_HPP

#include <filesystem>
#include <flat_map>
#include <flat_set>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <magic.h>

namespace chk {

class IFilter {
public:

    virtual std::optional<std::string_view> getFileType(std::filesystem::path file) const = 0;
    virtual ~IFilter() = default;

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

class MagicFilter : public IFilter {
public:
    MagicFilter() {
        _magicCookie = magic_open(MAGIC_MIME_TYPE);
        if (_magicCookie == 0) {
            throw std::runtime_error("Unable to initialize magic library");
        }

        if (magic_load(_magicCookie, 0) != 0) {
            std::string error = std::string{magic_error(_magicCookie)};
            magic_close(_magicCookie);
            throw std::runtime_error(std::string{"Cannot load magic database:"} + error);
        }

    }

    std::optional<std::string_view> getFileType(std::filesystem::path file) const override {
        const char *mimeType;
        mimeType = magic_file(_magicCookie, file.c_str());
        if (mimeType == 0) {
            throw std::runtime_error(std::string{"Cannot determine file type:"} + std::string{magic_error(_magicCookie)});
        }
        std::string_view type = getTopLevelType({mimeType});

        if(allowedMIMETypes.contains(type)) {
            return type;
        }
        else {
            return std::nullopt;
        }
    }

    ~MagicFilter() {
        magic_close(_magicCookie);
    }

private:

    static std::string_view getTopLevelType(std::string_view mimeType) {
        if(size_t slashPos = mimeType.find('/'); slashPos != mimeType.npos) {
            return mimeType.substr(0, slashPos);
        }
        else {
            return mimeType;
        }
    }

    const std::flat_set<std::string_view> allowedMIMETypes {
        "audio",
        "image",
        "video"
    };

    magic_t _magicCookie;

};

}

#endif // FILTER_HPP
