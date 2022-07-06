#include "../src/xport.h"
#include <string>
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <dirent.h>
#include <regex>
#include <stdio.h>
using namespace xport;
using namespace std;

/**
 * 这个demo适合入门
 * 
 * 主要3个步骤
 * 1. 定义你的媒体类型。这里是FileMedia，来自本地视频文件
 * 2. 注册媒体类型。这里是FileMediaCreator
 * 3. 启动http服务
 */

//FileMedia 本地文件媒体。所有函数都可以简单地用FILE接口实现
class FileMedia : public SeekableMedia{
    FILE* mFp;
    int64_t mSize;
	int64_t curSize;
public:
    FileMedia(string path) {
        mFp = fopen(path.c_str(), "rb");
    }
public:
    bool open(){
        if (mFp){
            fseek(mFp, 0, SEEK_END);
            mSize = ftell(mFp);
            fseek(mFp, 0, SEEK_SET);
			std::cout << "FileMedia.mSize = " << mSize << "\n";
            return true;
        }

        return false;
    }

    void close(){
        if (mFp)
            fclose(mFp);
    }
	int64_t get_cur_size(){
		curSize = ftell(mFp);
		std::cout << "curSize=" << curSize <<"\n";
		return curSize;
	}
    std::string read(){
        char buf[4096]={0};
		//printf("read file\n");
		//std::cout << "data thread:" << std::this_thread::get_id() << "\n";
        auto ret = fread(buf, 1, sizeof(buf), mFp);

        if (ret <= 0)
            return "";
        return string(buf, ret);
    }

    int64_t seek(int64_t offset){
        return fseek(mFp, offset, SEEK_SET) < 0 ? -1 : offset;
    }

    int64_t size(){
        return mSize;
    }

};

//http://192.168.0.129:8000/media/simple?path=/home/xpy/ts/m3u8/5
//http://192.168.0.200:8000/media/simple?path=/home/xpy/ts/m3u8/5
//FileMediaCreator 定义对于什么请求，返回FileMedia
class FileMediaCreator : public IMediaCreator {

public:
    FileMediaCreator() { std::cout << "FileMediaCreator constructor\n"; 
        
    }
    int scoreRequest(MediaRequest& req) {
        // return 100;//作为一个简单的Demo，我们只有一种媒体类型，所以也可以直接返回100
        auto segs = req.segments();
        /*for (int i=0; i<2; i++) {
            cout << segs[i] << endl;
        }*/

        return segs[1] == "simple";
    }

    IMedia* create(MediaRequest& req) {
        auto path = req.getParamValue("path"); //获取请求的视频文件路径
        auto pos = path.rfind("/");
        auto prefix = path.substr(0, pos+1);
        size_t n;
        string name;
        if ((n = path.find_last_of("/")) != string::npos) {
            auto pos2 = path.substr(n + 1);
            int res = 0;
            if ((res = read_ts(prefix.c_str(), atoi(pos2.c_str()))) < 0)
                return new FileMedia("/home/xpy/ts/hama/hama.ts");
            printf("res=%d\n", res);
            //print_ts();
            puts("11");
            int index = atoi(pos2.c_str());
            printf("index=%d atoi(pos2.c_str()=%d\n", index, atoi(pos2.c_str()));
            if (index > ts_array.size())
                return new FileMedia("/home/xpy/ts/hama/hama.ts");
            auto suffix = ts_array[index];
            puts("22");
            if (suffix.empty()) {
                suffix = ts_array[0];
                if (suffix.empty())
                    return new FileMedia("/home/xpy/ts/hama/hama.ts");
            } else
                name = prefix + suffix;
            
        }
        printf("name:%s\n", name.c_str());
        return new FileMedia(name);
    }
    int read_ts(string dir_name, int n) {
        struct dirent* filename; // return value for readdir()
        DIR* dir; // return value for opendir()
        dir = opendir(dir_name.c_str());
        int num = -1;
        if (NULL == dir)
            return num;
        if (old_ts_array_size != 0  && n < old_ts_array_size && dir_name == old_path) {
            closedir(dir);
            puts("不用重新读取\n");
            return old_ts_array_size - n -1;
        }
        else {
            puts("重新读取\n");
            ts_array.clear();
            while ((filename = readdir(dir)) != NULL)
            {
                if (strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0) {   // get rid of "." and ".."
                    // puts("continue");
                    continue;
                }
                if (std::regex_match(filename->d_name, std::regex("(.*)(ts)"))/*&& !is_empty(s.c_str())*/) {
                    ts_array.push_back(filename->d_name);
                    num++;
                }
                if (ts_array.size() > n) {
                    break;
                }
            }
        }
        old_ts_array_size = ts_array.size();
        old_path = dir_name;
        closedir(dir);
        return old_ts_array_size - n -1;
    }
    bool is_empty(const char* name) {
        FILE* fp = fopen(name, "b");
        if (!fp) {
            printf("not exist\n");
            return false;
        }
            
        int len = ftell(fp);
        fclose(fp);
        if (len == -1)
            return true;
        else
            return false;
    }
    void print_ts() {
        printf("---------------\n");
        for (int i = 0; i < ts_array.size(); i++)
            printf("%s\n",ts_array[i].c_str());
        printf("%u\n", ts_array.size());
        printf("---------------\n");
    }
    vector<string> ts_array;
    int old_ts_array_size;
    string old_path;
};

#ifndef EXCLUDE_MAIN
int main(int argc, char* argv[]){
    while (true) {
        if (releaseOwnershipToRegsiter(new FileMediaCreator) == -1)
            continue;

        //启动http服务
        std::string ip;
        int port;
        if (argc < 2) {
            ip = "192.168.0.129";
            port = 8000;
        }
        else if (argc == 2)
        {
            ip = argv[1];
            port = 8000;
        }
        else if (argc == 3)
        {
            ip = argv[1];
            port = atoi(argv[2]);
        }
        HttpServer server;
        if (!server.start(ip.c_str(), port)) {
            printf("start http server failed\n");
            return -1;
        }
    }
    //FileMediaCreator fc;
    //fc.read_ts("/home/xpy");
    //fc.print_ts();
    return 0;
}
#endif
