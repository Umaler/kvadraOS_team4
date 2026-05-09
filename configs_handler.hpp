#ifndef CONFIGS_HANDLER_HPP
#define CONFIGS_HANDLER_HPP

#include <chrono>
#include <filesystem>

namespace chk {

class ConfigsHandler {
public:

    using durationT = std::chrono::steady_clock::duration;

    ConfigsHandler(std::filesystem::path dirToCheck, durationT checksInterval) :
        _dirToCheck(dirToCheck),
        _checksInterval(checksInterval)
    {}

    std::filesystem::path getDirToCheck() const {
        return _dirToCheck;
    }

    durationT getChecksInterval() const {
        return _checksInterval;
    }

private:
    const std::filesystem::path _dirToCheck;
    const durationT _checksInterval;

};

}

#endif // CONFIGS_HANDLER_HPP
