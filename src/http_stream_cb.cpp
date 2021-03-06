#include "http_stream_cb.h"
#include "log.h"

using namespace std;

#define TAG (to_string(mReader->mediaId()).c_str())

namespace xport
{

HttpStreamCB::HttpStreamCB(const shared_ptr<IReader>& reader)
    :mReader(reader), mSize(reader->to() > 0 ? reader->to() - reader->from() + 1 : -1){
}

string HttpStreamCB::read(uint64_t offset){
    if (mSize > 0 && offset >= mSize) {
        //logi("read enough");
        mReader.reset();
        return "";
    }

    auto buf = mReader->read();

    if (mSize > 0){
        auto left = mSize - offset;
        if (left < buf.size()){
            return buf.substr(0, left);
        }
    }

    return buf;
}

} // namespace xport
