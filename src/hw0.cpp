#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <boost/tokenizer.hpp>
#include <string>
#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <queue>

using namespace std;
using namespace boost;

/////////////////////////////////////////////////////

bool andflag = false ;
bool orflag = false;
bool exit_flag = false;
bool finishflag = false;
bool semiflag = false;
bool success = false;

string cmd = "";

string c_or = "||";
string c_and = "&&";
string c_sc = ";";

string hash = "#";	//hash is for comments
queue<string> more_cmds;
queue <string> command;

string str_or;
string str_and;
string str_sc;

vector <string> connect_a;  //and vector
vector <string> connect_o;  //or vector
vector <string> connect_s;  //semi-colon vector
queue <string> connectors;

string get_cmd(){

    getline(cin,cmd);

    //check for hash and then creates a new cmd if found
    size_t found_hash = cmd.find(hash);
    if(found_hash <= cmd.size())
        cmd = cmd.substr(0, found_hash);

    if(cmd.size() ==0) {
        char you[64] = {0};
        string usr_name;
        if (getlogin() == NULL) {perror("getlogin() error");}
        else {usr_name = getlogin();}
        string host_name;
        if(gethostname(you, sizeof(you) -1) == -1) {perror("gethostname error");}
        else {host_name = gethostname(you, sizeof(you) -1);}

        for(unsigned int i = 0; i < 64; i++){
            if(you[i] == '.') {you[i] = '\0';}
        }
        string user_data = usr_name + "@" + host_name + "$:  ";

        cout << user_data;
        get_cmd();
    }
    for(unsigned int i = 0; i < cmd.size(); ++i) {

        if(cmd[i] == '&'){
            if(cmd[i+1] == '&') {
                connectors.push("&&");
                cmd[i] = ' ';
                cmd[i+1] = ' ';
            }
        }
        if(cmd[i] == '|'){
            if(cmd[i+1] == '|') {
                connectors.push("||");
                cmd[i] = ' ';
                cmd[i+1] = ' ';
            }
        }
        if(cmd[i] == ';'){
            connectors.push(";");
            cmd[i] = ' ';
        }
    }

//    size_t found_and = cmd.find(c_and);
//    if(found_and <= cmd.size())
//        str_and = cmd.substr(found_and, cmd.size());

//    size_t found_or = cmd.find(c_or);
//    if(found_or <= cmd.size())
//        str_or = cmd.substr(found_or, cmd.size());

//    size_t found_sc = cmd.find(c_sc);
//    if(found_sc <= cmd.size())
//        str_sc = cmd.substr(found_sc, cmd.size());



    return cmd;
}

int main(int argc, char **argv){
    while(1) {

        char you[64] = {0};
        string usr_name;
        if (getlogin() == NULL) {perror("getlogin() error");}
        else {usr_name = getlogin();}
        string host_name;
        if(gethostname(you, sizeof(you) -1) == -1) {perror("gethostname error");}
        else {host_name = gethostname(you, sizeof(you) -1);}

        for(unsigned int i = 0; i < 64; i++){
            if(you[i] == '.') {you[i] = '\0';}
        }
        string user_data = usr_name + "@" + host_name + "$:  ";

        cout << user_data;


        //command.erase(command.begin(), command.end());
        while(!command.empty()){ command.pop(); }

        get_cmd();
        //unsigned int c_int = 0;
        for(unsigned int k = 0; k <= connectors.size(); ++k) {
            if((success == true) || (k == 0)) {
                if(cmd.compare("exit") == 0) {exit(1);}
                int j = 0;

                typedef boost::tokenizer <boost::char_separator<char> > Tok;
                boost::char_separator<char> sep(" ");
                Tok tok(cmd, sep);

                for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
                    command.push(*beg);
                    //argv[j] = new char[(*beg).size()];
                    //strcpy(argv[j], (*beg).c_str());
                    //cout << *beg << endl;
                    //cout << command.front() << endl;
                }
                    //call fork()
                    int pid = fork();

                    if(pid == -1) { perror ( "fork failed"); exit(1);}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
                    //child process begins here
                    else if(pid == 0) {

                        string to_ex = command.front();
                        //command.pop();
                        if(execvp(to_ex.c_str(), argv) == -1) {
                        //if(execvp(argv[0], argv) == -1 ){
                            perror("There's an error in execvp");
                            //success = false;
                            exit(1);
                        }
                        //success = true;
                    }
                    //command.pop();
                    //parent process begins here
                    else if(pid > 0) {
                        if(-1 == wait(0)) {
                            //command.pop();
                            perror("There was an error in wait");
                        }
                        command.pop();
                        success = true;
                    }
 /////////////////////////////////////////////////////////////////////////////
                    while((connectors.size() > 0)){
                        string temp = connectors.front();
////////////////////////////////////AND CASE//////////////////////////////////////////
                        if((success ==true) && (temp.compare("&&") == 0)) {

                            string to_ex = command.front();
                            if(to_ex.compare("exit") == 0) {exit(1);}
                            int pid = fork();
                            if(pid == -1) { perror ( "fork failed"); exit(1);}

                            else if(pid == 0) {

                                //command.pop();
                                if(execvp(to_ex.c_str(), argv) == -1) {
                                //if(execvp(argv[0], argv) == -1 ){
                                    perror("There's an error in execvp");
                                    exit(1);
                                }
                                //success = true;
                            }
                            //parent process begins here
                            else if(pid > 0) {
                                //command.pop();
                                if(-1 == wait(0)) {
                                    perror("There was an error in wait");
                                }
                                command.pop();
                                success = true;
                            }
                        }
//////////////////////////////////OR CASE////////////////////////////////////////////
                        else if((success == true) && (temp.compare("||") == 0 )) {
                            command.pop();
                        }
                        else if((success == false) && (temp.compare("||") == 0)) {

                            string to_ex = command.front();
                            if(to_ex.compare("exit") == 0) {exit(1);}
                            int pid = fork();
                            if(pid == -1) { perror ( "fork failed"); exit(1);}

                            else if(pid == 0) {


                                //command.pop();
                                if(execvp(to_ex.c_str(), argv) == -1) {
                                //if(execvp(argv[0], argv) == -1 ){
                                    perror("There's an error in execvp");
                                    exit(1);
                                }
                                //success = true;
                            }
                            //parent process begins here
                            else if(pid > 0) {
                                //command.pop();
                                if(-1 == wait(0)) {
                                    perror("There was an error in wait");
                                }
                                command.pop();
                                success = true;
                            }
                        }
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////SEMI-COLON CASE//////////////////////////////
                        else if((command.size() > 0) && (temp.compare(";") == 0)) {

                            string to_ex = command.front();
                            if(to_ex.compare("exit") == 0) {exit(1);}
                            int pid = fork();
                            if(pid == -1) { perror ( "fork failed"); exit(1);}

                            else if(pid == 0) {

                                //command.pop();
                                if(execvp(to_ex.c_str(), argv) == -1) {
                                //if(execvp(argv[0], argv) == -1 ){
                                    perror("There's an error in execvp");
                                    exit(1);
                                }
                                //success = true;
                            }
                            //parent process begins here
                            else if(pid > 0) {
                                //command.pop();
                                if(-1 == wait(0)) {
                                    perror("There was an error in wait");
                                }
                                command.pop();
                                success = true;
                            }
                        }
////////////////////////////////////////////////////////////////////////////
                        connectors.pop();
                    }
/////////////////////////////////////////////////////////////////////////////
                //for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
                  //  delete [] argv[j];
                //}
            }
        }
    }
    return 0;
}

