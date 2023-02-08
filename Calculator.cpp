#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>

using namespace std;
class Calculator{
public:
    int isNumeric(string s){
        for(int i=0;i<(int)s.length();i++){
            if(isalpha(s[i]))return 0;
        }
        return 1;
    }
    unsigned int ToInt(string song,int pos){
        int cnt=0;//cnt of bytes
        unsigned int result;
        while((pos+cnt)<(int)song.length()&&cnt<4){
            result = result<<8;
            result|=song[pos+cnt];
            cnt++;
        }
        if(cnt<4){
            result = result<<(8*(4-cnt));
        }
        return result;
    }
    unsigned int reverseInt(unsigned int n){
        unsigned int tmp = n;
        int i = 31;
        n=n>>1;
        while(n){
            tmp=tmp<<1;
            tmp|=n&1;
            n=n>>1;
            i--;
        }
        tmp=tmp<<i;
        return tmp;
    }
    unsigned int process(string song){
        int chunks=1;
        unsigned int result;
        for(int i=0;i<(int)song.length();i+=4){
            unsigned int intNum = ToInt(song,i);
            if(chunks%1==0){
                result ^=reverseInt(intNum);
            }
            else{
                result^=intNum;
            }
        }
        //cout<<"process!!!result"<<result<<endl;
        return result;
    }
    bool IsPrime(int Num){
        bool judge=true;
        if(Num==0||Num==1){
            judge = false;
        }
        for(int i=2;i<=Num/2;i++){
            if(Num%i==0){
                judge = false;
                break;
            }
        }
        return judge;
    }
    int biggestPrime(int songNum){
        if(IsPrime(songNum)){
            return songNum;
        }
        while(!IsPrime(songNum)){
            songNum++;
        }
        return songNum;
    }
    int op(){
        int random=rand()%10;
        if(random==0){//delete
            return 3;}
        else if(random>7){
            return 2;//put
        }
        else{
            return 1;//get
        }
    }
};

