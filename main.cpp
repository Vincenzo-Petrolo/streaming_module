#include <iostream>
#include "lib/Stream/stream.h"
#include <vector>
#include <map>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cstdio>


//Max number of user input args.
#define MAXARG 3
using namespace std;

void displayDevices();
void displayCommands();
void displayStreams(map<string,shared_ptr<stream>> StreamsMap);
int parseInput( vector<string> user_input);
void displaySeparator();
void displayHeader();
void displayOutIp(string ip, string port);

vector <string> splitInput(string user_input);

//used to simplify mods
enum PARSE_INPUT{STREAM_START,STREAM_STOP,SERVICE_CLOSE,INPUT_WRONG};
enum ARGS{ARG_NAME,ARG_IP,ARG_PORT,ARG_NUM};

int main(int argc, char **argv){
    //Args for user input

    string user_input;
    string buff;
    //Contains and handles streams. streams are for now inicized as their input name (eg /dev/video0).
    //This means every input can stream only once at a time.
    map<string,shared_ptr<stream>> StreamsMap;
    //flag that catches the MODE of user input, used for closing while loop
    int modeFlag=100;

    //Args handling
    if(argc!=ARG_NUM){
        cerr << "Error in args number, usage:"<<endl<<"ProgramName <Receiver Ip> <Receiver port> , eg. ProgramName 10.0.0.0 100"<<endl;
        return -1;
    }
    string receiverIp (argv[ARG_IP]);
    string receiverPort (argv[ARG_PORT]);

    //Main while loop of the program. It serves as a "graphical loop" to display stream info and commands info and status, and
    //as a way to close all the streams at once if needed.
    while (modeFlag!=SERVICE_CLOSE){
        //clear screen
        system("clear");
        //show header
        displaySeparator();
        displayHeader();
        //show ip info
        displaySeparator();
        displayOutIp(receiverIp,receiverPort);
        //show Available cameras
        displaySeparator();
        displayDevices();
        //show Available commands
        displaySeparator();
        displayCommands();
        //show streams
        displaySeparator();
        displayStreams(StreamsMap);
        //get input
        displaySeparator();
        cout<<endl<<endl;

        getline(cin,user_input);

        { //scope for user_input_args
            //put input in a vector of strings
            vector<string> user_input_args = splitInput(user_input);

            //parse input
            if (parseInput(user_input_args) == STREAM_START) {
                //check if key is not already occupied
                if(StreamsMap.count(user_input_args[1])==0) {
                    StreamsMap[user_input_args[1]] = make_shared<stream>(user_input_args[1], receiverIp, receiverPort,
                                                                         user_input_args[2]);
                }
            } else if (parseInput(user_input_args) == STREAM_STOP) {
                //check if key is not already occupied
                if(StreamsMap.count(user_input_args[1])!=0) {
                    StreamsMap.erase(user_input_args[1]);
                }
            } else if (parseInput(user_input_args) == SERVICE_CLOSE) {
                modeFlag = SERVICE_CLOSE;
                StreamsMap.clear();
            } else {
                cout << "WRONG INPUT" << endl;
            }
        }
        //clean user input
        user_input.clear();
    }

}
int main__() {

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

void displayDevices(){
    cout<<"AVAILABLE V4L DEVICES"<<endl;
    system("v4l2-ctl --list-devices");
    cout << endl;

}
/**
 * Parse all the user input args, to find the meaning of the command (e.g START ...) and correct formattation
 * @param user_input
 * @return PARSE_INPUT enum with command result
 */
int parseInput( vector<string> user_input){
    //vars used for format check
    int tmp, tmp2;
    //parse START mode
    if (user_input[0]=="START"&& !user_input[1].empty()){
        return STREAM_START;
    }

    //parse STOP mode
    if (user_input[0]=="STOP"&& !user_input[1].empty() && user_input[2].empty()){
        return STREAM_STOP;
    }

    //parse CLOSE mode
    if(user_input[0]=="CLOSE" && user_input[1].empty()){
        return SERVICE_CLOSE;
    }

    return INPUT_WRONG;

}

void displayCommands(){
    cout <<"COMMAND LIST "<<endl
    <<"to start stream:                                 START <input> <pixelFormat>  eg. START /dev/video0 1080x720" << endl
    <<"to stop stream:                                  STOP <input>                 eg. STOP /dev/video0" << endl
    <<"to close streaming service, and all the streams: CLOSE" << endl
    <<"to refresh status:                               <anything not shown above, or press ENTER>"<<endl;
}

void displayStreams(map<string,shared_ptr<stream> > StreamsMap) {
    string isactive;
    cout << "ACTIVE STREAMS: " << endl;
    if (StreamsMap.empty()) {
        cout << "currently no active streams" << endl;
    } else {
        for (auto const &x : StreamsMap) {
            if (x.second->isStillRunning() == true) {
                isactive = "ACTIVE";
            } else {
                isactive = "NOT ACTIVE (invalid input or ffmpeg crash)";
            }
            {
                std::cout << x.first  // key (video input)
                          << " : format : "
                          << x.second->pix << ", pid :  " << x.second->pid << ", " << isactive
                          << std::endl;
            }
        }
    };
}

vector <string> splitInput(string user_input){
    vector <string> tmp(3);
    int word =0;

    const char *delim = " ";

    char *token = strtok(const_cast<char*>(user_input.c_str()), delim);
    while (token != nullptr)
    {
        tmp[word] = token;
        token = strtok(nullptr, delim);
        word++;
    }
    return tmp;
}

void displaySeparator(){
    cout <<"-------------------"<<endl;
};

void displayHeader(){
    cout<<"STREAMING SERVICE v0.0.1 (CowboyCoded version)"<<endl;
}

void displayOutIp(string ip, string port){
    cout << "out IP address is "<< ip <<  ", UDP port " << port<<endl;
}