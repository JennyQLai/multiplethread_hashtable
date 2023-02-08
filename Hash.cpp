#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>

using namespace std;
class HashNode{
    unsigned int hashKey;
    string songName;
    string socket;
    HashNode *next;
public:
    HashNode(unsigned int Key,string song,string soc){
        hashKey = Key;
        songName = song;
        socket = soc;
        next = NULL;
    }
    string getValue1(){
        return songName;
    }
    string getValue2(){
        return socket;
    }
    unsigned int getKey(){
        return hashKey;
    }
    HashNode* getNext(){
        return next;
    }
    void setValue1(string song){
        songName = song;
    }
    void setValue2(string soc){
        socket = soc;
    }
    void setNext(HashNode* following){
        next = following;
    }
};
class HashTable{
    HashNode **table;
    int HashTableSize;
    pthread_mutex_t *hashlock;
public:
    HashTable(int ts){
        pthread_mutex_t newmutex[ts];
        hashlock = new pthread_mutex_t[ts];
        for(int i=0;i<ts;i++){
            pthread_mutex_init(&newmutex[i],NULL);
            hashlock[i]=newmutex[i];
        }
        HashTableSize = ts;
        table = new HashNode*[ts];
        for(int i = 0;i<ts;i++){
            table[i]=NULL;
        }
    }
    int get(unsigned int hashKey,string songName,string socket){
        int index;
        index=abs((int)hashKey%HashTableSize);
        //cout<<"get_index="<<index<<endl;
        pthread_mutex_lock(&hashlock[index]);
        HashNode *entry = table[index];
        while(entry!=NULL){
            if(songName.compare(entry->getValue1())==0&&socket.compare(entry->getValue2())==0){
                songName = entry->getValue1();
                socket = entry->getValue2();
                pthread_mutex_unlock(&hashlock[index]);
                return index;
            }
            entry = entry->getNext();
        }
        pthread_mutex_unlock(&hashlock[index]);
        return -1;
    }
    int put(unsigned int hashKey,string songName,string socket){
        //cout<<"in put function"<<endl;
        //cout<<"HashtableSize"<<HashTableSize<<endl;
        int index = abs((int)hashKey%HashTableSize);
        //cout<<"put_index="<<index<<endl;
        int ret = index;
        pthread_mutex_lock(&hashlock[index]);
        HashNode *front = NULL;
        HashNode *entry = table[index];
        while(entry!=NULL&&(entry->getValue1().compare(songName)!=0||entry->getValue2().compare(socket)!=0)){
            front = entry;
            entry = entry->getNext();
        }
        if(entry==NULL){//not in there
            entry = new HashNode(hashKey,songName,socket);
            if(front == NULL){
                table[index]=entry;
            }
            else{
                front->setNext(entry);
            }
        }
        else if(entry->getValue1().compare(songName)==0||entry->getValue2().compare(socket)==0){
            ret = -1;
            //cout<<songName<<" at "<<socket<<" already exist! It is conflicting"<<endl;
        }
        pthread_mutex_unlock(&hashlock[index]);
        return ret;
    }

    int delet(unsigned int hashKey,string songName,string socket){
        int index = abs((int)hashKey%HashTableSize);
        //cout<<"delete_index="<<index<<endl;
        int ret = index;
        pthread_mutex_lock(&hashlock[index]);
        HashNode *front=NULL;
        HashNode *entry = table[index];
        while(entry!=NULL&&(entry->getValue1().compare(songName)!=0||entry->getValue2().compare(socket)!=0)){
            front = entry;
            entry = entry->getNext();
        }
        if(entry==NULL){// not found
            ret=-1;
        }
        else if(entry->getValue1().compare(songName)==0||entry->getValue2().compare(socket)==0){
            if(front==NULL){
                table[index]=NULL;
            }
            else{
                front->setNext(entry->getNext());
            }
        }
        pthread_mutex_unlock(&hashlock[index]);
        return ret;
    }

    void print(){
        for(int i=0;i<HashTableSize;i++){
            if(table[i]!=NULL){
                HashNode *entry = table[i];
                cout<<"Printing table..."<<endl;
                while(entry!=NULL){
                    cout<<"SongName: "<<entry->getValue1()<<" at "<<entry->getValue2()<<"\t HashKey:"<<entry->getKey()<<endl;
                    entry = entry->getNext();
                }
                cout<<endl;
            }
        }
    }
    ~HashTable(){
        for(int i = 0;i<HashTableSize;i++){
            if(table[i]!=NULL){
                HashNode *front = NULL;
                HashNode *entry = table[i];
                while(entry!=NULL){
                    front = entry;
                    entry = entry->getNext();
                    delete front;
                }
            }
        }
        delete[] table;
    }
};
