//
// Created by Thoma on 06/06/2021.
//

#ifndef TEST_CAMERASERVICELAUNC_STREAM_H
#define TEST_CAMERASERVICELAUNC_STREAM_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

void my_handler (int param)
{
   int  signaled = 1;
}

class stream {
public:
    int pid;
    string arg_videoNumber;

    stream(string arg_videoNumber): arg_videoNumber(arg_videoNumber){
        cout << "stream::constructor : forking...";
        pid=fork();

        if(pid==0){ //child
            cout << "stream::constructor : child calling the exec";
            cout << "CHILD IS PID " << getpid();
            execlp("/usr/bin/ffmpeg", "ffmpeg","-f" ,"v4l2" ,"-i", arg_videoNumber.c_str(), "-profile:v", "high", "-pix_fmt", "yuvj420p", "-level:v", "4.1", "-preset", "ultrafast", "-tune" ,"zerolatency" , "-vcodec", "libx264", "-r", "10", "-b:v" ,"512k", "-s", "1280x720", "-f" ,"mpegts", "-flush_packets", "0", "udp://192.168.178.100:100?pkt_size=1316",NULL);

            throw runtime_error("Error on launching ffmpeg");

        }else{ //father ?
            cout << "stream::constructor : father says hello";
        }
    }


    ~stream(){
        if(pid !=0) {


            kill(pid, 15);
            wait(NULL);
            //wait for child to be closed
            cout << "stream::constructor : successfully killed the stream!";
        }
    }


};




#endif //TEST_CAMERASERVICELAUNC_STREAM_H
