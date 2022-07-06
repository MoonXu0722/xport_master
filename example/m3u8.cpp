#include "../src/xport.h"
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;
using namespace xport;

#define RES_DIR "./"
static const int VIDEO_COUNT = 18;
static const char* VIDEOS[VIDEO_COUNT] = {"hyj0.ts", "hyj1.ts", "hyj2.ts", "hyj3.ts", "hyj4.ts", \
	"hyj5.ts","hyj6.ts", "hyj7.ts", "hyj8.ts","hyj9.ts", "hyj10.ts", "hyj11.ts", "hyj12.ts",\
	"hyj13.ts", "hyj14.ts", "hyj15.ts", "hyj16.ts","hyj17.ts"};
//	static const int VIDEO_COUNT = 1;
//static const char* VIDEOS[VIDEO_COUNT] = {"student.ts"};

//M3U8Media 把多个视频片段当做一个连续的视频流推送（类似M3U8)
class M3U8Media : public StreamMedia{
    FILE* mFp{nullptr};
    int mCurVideo{0};
public:
	M3U8Media(){
		std::cout << "M3U8Media construction" << std::endl;
	}
	~M3U8Media(){
		std::cout << "M3U8Media deconstruction" << std::endl;
	}
    bool open(){
        string path{RES_DIR};
        path+=VIDEOS[mCurVideo];
        printf("open %s\n", path.c_str());
        mFp = fopen(path.c_str(), "rb");
        return mFp != nullptr;
    }

    void close(){
        if (mFp){
            fclose(mFp);
            mFp = nullptr;
        }
    }

    string read(){
        const int unitSize = 4096;
        char buf[unitSize];
        
        auto ret = readBuf(buf, unitSize);
        if (ret > 0){
            return string(buf, ret);
        }else if (ret == 0){
            printf("eof\n");
            return "";
        }else{
            printf("error\n");
            return "";
        }

        return "";
    }

    bool reset(){
        close();
        mCurVideo = 0;
        return open();
    }

private:
    int nextVideo(){
        ++mCurVideo;
        if (mCurVideo >= VIDEO_COUNT)
            return 0;
        
        return open() ? 1 : -1;
    }

    int readBuf(char* buf, int size) {
        auto ret = fread(buf, 1, size, mFp);

        if (ret == 0) {
            auto nextRet = nextVideo();
            if (nextRet <= 0)
                return nextRet;
            else
                return readBuf(buf, size);
        }

        return ret;
    }
};

//http://127.0.0.1:8000/media/m3u8
class M3U8MediaCreator : public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        auto segs = req.segments();
        return segs[1] == "m3u8";
    }

    IMedia* create(MediaRequest& req){
        return new M3U8Media;
    }
};

#ifndef EXCLUDE_MAIN
int main(int argc, char* argv[]){
    releaseOwnershipToRegsiter(new M3U8MediaCreator);
	  std::string ip;
    int port;
    if(argc < 2){
        ip = "192.168.20.129";
        port = 8000;
    } else if(argc == 2)
    {
        ip = argv[1];
        port = 8000;
    } else if (argc == 3)
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }
    //启动http服务
    HttpServer server;
    if (!server.start(ip.c_str(), port)){
        printf("start http server failed\n");
        return -1;
    }

    return 0;
}
#endif
