#ifndef MAIN_WORKER_HPP
#define MAIN_WORKER_HPP

#include "searcher.hpp"

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace chk {

class MainWorker {
public:
    MainWorker(chk::ConfigsHandler configs,
           std::unique_ptr<chk::ISearcher> searcher,
           std::function<void(const std::string&)> onNewResult = std::function<void(const std::string&)>{}
          ) :
        _onNewResult(onNewResult),
        _workingThread(std::bind_front(&MainWorker::workFunc, this), configs, std::move(searcher))
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

    ~MainWorker() {
        stopAndWait();
    }

private:
    void workFunc(std::stop_token stoken, const chk::ConfigsHandler configs, std::unique_ptr<chk::ISearcher> searcher) {
        try {
            std::mutex cv_mutex;
            while(!stoken.stop_requested()) {
                auto files = searcher->getMediaFiles(configs.getDirToCheck(), [&]()->bool{return stoken.stop_requested();});
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
        catch(const std::exception& ex) {
            std::ostringstream bos; //buffer ostream
            bos << "Exception happened in working thread of MainWorker: " << ex.what() << std::endl;
            std::cerr << bos.str();
        }
        catch(...) {
            std::cerr << "Unknown exception happened in working thread of MainWorker\n";
        }
    }

    mutable std::mutex _resultingJSON_m;
    std::string _resultingJSON;

    std::function<void(const std::string&)> _onNewResult;

    std::jthread _workingThread;

};

}

#endif // MAIN_WORKER_HPP
