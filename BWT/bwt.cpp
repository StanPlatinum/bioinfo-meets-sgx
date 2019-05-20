#include <iostream>
#include <string>
#include <algorithm>
#include <string.h>

using namespace std;

///编码，生成last数组
int getLastArray(char *lastArray,const string &str){    ///子串排序
    int len=str.size();
    string array[len];

    for(int i=0;i<len;i++){
        array[i] = str.substr(i);
    }
    sort(array,array+len);
    for(int i=0;i<len;i++){
        lastArray[i] = str.at((2*len-array[i].size()-1)%len);
    }
    return 0;
}

int getCountPreSum(int *preSum,const string &str){
    memset(preSum,0,27*sizeof(int));
    for(int i=0;i<str.size();i++){
        if(str.at(i) == '#')
            preSum[0]++;
        else
            preSum[str.at(i)-'a'+1]++;
    }

    for(int i=1;i<27;i++)
        preSum[i] += preSum[i-1];
    return 0;
}

///解码，使用last数组，恢复原来的文本块
int regainTextFromLastArray(char *lastArray,char *reGainStr,int *preSum){
    int len=strlen(lastArray);
    int pos=0;
    char c;
    for(int i=len-1;i>=0;){
        reGainStr[i] = lastArray[pos];
        c = lastArray[pos];
        pos = preSum[c-'a']+count(lastArray,lastArray+pos,c);
        i--;
    }
    return 0;
}

int main (){
    string str("sdfsfdfdsdfgdfgfgfggfgdgfgd#");
    int preSum[27];
    int len=str.size();

    char *lastArray = new char[len+1];
    char *reGainStr = new char[len+1];
    lastArray[len]='\0';
    reGainStr[len]='\0';

    getCountPreSum(preSum,str);
    getLastArray(lastArray,str);
    regainTextFromLastArray(lastArray,reGainStr,preSum);

    cout<<"       str: "<<str<<endl;
    cout<<"lastArray : "<<lastArray<<endl;
    cout<<"reGainStr : "<<reGainStr<<endl;

    delete lastArray;
    delete reGainStr;
    return 0;
}
