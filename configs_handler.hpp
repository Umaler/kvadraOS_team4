#ifndef CONFIGS_HANDLER_HPP
#define CONFIGS_HANDLER_HPP

#include <chrono>
#include <filesystem>

namespace chk {

class ConfigsHandler {
public:

    using durationT = std::chrono::steady_clock::duration;

    ConfigsHandler(std::filesystem::path dirToCheck, durationT checksInterval, bool useWeb, bool profiling, bool recursive) :
        _dirToCheck(dirToCheck),
        _checksInterval(checksInterval),
        _useWeb(useWeb),
        _profiling(profiling),
        _recursive(recursive)
    {}

    std::filesystem::path getDirToCheck() const {
        return _dirToCheck;
    }

    durationT getChecksInterval() const {
        return _checksInterval;
    }

    bool useAccessOverWeb() const {
        return _useWeb;
    }

    bool profiling() const {
        return _profiling;
    }

    bool recursive() const {
        return _recursive;
    }

private:
    const std::filesystem::path _dirToCheck;
    const durationT _checksInterval;
    const bool _useWeb = false;
    const bool _profiling = false;
    const bool _recursive = false;

};

}

#endif // CONFIGS_HANDLER_HPP
