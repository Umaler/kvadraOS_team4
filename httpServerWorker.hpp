#ifndef HTTP_SERVER_WORKER_HPP
#define HTTP_SERVER_WORKER_HPP

#include <thread>
#include <mutex>
#include <string>

#include <httplib.h>

namespace chk {

class HTTPServerWorker {
public:
    HTTPServerWorker(std::string ip = "0.0.0.0", unsigned int port = 1234) {
        _workerThread = std::jthread([this, ip, port](){
            try {
                _server.Get("/media_files", [this](const httplib::Request&, httplib::Response& res) {
                    std::unique_lock lock(_jsonToReturn_m);
                    res.set_content(_jsonToReturn, "application/json");
                });
                _server.listen(ip, port);
            }
            catch(const std::exception& ex) {
                std::ostringstream bos; //buffer ostream
                bos << "Exception happened in working thread of HTTPServerWorker: " << ex.what() << std::endl;
                std::cerr << bos.str();
            }
            catch(...) {
                std::cerr << "Unknown exception happened in working thread of HTTPServerWorker\n";
            }
        });
    }

    void setJson(const std::string& json) {
        std::unique_lock lock(_jsonToReturn_m);
        _jsonToReturn = json;
    }

    ~HTTPServerWorker() {
        _server.stop();
    }

private:
    std::mutex _jsonToReturn_m;
    std::string _jsonToReturn;

    std::jthread _workerThread;

    httplib::Server _server;

};

}

#endif // HTTP_SERVER_WORKER_HPP
