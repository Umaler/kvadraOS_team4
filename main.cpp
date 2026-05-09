#include <iostream>

#include "configs_handler.hpp"
#include "utility.hpp"
#include "system_specific.hpp"
#include "searcher.hpp"

#include <fstream>

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class Worker {
public:
    Worker(chk::ConfigsHandler configs,
           std::unique_ptr<chk::ISearcher> searcher,
           std::function<void(const std::string&)> onNewResult = std::function<void(const std::string&)>{}
          ) :
        _onNewResult(onNewResult),
        _workingThread(std::bind_front(&Worker::workFunc, this), configs, std::move(searcher))
    {}

    void requestStop() {
        _workingThread.request_stop();
    }

    void stopAndWait() {
        requestStop();
        _workingThread.join();
    }

    std::string getJSON() const {
        std::unique_lock lock(_resultingJSON_m);
        return _resultingJSON;
    }

    ~Worker() {
        stopAndWait();
    }

private:
    void workFunc(std::stop_token stoken, const chk::ConfigsHandler configs, std::unique_ptr<chk::ISearcher> searcher) {
        std::mutex cv_mutex;
        while(!stoken.stop_requested()) {
            auto files = searcher->getMediaFiles(configs.getDirToCheck());
            std::string newJSON = chk::toJSONString(files);
            if(_onNewResult) _onNewResult(newJSON);

            {
                std::unique_lock lock(_resultingJSON_m);
                _resultingJSON = newJSON;
            }

            // sleep for required time
            std::unique_lock lock(cv_mutex);
            std::condition_variable_any().wait_for(lock, stoken, configs.getChecksInterval(), []{ return false; });
        }
    }

    mutable std::mutex _resultingJSON_m;
    std::string _resultingJSON;

    std::function<void(const std::string&)> _onNewResult;

    std::jthread _workingThread;

};

int main() {
    using namespace chk;
    using namespace std::chrono_literals;

    chk::ConfigsHandler handler(chk::getHomeDir(), 5s);
    std::filesystem::path resultingFile = chk::getHomeDir() / ".media_files";
    Worker worker(handler,
                  std::make_unique<OneThreadSearcher<>>(
                    std::make_unique<BasicFilter<>>()
                  ),
                  [resultingFile](std::string str) {
                      std::ofstream file(resultingFile);
                      file << str;
                  }
                 );
    std::this_thread::sleep_for(22s);
}
