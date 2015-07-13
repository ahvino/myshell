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
#include <signal.h>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>
#include <queue>
#include <sys/stat.h>

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
string path_str = "";

string hash = "#";	//hash is for comments
queue<string> more_cmds;
vector <string> command;

string str_or;
string str_and;
string str_sc;

vector <string> connect_a;  //and vector
vector <string> connect_o;  //or vector
vector <string> connect_s;  //semi-colon vector

vector <string> connectors;
queue <string> cmd_path;
vector <string> col_cmd;

//get cmd from user
string get_cmd(){

    getline(cin,cmd);

    //check for hash and then creates a new cmd if found
    size_t found_hash = cmd.find(hash);
    if(found_hash <= cmd.size())
        cmd = cmd.substr(0, found_hash);


    for(unsigned int i = 0; i < cmd.size(); ++i) {

        if(cmd[i] == '&'){
            if(cmd[i+1] == '&') {
                connectors.push_back("&&");
                cmd[i] = ' ';
                cmd[i+1] = ' ';
            }
        }
        if(cmd[i] == '|'){
            if(cmd[i+1] == '|') {
                connectors.push_back("||");
                cmd[i] = ' ';
                cmd[i+1] = ' ';
            }
        }
        if(cmd[i] == ';'){
            connectors.push_back(";");
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

/*
    typedef boost::tokenizer <boost::char_separator<char> > Tok;
        boost::char_separator<char> sep(" ");
        Tok tok(cmd, sep);
        //Tok::iterator do_cmd = tok.begin();
    for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
        command.push_back(*beg);
        argv[j] = new char[(*beg).size()];
        strcpy(argv[j], (*beg).c_str());
    }
*/

    return cmd;
}
//SIGINT(2) interrupt from keyboard
void get_sigint(int s){
    signal(SIGINT, get_sigint);
}

void quit_sig(){
    exit(0);
}

void s_handle(int sig){
    //get_cmd();
    cout << endl;
}


string get_path(string s){

    if(getenv("PATH") == NULL) { perror("error in getenv"); }
    const char* path_var = getenv("PATH");

    string temp_var = path_var;

    typedef boost::tokenizer <boost::char_separator<char> > CTok;
    boost::char_separator<char> col_sep(":");
    CTok col(temp_var, col_sep);

    for(CTok::iterator ex_cmd=col.begin(); ex_cmd!=col.end(); ++ex_cmd){
        //col_cmd.push_back(*ex_cmd);

        string temp = *ex_cmd + "/" + s;
        cmd_path.push(temp);
    }
    int cmsz = cmd_path.size();
    for(int i = 0; i < cmsz; ++i){
        struct stat buf;
        string check = cmd_path.front();
        cmd_path.pop();

        if(stat(check.c_str(), &buf) == -1){
            continue;
            perror("not correct direc");
        }
        else{
            return check;
        }
    }
return s;
}

int main(int argc, char **argv){
    while(1) {

        //SIGINT (2) interrupts from keyboard,
        //if(SIG_ERR == signal(SIGINT, s_handle)) { perror ("Error in signal");}

        char you[64] = {0};
        string usr_name;
        //if (getlogin() == NULL) {perror("getlogin() error");}
        //else {usr_name = getlogin();}
        usr_name = getlogin();
        string host_name;
        if(gethostname(you, sizeof(you) -1) == -1) {perror("gethostname error");}
        else {host_name = gethostname(you, sizeof(you) -1);}

        for(unsigned int i = 0; i < 64; i++){
            if(you[i] == '.') {you[i] = '\0';}
        }
        string user_data = usr_name + "@" + host_name + "$:  ";

        cout << user_data;
        command.erase(command.begin(), command.end());
        get_cmd();

        //for(int k = 0; k <= connectors.size(); ++k) {
            //if((success == true) || (k == 0)) {
                if(cmd.compare("exit") == 0) {exit(1);}
                if(cmd.compare("cd") == 0 ) {
                    //path_str = getenv("HOME");
                    const char* cd_path = getenv("HOME");
                    chdir(cd_path);
                    cout << endl;
                    cout << endl;
                    command.erase(command.begin(), command.end());
                    get_cmd();
                }
                int j = 0;
                typedef boost::tokenizer <boost::char_separator<char> > Tok;
                boost::char_separator<char> sep(" ");
                Tok tok(cmd, sep);

                for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
                    command.push_back(*beg);
                    argv[j] = new char[(*beg).size()];
                    strcpy(argv[j], (*beg).c_str());
                }
                    argv[j] = NULL;
                    j = 0;
                    //call fork()

                    int pid = fork();

                    if(pid == -1) { perror ( "fork failed"); exit(1);}

//child process begins here
                    if(SIG_ERR == signal(SIGINT, s_handle)) { perror ("Error in signal");}

                    else if(pid == 0) {

                        string myexec = get_path(argv[0]);
                        if(execv(myexec.c_str(), argv) == -1 ){
                            perror("There's an error in execvp");
                            exit(1);
                        }

                        success = true;
                        for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
                            delete [] argv[j];
                        }
                    }
//parent process begins here

                    else if(pid > 0) {
                        if(-1 == wait(0)) {
                            perror("There was an error in wait");
                        }
                    }
                //for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
                  //  delete [] argv[j];
                //}
            //}
        //}
    }
    return 0;
}

