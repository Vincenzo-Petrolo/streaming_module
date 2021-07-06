//Streaming service

#include <iostream>
#include "lib/Stream/stream.h"

using namespace std;
/**
 * O HI! This is the stream service thing I made! Its easy to use. Just launch one per stream with correct arguments, or use this as a
 * template for your streaming service. Everything happens inside the "stream" class anyway.
 * @return
 */


int main() {

    char s = '0';

    // Wait for input (shall be replaced with a MOQUETTE signal waiting)
    cout << "'S' to start a stream"<< endl;
    while(s != 's'){
        cin >> s;
    }

    cout << "Streaming starting. ! to stop"<< endl;
    {
        //launch stream (performs fork internally, child process launches ffmpeg, father continues, ideally can launch more than one stream)
        stream myStream("/dev/video0", "10.0.0.107:100", "100", "1080x720");

        // Wait for input to stop stream (shall be replaced with a MOQUETTE signal)
        while (s != '!') {
            cin >> s;
        }
        //class stream closes ffmpeg when goes out of scope
    }

    cout << "All done bye"<< endl;
    return 0;
}
