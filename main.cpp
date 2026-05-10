#include <iostream>

#include "configs_handler.hpp"
#include "httpServerWorker.hpp"
#include "mainWorker.hpp"
#include "utility.hpp"
#include "system_specific.hpp"
#include "searcher.hpp"

#include <fstream>
#include <sstream>

namespace chk {

class CmdLineArgInvalid : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

}

chk::ConfigsHandler getConfigs(int argc, char *argv[]) {
    using namespace std::chrono_literals;

    auto homeDir = chk::getHomeDir();
    auto period = 5s;
    bool useWeb = false;
    bool profiling = false;
    bool recursive = false;

    for(int i = 1; i < argc; i++) {
        auto [command, value] = chk::getTwoParts(argv[i], '=');
        if(command == "--dir") {
            if(!value) {
                throw chk::CmdLineArgInvalid(std::string{"No value for "} + std::string(command.begin(), command.end()));
            }
            std::istringstream is(std::string(value->begin(), value->end()));
            if(!(is >> homeDir)) {
                throw chk::CmdLineArgInvalid("Incorrect value for --dir");
            }
        }
        else if(command == "--period") {
            if(!value) {
                throw chk::CmdLineArgInvalid(std::string{"No value for "} + std::string(command.begin(), command.end()));
            }
            std::istringstream is(std::string(value->begin(), value->end()));
            size_t seconds;
            if(!(is >> seconds)) {
                period = std::chrono::seconds(seconds);
                throw chk::CmdLineArgInvalid("Incorrect value for --period");
            }
        }
        else if(command == "--recursive") {
            recursive = true;
        }
        else if(command == "--web") {
            useWeb = true;
        }
        else if(command == "--profiling") {
            profiling = true;
        }
        else {
            throw chk::CmdLineArgInvalid(std::string{"No such parameter as "} + std::string(command.begin(), command.end()));
        }
    }

    return chk::ConfigsHandler(homeDir, period, useWeb, profiling, recursive);
}

int main(int argc, char *argv[]) {
    using namespace chk;

    try {

        auto configs = getConfigs(argc, argv);

        std::unique_ptr<HTTPServerWorker> httpServer;
        if(configs.useAccessOverWeb()) {
            httpServer = std::make_unique<HTTPServerWorker>();
        }

        std::unique_ptr<ISearcher> searcher;
        if(configs.recursive()) {
            searcher = std::make_unique<OneThreadSearcher<true>>(
                          std::make_unique<MagicFilter>()
                       );
        }
        else {
            searcher = std::make_unique<OneThreadSearcher<false>>(
                          std::make_unique<MagicFilter>()
                       );
        }

        std::filesystem::path resultingFilePath = chk::getHomeDir() / ".media_files";
        MainWorker worker(configs,
                      std::move(searcher),
                      [resultingFilePath, configs, &httpServer](std::string str) {
                          if(httpServer) {
                              httpServer->setJson(str);
                          }
                          else {
                              std::ofstream file(resultingFilePath);
                              file << str;
                          }
                      }
                     );

        if(configs.profiling()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else {
            std::cout << "Enter \"exit\" to stop" << std::endl;
            while(true) {
                std::string str;
                std::cin >> str;
                if(str == "exit") {
                    break;
                }
            }
        }

    }
    catch(const CmdLineArgInvalid& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
    catch(const std::exception& ex) {
        std::cerr << "Exception caught in main: " << ex.what() << std::endl;
        return -2;
    }
    catch(...) {
        std::cerr << "Unknown exception caught in main\n";
        return -3;
    }
    return 0;
}
