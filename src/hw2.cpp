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
#include <fcntl.h>
#include <fstream>
#include <istream>

using namespace std;
using namespace boost;

bool andflag = false;
bool orflag = false;
bool exit_flag = false;
bool finishflag = false;
bool outputflag = false;

string exitcmd = "exit";
string cmd = "";
string r_out = ">";
string r_in = "<";
string semcol = ";";

string hash = "#";	//hash is for comments
queue<string> more_cmds;
vector <string> command;
vector <const char*> cmd_char;

        //check for number of ands
	    //size_t found_and = 0;
	    //for(int i = 0; i< cmd.size(); i ++){
		  //  if(cmd.at(i) == '&' && cmd.at(i+1) == '&' ){
			//    andflag = true;
		    //}
	    //}
        //check for number of ors
	    //size_t found_or = 0;
	    //for(int i = 0; i< cmd.size(); i++){
		  //  if(cmd.at(i) == '|' && cmd.at(i+1) == '|')
			//    orflag = true;
	    //}
        //
///////////////////////////////////////
//THIS IS FOR OUTPUT REDIR WITH////////
// APPEND FUNCTIONALITY INCLUDED///////
///////////////////////////////////////
void std_out(const string s){
    vector <string> arg_cmd;
    string out_str;
    string out_file;
    string out_cmd;
    int j = 0;
    char* argv[2];
    bool append_flag = false;

    size_t redir_out = cmd.find(r_out);

    if(redir_out <= cmd.size()){
        if(cmd[redir_out +1] == '>') {
            append_flag = true;
            cmd[redir_out +1] = ' ';
       //     cout << "time 2 append" << endl; exit(1);
         }
    //    cout <<"found > at:  " << redir_out << endl;

        out_cmd = cmd.substr(0, redir_out);
        out_file = cmd.substr(redir_out+1, cmd.size());

      //  cout <<"Out cmd is:     " << out_cmd << endl;
      //  cout <<"File is:        " << out_file << endl;
    }
    if(!append_flag){
        typedef boost::tokenizer <boost::char_separator<char> > rightTok;
        boost::char_separator<char> rsep(" ");
        rightTok rtok(out_file, rsep);
        rightTok::iterator riter = rtok.begin();


        const char* new_file = (*riter).c_str();
        int intake = open(new_file, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if(intake == -1){ perror("open failed"); }

        int save_out = dup(1);
        if(save_out == -1) { perror("fail in dup"); }

        if(dup2(intake, 1) == -1) { perror("error in dup2"); exit(1); }

        close(intake);

        typedef boost::tokenizer <boost::char_separator<char> > leftTok;
        boost::char_separator<char> lsep(" ");
        leftTok ltok(out_cmd, lsep);

       // cout << "before  execvp " << endl;
        for(leftTok::iterator beg=ltok.begin(); beg!=ltok.end(); ++j, ++beg){
                arg_cmd.push_back(*beg);

                argv[j] = new char[(*beg).size()];
                strcpy(argv[j], (*beg).c_str());

            //    out_str = arg_cmd.at(0);
        }
        argv[j] = NULL;
        j = 0;
        //char* new_cmd = new char[cmd.length() + 1];
        //cout << " calling execvp for out redir: " << endl;
        if(execvp(argv[0], argv) == -1) {
            perror("There's an error redir execvp");
            exit(1);
        }
        for(leftTok::iterator rid_iter=ltok.begin(); rid_iter!=ltok.end(); ++j, ++rid_iter){
            delete [] argv[j];
        }
        if(dup2(save_out, 1) == -1) { perror("error in dup2"); exit(1); }
        close(save_out);
    }
    if(append_flag){
        typedef boost::tokenizer <boost::char_separator<char> > rightTok;
        boost::char_separator<char> rsep(" ");
        rightTok rtok(out_file, rsep);
        rightTok::iterator riter = rtok.begin();

        const char* new_file = (*riter).c_str();
        int intake = open(new_file, O_CREAT | O_APPEND | O_WRONLY, 0666);
        if(intake == -1){ perror("open failed"); }

        int save_out = dup(1);
        if(save_out == -1) { perror("fail in dup"); }

        if(dup2(intake, 1) == -1) { perror("error in dup2"); exit(1); }
        close(intake);

        typedef boost::tokenizer <boost::char_separator<char> > leftTok;
        boost::char_separator<char> lsep(" ");
        leftTok ltok(out_cmd, lsep);

        //cout << "before  execvp " << endl;
        for(leftTok::iterator beg=ltok.begin(); beg!=ltok.end(); ++j, ++beg){
                arg_cmd.push_back(*beg);
                argv[j] = new char[(*beg).size()];
                strcpy(argv[j], (*beg).c_str());
            //    out_str = arg_cmd.at(0);
        }
        argv[j] = NULL;
        j = 0;
        //char* new_cmd = new char[cmd.length() + 1];
        //cout << " calling execvp for out redir: " << endl;

        if(execvp(argv[0], argv) == -1) {
            perror("There's an error redir execvp");
            exit(1);
        }

        for(leftTok::iterator rid_iter=ltok.begin(); rid_iter!=ltok.end(); ++j, ++rid_iter){ delete [] argv[j];
        }
        if(dup2(save_out, 1) == -1) { perror("error in dup2"); exit(1); }
        close(save_out);
        append_flag = false;
    }
}
////////////////////////////////////////
//THIS WILL BE FOR INPUT REDIRECTION////
////////////////////////////////////////
//
//
void std_in(const string s){
    vector <string> arg_in;
    string in_str;
    string in_file;
    string in_cmd;
    int j = 0;
    char* argv[2];
    size_t redir_in = cmd.find(r_in);
        if(redir_in <= cmd.size()){
          //  cout <<"found < at:  " << redir_in << endl;
            in_cmd = cmd.substr(0, redir_in);
            in_file= cmd.substr(redir_in +1, cmd.size());
        }
        typedef boost::tokenizer <boost::char_separator<char> > rightTok;
        boost::char_separator<char> rsep(" ");
        rightTok rtok(in_file, rsep);
        rightTok::iterator riter = rtok.begin();
        const char* new_file = (*riter).c_str();
        int intake = open(new_file, O_RDONLY);
        //cout <<"new file is: " << new_file << endl;
        if(intake == -1){ perror("open failed"); }
        int save_out = dup(0);
        if(save_out == -1) { perror("fail in dup"); }
        if(dup2(intake, 0) == -1) { perror("error in dup2"); exit(1); }

        close(intake);
        typedef boost::tokenizer <boost::char_separator<char> > leftTok;
        boost::char_separator<char> lsep(" ");
        leftTok ltok(in_cmd, lsep);
        //cout << "before  execvp " << endl;
        //try inputting *in_cmd instead of *beg
        for(leftTok::iterator beg=ltok.begin(); beg!=ltok.end(); ++j, ++beg){
                arg_in.push_back(*beg);
                argv[j] = new char[(*beg).size()];
                strcpy(argv[j], (*beg).c_str());
        }
        argv[j] = NULL;
        j = 0;
        if(execvp(argv[0], argv) == -1) {
            perror("There's an error redir execvp");
            exit(1);
        }
        for(leftTok::iterator rid_iter=ltok.begin(); rid_iter!=ltok.end(); ++j, ++rid_iter){
            delete [] argv[j];
        }
        if(dup2(save_out, 1) == -1) { perror("error in dup2"); exit(1); }
        close(save_out);
}

//checks for hash
string hash_check(string& s){
    size_t found_hash = cmd.find(hash);
    if(found_hash <= cmd.size()){
        cmd = cmd.substr(0, found_hash);
    }
    return s;
}

string semcol_check(string s){
    size_t found_semcol = cmd.find(semcol);
    if(found_semcol <= cmd.size()){
        //we need to execute up to semi col
    }
    return cmd;
}
void to_pipe(const string s){
    int fd[2];
    const int PIPE_READ = 0;
    const int PIPE_WRITE = 1;
    char* argv[2];
    char* argv2[2];
    int savestdin;
    if(pipe(fd) == -1) { perror ("error in pipe"); exit(1);}

    int pid = fork();

    if(pid ==-1) { perror ("error in fork"); exit(1);}

    else if(pid == 0) {
        if(close(fd[1]) == -1) { perror ("error in close"); exit(1); }

        string from;
        string to;

        if(-1 == dup2(fd[PIPE_WRITE], 1)) {perror("error in dup2"); exit(1); }

        if(-1 == close(fd[PIPE_READ])) { perror("error in close"); exit(1); }

        if(-1 == execvp(argv[0], argv)) { perror("error in execvp"); exit(1);}

    }

    else if( pid > 0){

        if(-1 == (savestdin = dup(0))) { perror ("error in dup"); }

        if(-1 == wait(0)) { perror ("error in wait"); }

        int pid2 = fork();
        if(pid2 == -1) { perror("error in fork"); exit(1); }

        else if(pid2 == 0){

            if(-1 == dup2(fd[PIPE_READ], 0)) { perror ("error in dup2"); }

            if(-1 == close(fd[PIPE_WRITE])) { perror ("error in close"); }

            if(-1 == execvp(argv2[0], argv2)) { perror ("error in execvp"); exit(1); }
        }
        else if( pid2 > 0) {
            if(-1 == wait(0)) { perror("error in wait"); }
        }
    }
    if(-1 == dup2(savestdin, 0)) { perror("error in dup2"); }
}

string get_cmd(){
    string out_char = ">";
    string in_char = "<";
    string pipe_char = "|";

    cout << "$: ";
    getline(cin,cmd);
    hash_check(cmd);

    if(cmd.compare("exit") == 0) {exit(1);}

    size_t found_pipe = cmd.find(pipe_char);
    if(found_pipe <= cmd.size()) {to_pipe(cmd); }

    size_t found_out = cmd.find(out_char);
    if(found_out <= cmd.size()) { std_out(cmd); }

    size_t found_in = cmd.find(in_char);
    if(found_in <= cmd.size()) { std_in(cmd); }
    return cmd;
}
//////////////////////////////////////////////
int main(int argc, char **argv){
        while(1) {
	            get_cmd();
                //now we need to convert to char* for execvp
                if(cmd.compare("exit") == 0) {exit(1);}

                int pid = fork();

                if(pid== -1) { perror ("fork failed");
                    exit(1);
                }
////////////////CHILD PROCESS//////////////////////////
                if(pid == 0){
                    char* argv[1024];
                    int j = 0;
                    string str;
                    typedef boost::tokenizer <boost::char_separator<char> > Tok;
	                boost::char_separator<char> sep(" ");
	                Tok tok(cmd, sep);

                    for(Tok::iterator beg=tok.begin(); beg!=tok.end(); ++j, ++beg){
	                    command.push_back(*beg);

                        argv[j] = new char[(*beg).size()];
                        strcpy(argv[j], (*beg).c_str());

                        str = command.at(0);
                    }
                    argv[j] = 0;
                    j = 0;
                    //char* new_cmd = new char[cmd.length() + 1];
                    if(execvp(argv[0], argv) == -1) {
                        perror("There's an error in execvp");
                        exit(1);
                    }
                    for(Tok::iterator rid_iter=tok.begin(); rid_iter!=tok.end(); ++j, ++rid_iter){
                        delete [] argv[j];
                    }
                }
////////////////PARENT PROCESS//////////////////////////
                if(pid > 0) {
                    if(-1 == wait(0)) {
                        perror("There was an error in wait");
                    }
                }
            }
    return 0;
}

