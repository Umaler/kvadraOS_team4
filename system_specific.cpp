#include "system_specific.hpp"

#include <stdexcept>
#include <system_error>

#ifdef linux

#include <errno.h>
#include <pwd.h>
#include <unistd.h>

#else
    #error The program is implemented only for linux
#endif // linux

namespace chk {

std::filesystem::path getHomeDir() {
    errno = 0;
    // getpwuid should be used instead of just extracting
    // env[$HOME] at least because theoretically $HOME var can be erased
    // just by 'export HOME='
    struct passwd* pwuid = getpwuid(geteuid());
    if(pwuid == 0) {
        throw std::runtime_error("getpwuid return NULL ptr");
    }
    if(errno != 0) {
        throw std::system_error(errno, std::generic_category(), "Fail on getpwuid()");
    }
    if(pwuid->pw_dir == 0) {
        throw std::runtime_error("pwuid->pw_dir is NULL");
    }

    return {pwuid->pw_dir};
}

}
