#include <iostream>

#include "configs_handler.hpp"
#include "utility.hpp"
#include "system_specific.hpp"
#include "searcher.hpp"

int main() {
    using namespace std::chrono_literals;

    /*chk::ConfigsHandler handler(chk::getHomeDir(), 2s);

    std::cout << "dir: " << handler.getDirToCheck() << std::endl
              << "duration: " << handler.getChecksInterval() << std::endl;*/

    chk::OneThreadSearcher<> searcher(chk::getHomeDir(), std::make_unique<chk::BasicFilter<>>());
    auto res = searcher.getMediaFiles();
    std::string js = chk::toJSONString(res);
    std::cout << js << std::endl;
}
