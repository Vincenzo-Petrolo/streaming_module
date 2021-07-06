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
/**
 * This class launches and destroys a specific stream. It makes a fork() in the constructor to launch
 * ffmpeg, and a kill() in the deconstructor to close the stream (kills ffmpeg as well)
 */
class stream {
public:
    int pid;
    string arg_videoNumber;
    string pix;
    /**
     * constructor of stream class
     * @param VideoPort name of the /dev/video* port to be streamed. eg "/dev/video0"
     * @param receiver_ip ip of udp receiver (will be Webapp). eg. "192.168.0.0"
     * @param receiver_ip_port port used by udp video receiver, eg "100".
     * @param pix video pixel size, eg "100x100"
     *
     */
    stream(string VideoPort, string receiver_ip, string receiver_ip_port, string pix): arg_videoNumber(VideoPort),pix(pix){
        cout << "stream::constructor : forking...";
        pid=fork();

        if(pid==0){ //child
            cout << "stream::constructor : child calling the exec";
            cout << "CHILD IS PID " << getpid();
            string full_udp("udp://"+receiver_ip+":"+receiver_ip_port+":"+"?pkt_size=1316");
            execlp("/usr/bin/ffmpeg", "ffmpeg","-f" ,"v4l2" ,"-i", VideoPort.c_str(), "-profile:v", "high", "-pix_fmt", "yuvj420p", "-level:v", "4.1", "-preset", "ultrafast", "-tune" ,"zerolatency" , "-vcodec", "libx264", "-r", "10", "-b:v" ,"512k", "-s", pix.c_str(), "-f" ,"mpegts", "-flush_packets", "0", full_udp.c_str(),NULL);

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
