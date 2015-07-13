#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <vector>
#include <string.h>
#include <string>
#include <boost/tokenizer.hpp>
#include <stdio.h>
#include <time.h>
#include <iomanip>
#include <pwd.h>
#include <grp.h>

using namespace std;

bool dash_a = false;
bool dash_l = false;
bool dash_R = false;
bool directory = false;

struct stat s;      //used for file || direct...
vector<char*> files;
vector<char*> directories;
vector<string> outputs;
/*
 *This should program should use opendir/readdir/closedir
 *It will also use error checking on these functions.
*/
void get_flags(int c, char** s){
int opt;
   while((opt = getopt(c, s , "alR")) != -1){
        if(opt == '?') {perror("error in getopt");
            exit(0);
        }
        switch (opt){ //transitions to opt states
            case 'a':
                dash_a = true;
            break;
            case 'l':
                dash_l = true;
            break;
            case 'R':
                dash_R = true;
            break;
            default:
                exit(0);
            break;
        }
    }
}
//this is for the -a flag
void a_flag(){
    const char* dirName = ".";
    DIR* dirp;
    dirent* direntp;

    if((dirp = opendir(dirName)) == NULL){ perror ("error in open directory"); return; }

    else {
        while((direntp = readdir(dirp))){
            string file_name = direntp->d_name;
            //cout << file_name <<"   ";
            outputs.push_back(file_name);
            //prints++;
            //if(prints % 3 == 0){ cout << endl;}
        }
        sort(outputs.begin(), outputs.end());
        for(int i = 0; i < outputs.size(); i++){
            cout << outputs[i] << "     ";
            cout << setw(5);
        }
    }
    if(closedir(dirp) == -1){ perror("error in close" ); }
    /*
    sort(outputs.begin(), outputs.end());
    for(int i = 0; i < outputs.size(); i++){
        cout << outputs[i];
        cout << setw(5);
    }
    */
    cout << endl;
}

//this is for the -l flag
void l_flag(){
    int l_total = 0;
    struct stat stat_buf;
    const char* dirName = ".";
    DIR* dirp;
    dirent* direntp;
    if((dirp = opendir(dirName)) == NULL) { perror("open direc"); return;}
    else {
        cout << stat_buf.st_ino << endl;
        while((direntp = readdir(dirp))){
            char path[1024];

            string l_file = direntp->d_name;
            if(stat(l_file.c_str(), &stat_buf) == -1) {perror("stat");}

            if(direntp->d_name[0] != '.'){
                struct passwd *owner = getpwuid(stat_buf.st_uid);
                struct group *grp = getgrgid(stat_buf.st_gid);
                if(owner == NULL) {perror ("getpwuid"); exit(1);}
                if(grp == NULL) {perror ("getgrgid"); exit(1);}

                cout << (S_ISDIR(stat_buf.st_mode) ? "d":"-");
                cout << ((stat_buf.st_mode & S_IRUSR) ? "r":"-");
                cout << ((stat_buf.st_mode & S_IWUSR) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXUSR) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IRGRP) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IWGRP) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXGRP) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IROTH) ? "r":"-");
                cout << ((stat_buf.st_mode & S_IWOTH) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXOTH) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IXOTH) ? "x":"-");
                cout << setw(4);
                cout << " " << owner->pw_name << "  ";
                cout << setw(4);
                cout << " " << grp->gr_name << "  ";
                cout << setw(4);
                //gets the file size
                cout << " " << stat_buf.st_size << "  ";
                cout << setw(4);
                //get the time
                //time_t time = stat_buf.st_mtime;

                //struct time *tinfo;
                //tinfo = localtime (&stat_buf.st_mtime);
               cout << stat_buf.st_ctime <<":" << stat_buf.st_atime <<" "<< stat_buf.st_mtime;
               cout << setw(4);
               string file_name = direntp->d_name;
               cout << "    " << file_name << endl;


            }
        }
        if(closedir(dirp) == -1){ perror("error in close" ); }
    }
}

