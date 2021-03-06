#include "http_server.h"
#include "media_manager.h"
#include "httplib.h"
#include "http_stream_cb.h"
#include <string>
#include <future>
#include <chrono>
#include <regex>
#include <stdexcept>
#include "log.h"

using namespace httplib;
using namespace std;

#define TAG "HttpServer"

namespace xport
{

class HttpServerImpl {
    shared_ptr<MediaManager> mMediaManager{new MediaManager};
    Server mServer;
    thread mThd;
    promise<bool> mThdRet;
private:
    bool prepareServer(){
        if (!mMediaManager->init()){
            loge("init media manager failed");
            return false;
        }
        mServer.Get("/media(/.*)?", bind(&HttpServerImpl::onNewMediaRequest, this, placeholders::_1, placeholders::_2));
        return true;
    }
    bool startServer(const char* host, int port){
        return mServer.listen(host, port);
    }
public:
    bool startAsync(const char* host, int port) {
        //logi("startAsync");

        if (!prepareServer()){
            return false;
        }

        mThd = thread([&](HttpServerImpl* impl){
            impl->mThdRet.set_value(impl->startServer(host, port));
        }, this);

        auto getThreadRunRet = mThdRet.get_future();

        future_status fs;
        do{
            fs = getThreadRunRet.wait_for(chrono::milliseconds(10));
            if (fs == future_status::ready)
                return getThreadRunRet.get();
            if (mServer.is_running())
                return true;
        }while(true);

        return true;
    }
    bool start(const char* host, int port) {
        //logi("start");

        if (!prepareServer()){
			printf("prepare failed\n");
            return false;
        }
        
        return startServer(host, port);
    }
    void stop() {
        //logi("stop");
        mServer.stop();

        if (mThd.joinable()){
            mThd.join();
        }

        mMediaManager->uninit();
    }
    
    void onNewMediaRequest(const Request &req, Response &res) {
        auto sessionId = getSessionId(req);
        MediaRequest mediaRequest((MediaRequestImpl*)&req);

        shared_ptr<ReadMedia> media;
        if (sessionId.empty()) {
            media = mMediaManager->createMedia(mediaRequest);
        }else{
            media = mMediaManager->getMediaOrCreate(stoi(sessionId), mediaRequest);
        }

        if (media){
            //logi("new request: sessionId=%s, path=%s, response media: %d, seekable=%d", sessionId.c_str(), req.path.c_str(), media->id(), media->seekable());
            makeMediaResponse(req, res, media);
        }else{
            logi("new request: sessionId=%s, path=%s, response media:null", sessionId.c_str(), req.path.c_str());
            makeFailResponse(res);
        }
    }

    inline string getSessionId(const Request& req){
        regex idPattern("id=(-?\\d+)");

        smatch sm;
        auto cookie = req.get_header_value("Cookie");
        if (regex_search(cookie, sm, idPattern)) {
			//logi("cookie: %s",sm.str(1));
            return sm.str(1);
        }
        return "";
    }

    inline void getRange(const Request& req, int64_t& from, int64_t& to){
        from = 0;
        to = -1;

        regex rangePattern("bytes=(\\d+)-(\\d+)?");

        smatch sm;
        auto range = req.get_header_value("Range");
        if (regex_search(range, sm, rangePattern)){
            auto fromStr = sm.str(1);
            if (!fromStr.empty()){
                try{
                    from = stoi(fromStr);
                }catch(invalid_argument& e){
                    loge("invalid 'from': %s", e.what());
                    from = 0;
                }
            }
            
            if (sm.size()>=3){
                auto toStr = sm.str(2);
                if (!toStr.empty()){
                    try{
                        to = stoi(toStr);
                    }catch(invalid_argument& e){
                        loge("invlaid 'to': %s", e.what());
                        to = -1;
                    }
                }
            }
        }
    }

    void makeMediaResponse(const Request &req, Response& res, const shared_ptr<ReadMedia>& media) {
        int64_t from = 0;
        int64_t to  = -1;
        getRange(req, from, to);

        auto mediaId = to_string(media->id());
        auto tag = mediaId.c_str();
        //logti(tag, "request media range from %" PRId64 " to %" PRId64, from, to);
        auto watchDog = mMediaManager->obtainMediaWatchDog();
        auto reader = media->createReader(watchDog, from, to);
        if (!reader){
            logti(tag, "no reader can be created");
            res.status = 416;
            return;
        }

        shared_ptr<HttpStreamCB> streamcb(new HttpStreamCB(reader));
        res.streamcb = std::bind(&HttpStreamCB::read, streamcb, placeholders::_1);
        res.set_header("Accept-Ranges","bytes");
        res.set_header("Content-Type", media->mimeType());
        res.set_header("Set-Cookie", "id="+to_string(media->id())+";path="+req.path);

        if (media->seekable()){
            if (media->size() < 0 || reader->to() < 0 || reader->from() < 0){
                loge("seekable media with invalid read parameters: size=%d, from=%" PRId64 ", to=%" PRId64, media->size(), reader->from(), reader->to());
                res.status = 500;
                return;
            }

            res.status = 206;//here
            auto length = reader->to() - reader->from() + 1;
            res.set_header("Content-Length", to_string(length));
            res.set_header("Content-Range", "bytes " + to_string(reader->from()) + "-" + to_string(reader->to()) + "/" + to_string(media->size()));
        }
        else{
            res.status = 200;
            res.set_header("Content-Range", "bytes 0-");
        }
    }

    void makeFailResponse(Response& res){
        res.status = 404;
    }
};

HttpServer::HttpServer()
    :mImpl(new HttpServerImpl){

}

HttpServer::~HttpServer() {
    delete mImpl;
}

bool HttpServer::start(const char* host, int port) {
    return mImpl->start(host, port);
}

bool HttpServer::startAsync(const char* host, int port) {
    return mImpl->startAsync(host, port);
}

void HttpServer::stop(){
    return mImpl->stop();
}

} // namespace xport