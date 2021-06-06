//Streaming service

#include <iostream>
#include "lib/Stream/stream.h"

using namespace std;

int main() {
    char s = '0';
    cout << "'S' to start a stream"<< endl;

    while(s != 's'){
        cin >> s;
    }

    cout << "Streaming started ! to stop"<< endl;
    {
        stream myStream("/dev/video0");
        while (s != '!') {
            cin >> s;
        }
    }

    cout << "All destroyed"<< endl;
    return 0;
}
