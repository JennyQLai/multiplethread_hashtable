#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <cstring>
#include <regex>
#include <algorithm>
#include "operations.cpp"
#include "Calculator.cpp"
#include "Hash.cpp"
using namespace std;

struct args
{
    int opNum;
    int threadNum;
    int songNum;
    vector<string> song_list;
    vector<string> socket_list;
    pthread_mutex_t rw_lock;
};
struct args initialize;
Calculator Cal;
HashTable *safeHash;
int Opcnt;
void *Operation(void *threadId){
    int choice;//the choice of operations
    long id = (long)threadId;
    string song[initialize.songNum];
    string socket[initialize.songNum];
    vector<string>::iterator it1 = initialize.song_list.begin();
    vector<string>::iterator it2 = initialize.socket_list.begin();
    for(int i=0;i<initialize.songNum;i++,it1++,it2++){
        song[i] =*it1;
        socket[i] =*it2;
    }
    //int cnt=initialize.opNum;
    int i=0;
    while(Opcnt>1){
        int index;
        choice = Cal.op();
        unsigned int hashKey = Cal.process(song[i]);
        if(choice==1){//put
            index=safeHash->put(hashKey,song[i],socket[i]);
            if(index!=-1){//if put successfully
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": put song: "<<song[i]<<" at "<<socket[i]<<" in the hash table with index "<<index<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }
            else{
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": put song: "<<song[i]<<" at "<<socket[i]<<" already in the hash table with index "<<index<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }
        }
        else if(choice==2){//get
            index=safeHash->get(hashKey,song[i],socket[i]);
            if(index!=-1){//if found
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": get song: "<<song[i]<<" can be downloaded from "<<socket[i]<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }
            else{
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": get song: "<<song[i]<<" is not in the hash table"<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }
        }
        else{//delete
                index=safeHash->delet(hashKey,song[i],socket[i]);
            if(index!=-1){//if found
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": delete song: "<<song[i]<<" at "<<socket[i]<<" in the hash table"<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }
            else{
                pthread_mutex_lock(&initialize.rw_lock);
                cout<<"Thread "<<id<<": delete song: "<<song[i]<<" at "<<socket[i]<<" is not in the hash table"<<endl;
                pthread_mutex_unlock(&initialize.rw_lock);
            }

        }
        Opcnt--;
        //cout<<"\tcnt\t=\t"<<Opcnt<<endl;
        i=(i+1)%initialize.songNum;
    }
    pthread_exit(NULL);
    return 0;
}
int main()
{
    std::string re =  ",";
    regex r(re);
    //time_t t;
    //int threadNum;
    std::string mode;
    std::string tmp;
    std::string song;
    std::string socket;


    std::getline(std::cin,mode);
    while(mode[0]=='#'||mode[0]=='/'){std::getline(std::cin,mode);}

    if(mode.compare("random")==0){
        //--------------------------------------random-------------------------------------------------//
        cout<<"Have entered the \"random\" mode"<<endl;
        std::getline(std::cin,tmp);
        while(tmp[0]=='#'||tmp[0]=='/'){std::getline(std::cin,tmp);}
        if(!Cal.isNumeric(tmp)){
            cout<<"Error! the number of threads can only be number!Exiting.."<<endl;
            exit(-1);
        }
        initialize.threadNum = atoi(tmp.c_str());
        cout<<"the number of threads is:"<<initialize.threadNum<<endl;
        if(initialize.threadNum<1){
            cout<<"Error! the number of threads can not be less then 1! Exiting.."<<endl;
            exit(-1);
            /*
            std::getline(std::cin,tmp);
            while(tmp[i]=='#'||tmp[i]=='/'){std::getline(std::cin,tmp);}
            initialize.threadNum = atoi(temp.c_str());
            cout<<"the number of threads is:"<<initialize.threadNum<<endl;
            */
        }
        tmp.clear();

        std::getline(std::cin,tmp);
        while(tmp[0]=='#'||tmp[0]=='/'){std::getline(std::cin,tmp);}
        if(!Cal.isNumeric(tmp)){
            cout<<"Error! the number of operations can only be number!Exiting.."<<endl;
            exit(-1);
        }
        initialize.opNum = atoi(tmp.c_str());
        Opcnt=initialize.opNum;
        cout<<"the number of operations is:"<<initialize.opNum<<endl;
        if(initialize.opNum<1){
            cout<<"Error! the number of operations can not be less then 1!Exiting.."<<endl;
            exit(-1);
            /*
            std::getline(std::cin,tmp);
            while(tmp[i]=='#'||tmp[i]=='/'){std::getline(std::cin,tmp);}
            initialize.opNum = atoi(temp.c_str());
            cout<<"the number of operations is:"<<initialize.opNum<<endl;
            */
        }
        tmp.clear();

        cout<<"Please enter the song name and socket, if want to finish entering, enter \"@\""<<endl;
        while(std::getline(std::cin,tmp)){
                while(tmp[0]=='#'||tmp[0]=='/'){std::getline(std::cin,tmp);}
                if(tmp[0]=='@'){break;}
                for(int a=0;a<(int)tmp.length();a++){
                    if(tmp[a]==','){
                        song = tmp.substr(0,a);
                        socket = tmp.substr(a+1);
                        if((int)socket.find("http")==-1){
                            cout<<"The socket "<<socket<<"of song"<<song<<" is invalid!"<<endl;
                        }
                        else{
                            initialize.song_list.push_back(song);
                            initialize.socket_list.push_back(socket);
                        }
                        break;
                    }
                }
        }
        initialize.songNum = initialize.song_list.size();
        if(initialize.songNum<1){
            cout<<"Error! There is no song! Exiting"<<endl;
            exit(-1);
        }
        //--------------------------------------PTHREAD---------------------------------------------//
        pthread_t threads[initialize.threadNum];
        int HashTableSize = Cal.biggestPrime(initialize.songNum);
        safeHash = new HashTable(HashTableSize);
        if(pthread_mutex_init(&initialize.rw_lock,NULL)!=0){
            cout<<"error in initialization of mutex...exiting"<<endl;
            exit(-1);
        }
        else{
            cout<<"Successfully initialized..."<<endl;
        }
        for(int a=0;a<initialize.threadNum;a++){
            pthread_mutex_lock(&initialize.rw_lock);
            cout<<"Creating thread No."<<a<<endl;
            pthread_create(&threads[a],NULL,Operation,(void *)a);
            pthread_mutex_unlock(&initialize.rw_lock);
        }
        for(int a=0;a<initialize.threadNum;a++){
            (void)pthread_join(threads[a],NULL);
        }
        pthread_mutex_destroy(&initialize.rw_lock);
    }
    else if(mode.compare("manual")==0){
        //--------------------------------------------manual-----------------------------------------------//
        safeHash = new HashTable(17);//default size: 17
        cout<<"have entered the \"manual\" mode,if you want to exit, please press ctrl+Z or input \"@\""<<endl;
        string choice;
        while(choice[0]!='@'&&tmp[0]!='@'){
            cout<<"please enter your choice of operation:(put/get/delete)"<<endl;
            cin>>choice;
            if(choice[0]=='@'){cout<<"Exiting..."<<endl;exit(-1);}
            std::getline(std::cin,tmp);
            //cout<<"choice:"<<choice<<endl;
            cout<<"please enter your song name and socket:(e.g.Listen to the music, http://foo.com:54321)"<<endl;
            std::getline(std::cin,tmp);
            //cout<<"CIN done!  tmp: "<<tmp<<endl;
            if(tmp[0]=='@'){cout<<"Exiting..."<<endl;exit(-1);}
            for(int a=0;a<(int)tmp.length();a++){
                        if(tmp[a]==','){
                            song = tmp.substr(0,a);
                            //cout<<song<<endl;
                            socket = tmp.substr(a+1);
                            //cout<<socket<<endl;
                            break;
                        }
                    }
            int index;
            unsigned int hashKey = Cal.process(song);//fine
            if(choice.compare("put")==0){
                    //cout<<"enter put function"<<endl;
                    /*
                    initialize.song_list.push_back(song);
                    initialize.socket_list.push_back(socket);
                    */
                    index=safeHash->put(hashKey,song,socket);
                    //cout<<"index=="<<index<<endl;
                    if(index!=-1){//put successfully
                        cout<<"Successfully put song: "<<song<<" at "<<socket<<" in the hash table with index "<<index<<endl;
                    }
                    else{
                        cout<<"Error: put song: "<<song<<" at "<<socket<<" already in the hash table"<<endl;
                    }
            }
            if(choice.compare("get")==0){
                index=safeHash->get(hashKey,song,socket);
                if(index!=-1){//put successfully
                    cout<<"Success! get song: "<<song<<" can be downloaded from "<<socket<<endl;
                }
                else{
                    cout<<"Error! get song: "<<song<<" is not in the hash table"<<endl;
                }
            }
            if(choice.compare("delete")==0){
                index=safeHash->delet(hashKey,song,socket);
                if(index!=-1){//if found
                    cout<<"Success! delete song: "<<song<<" at "<<socket<<" in the hash table"<<endl;
                }
                else{
                    cout<<"Error! delete song: "<<song<<" at "<<socket<<" is not in the hash table"<<endl;
                }
            }
        }
        cout<<"Exiting.."<<endl;
    }
    else{
        cout<<"Error, please enter \"random\" or \"manual\", exiting.."<<endl;
        exit(-1);
    }
    return 0;
}
