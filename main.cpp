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
enum PARSE_INPUT{STREAM_START,STREAM_STOP,SERVICE_EXIT,INPUT_WRONG};
//Args for program start
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
    while (modeFlag != SERVICE_EXIT){
        //--- Graphics
        //clear screen
        system("clear");
        //show header
        displaySeparator();
        displayHeader();
        //show Available commands
        displaySeparator();
        displayCommands();
        //show ip info
        displaySeparator();
        displayOutIp(receiverIp,receiverPort);
        //show Available cameras
        displaySeparator();
        displayDevices();

        //show streams
        displaySeparator();
        displayStreams(StreamsMap);
        //get input
        displaySeparator();
        cout<<endl<<endl;

        //--- User Input. todo: this part can be overrided with MOQUETTE commands
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
            } else if (parseInput(user_input_args) == SERVICE_EXIT) {
                modeFlag = SERVICE_EXIT;
                StreamsMap.clear();
            } else {
                cout << "WRONG INPUT" << endl;
            }
        }
        //clean user input
        user_input.clear();
    }

}

//----STRING HANDLING----//

/**
 * Parse all the user input args, to find the meaning of the command (e.g START ...) and correct formattation
 * @param user_input
 * @return PARSE_INPUT enum with command result
 */
int parseInput( vector<string> user_input){
    //vars used for format check
    int tmp, tmp2;
    //parse START mode
    if ((user_input[0]=="START" || user_input[0]=="start")&& !user_input[1].empty()){
        return STREAM_START;
    }

    //parse STOP mode
    if ((user_input[0]=="STOP" ||user_input[0]=="stop")&& !user_input[1].empty() && user_input[2].empty()){
        return STREAM_STOP;
    }

    //parse CLOSE mode
    if((user_input[0]=="EXIT" || user_input[0]=="exit") && user_input[1].empty()){
        return SERVICE_EXIT;
    }

    return INPUT_WRONG;

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

//----GRAPHICS----//

void displayCommands(){
    cout <<"\033[1;33mCOMMAND LIST \033[0m"<<endl
    <<"\033[1;33mto start stream:\033[0m                                 START <input> <pixelFormat>  eg. START /dev/video0 1080x720" << endl
    <<"\033[1;33mto stop stream:\033[0m                                  STOP <input>                 eg. STOP /dev/video0" << endl
    <<"\033[1;33mto close streaming service, and all the streams:\033[0m EXIT" << endl
    <<"\033[1;33mto refresh status:\033[0m                               <anything not shown above, or press ENTER>"<<endl;
}

void displayStreams(map<string,shared_ptr<stream> > StreamsMap) {
    string isactive;
    cout << "\033[1;33mACTIVE STREAMS: \033[0m" << endl;
    if (StreamsMap.empty()) {
        cout << "\033[1;31mcurrently no active streams\033[0m" << endl;
    } else {
        for (auto const &x : StreamsMap) {
            if (x.second->isStillRunning() == true) {
                isactive = "\033[1;32mACTIVE\033[0m";
            } else {
                isactive = "\033[1;31mNOT ACTIVE \033[0m (invalid input or ffmpeg crash)";
            }
            {
                std::cout << x.first  // key (video input)
                          << " \033[1;33mformat \033[0m"
                          << x.second->pix << "\033[1;33m pid \033[0m" << x.second->pid << "\033[1;33m status \033[0m" << isactive
                          << std::endl;
            }
        }
    };
}

void displaySeparator(){
    cout <<"-------------------"<<endl;
};

void displayHeader(){
    cout<<"\033[1;33mSTREAMING SERVICE v0.0.1\033[0m"<<endl;
}

void displayOutIp(string ip, string port){
    cout << "\033[1;33mOUT IP ADDRESS\033[0m"<<endl<< ip <<  ":" << port<<endl;
}

void displayDevices(){
    cout<<"\033[1;33mAVAILABLE V4L DEVICES\033[0m"<<endl;
    system("v4l2-ctl --list-devices");
    cout << endl;

}