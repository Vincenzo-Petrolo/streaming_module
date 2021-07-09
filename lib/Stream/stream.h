//
// Created by Thoma on 06/06/2021.
//

#ifndef TEST_CAMERASERVICELAUNC_STREAM_H
#define TEST_CAMERASERVICELAUNC_STREAM_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    string videoId;
    string pix;
    bool closed;
    int id;

    /**
     * constructor of stream class
     * @param VideoPort name of the /dev/video* port to be streamed. eg "/dev/video0"
     * @param receiver_ip ip of udp receiver (will be Webapp). eg. "192.168.0.0"
     * @param receiver_ip_port port used by udp video receiver, eg "100".
     * @param pix video pixel size, eg "100x100"
     *
     */
    stream(string VideoPort, string receiver_ip, string receiver_ip_port, string pix): videoId(VideoPort), pix(pix){
        cout << "stream::constructor : forking...";
        pid=fork();

        if(pid==0){ //child
            cout << "stream::constructor : child calling the exec";
            cout << "CHILD IS PID " << getpid();

            //open /dev/null for writing, TO REDIRECT FFMPEG OUTPUT TO nuLL */
            int fd = open("/dev/null", O_WRONLY);

            dup2(fd, 1);    /* make stdout a copy of fd (> /dev/null) */
            dup2(fd, 2);    /* ...and same with stderr */
            dup2(fd, 0);
            close(fd);      /* close fd */

            string full_udp("udp://"+receiver_ip+":"+receiver_ip_port+":"+"?pkt_size=1316");
            execlp("/usr/bin/ffmpeg", "ffmpeg","-f" ,"v4l2" ,"-i", VideoPort.c_str(), "-profile:v", "high", "-pix_fmt", "yuvj420p", "-level:v", "4.1", "-preset", "ultrafast", "-tune" ,"zerolatency" , "-vcodec", "libx264", "-r", "10", "-b:v" ,"512k", "-s", pix.c_str(), "-f" ,"mpegts", "-flush_packets", "0", full_udp.c_str(),NULL);

            throw runtime_error("Error on launching ffmpeg");

        }else{ //father ?
            cout << "stream::constructor : fork successfully done."<<endl;
        }
    }

    ~stream(){
        //check if son is effectively a running child:
        // isstilrunning()=false means that the process has already been wait()ed, and closed by parent."
        if(pid !=0 && isStillRunning()) {
            kill(pid, 15);
            wait(NULL);
            //wait for child to be closed
            cout << "stream::constructor : successfully killed the stream!";
        }
    }

    bool isStillRunning(){
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == 0) {
            return true;
        } else if (result == -1) {
            //error on child
            return false;
        } else {
            //child has exited
            return false;
        }
    }

    /**
     * @brief Move contructor
     */
    stream(stream &&) = delete;

    /**
     * @brief Copy operator
     * @return Shared object
     */
    stream &operator=(const stream &) = delete;

};




#endif //TEST_CAMERASERVICELAUNC_STREAM_H