//this is for the R flag
void R_flag(const char* file){

    vector <char*> directory;
    struct stat stat_buf;
    DIR* dirp;

    dirent* direntp;

    if((dirp = opendir(file)) == NULL) { perror("open dir"); return;}
    cout << file << endl;

    while((direntp = readdir(dirp))) {

        string fslash = "/";
        char path[1024];
        strcpy(path, file);

        strcat(path, fslash.c_str());
        strcat(path, direntp->d_name);

        //stat call
        if((stat(path, &stat_buf)) == -1){ perror("error in stat"); }

        if(direntp->d_name[0] == '.') { continue;}
        string file_name = direntp->d_name;

        cout << file_name << setw(5);
        if(S_ISDIR(stat_buf.st_mode)) { directory.push_back(direntp->d_name);}

    }
    cout << endl;
    int dir_sz = directory.size();

    //this recursively calls R_flag()
    for(int i = 0; i < dir_sz; ++i){
        char path[1024];
        strcpy(path, file);

        string fslash = "/";
        strcat(path, fslash.c_str());
        strcat(path, directory[i]);
        R_flag(path);
    }
    if((closedir(dirp) == -1)) { perror("error in close");}
    return;
}

//this is for the al flag
void al_flag(){
    struct stat stat_buf;
    const char* dirName = ".";
    DIR* dirp;
    dirent* direntp;
    if((dirp = opendir(dirName)) == NULL) { perror("open dir"); return;}
    else {
        cout << stat_buf.st_ino << endl;
        while((direntp = readdir(dirp))){
            char path[1024];

            string l_file = direntp->d_name;
            if(stat(l_file.c_str(), &stat_buf) == -1) {perror("stat");}

            //if(direntp->d_name[0] != '.'){
                struct passwd *owner = getpwuid(stat_buf.st_uid);
                struct group *grp = getgrgid(stat_buf.st_gid);
                if(owner == NULL) {perror ("getpwuid"); exit(1);}
                if(grp == NULL) {perror ("getgrgid"); exit(1);}

                cout << (S_ISDIR(stat_buf.st_mode) ? "d":"-");
                cout << ((stat_buf.st_mode & S_IRUSR) ? "r":"-");
                cout << ((stat_buf.st_mode & S_IWUSR) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXUSR) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IRGRP) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IWGRP) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXGRP) ? "x":"-");
                cout << ((stat_buf.st_mode & S_IROTH) ? "r":"-");
                cout << ((stat_buf.st_mode & S_IWOTH) ? "w":"-");
                cout << ((stat_buf.st_mode & S_IXOTH) ? "x":"-");
                cout << setw(4);
                cout << " " << owner->pw_name << "  ";
                cout << setw(4);
                cout << " " << grp->gr_name << "  ";
                cout << setw(4);
                //gets the file size
                cout << " " << stat_buf.st_size << "  ";
                cout << setw(4);
                //get the time
                //time_t time = stat_buf.st_mtime;

                //struct time *tinfo;
                //tinfo = localtime (&stat_buf.st_mtime);
               cout << stat_buf.st_ctime <<":" << stat_buf.st_atime <<" "<< stat_buf.st_mtime;
               cout << setw(4);
               cout << "    " << direntp->d_name << endl;;
           // }
        }
        if(closedir(dirp) == -1){ perror("error in close" ); }
    }

}

void aR_flag(){

}

void lR_flag(){

}
void alR_flag(){

}

int main (int argc, char* argv[]){
    for(int i = 1; i != argc; i++){//for loop skips argv[0] since it's the executable

    }

    vector <char*> all_files;
    for(int i = 0; i < argc; i++) {
        all_files.push_back(argv[i]);
    }

    get_flags(argc, argv);


    if     (dash_a == true && dash_l == false && dash_R == false)   {a_flag();}
    else if(dash_a == false && dash_l == true && dash_R == false)   {l_flag();}
    else if(dash_a == false && dash_l == false && dash_R == true)   {
        const char* dirName = ".";
        R_flag(dirName);}
    else if(dash_a == true && dash_l == true && dash_R == false)    {al_flag();}
    else if(dash_a == true && dash_l == false && dash_R == true)    {aR_flag();}
    else if(dash_a == false && dash_l == true && dash_R == true)    {lR_flag();}
    else if(dash_a == true && dash_l == true && dash_R == true)     {alR_flag();}

    return 0;
}
