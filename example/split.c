#include <string>
#include <stdio.h>

using namespace std;
int main() {
    string s = "http://123/456/789";
    unsigned ss = s.find_last_of("/");
    printf("%s\n", s.substr(ss).c_str());
    printf("%s\n", s.substr(0, s.rfind("/")).c_str());
    
}
