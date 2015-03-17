/* mc-typ-04: ver 1.0 */

#include "mbed.h"
#include <stdio.h>
#include <string>

string MC_ID="0";

Serial mPc(USBTX, USBRX);
Timer mTimer;

AnalogIn inSen1(dp13);
AnalogIn inSen2(dp11);
AnalogIn inSen3(dp10);
AnalogIn inSen4(dp9 );

DigitalOut outRelary_1(dp14);
DigitalOut outRelary_2(dp28);
DigitalOut outRelary_3(dp26);
DigitalOut outRelary_4(dp25);

int mSTAT =0;
int mSTAT_CONECT=1;
int mSTAT_VALVE=2;
int mTmMax=10;
int mValveGet = 20;
int mCtOpen   = 0;

string mResponse3="";

//Struct
struct stParam{
 string stat;
 string statCode;
 string moi_num;
 string vnum_1;
 string vnum_2;
 string vnum_3;
 string vnum_4;
 string kai_num_1;
 string kai_num_2;
 string kai_num_3;
 string kai_num_4;
};

string conv_zeroStr(string src ,int width){
    string ret="00000000";
    ret +=src;
    int iNum = ret.length();
    ret = ret.substr(iNum-width);
    return ret;    
}

void proc_start() {
    mResponse3="";
    int iSen  =0;
    int iSen2 =0;
    int iSen3 =0;
    int iSen4 =0;
    float fSen  = inSen1;
    float fSen2 = inSen2;
    float fSen3 = inSen3;
    float fSen4 = inSen4;
    iSen  = int(fSen  * 1000);
    iSen2 = int(fSen2 * 1000);
    iSen3 = int(fSen3 * 1000);
    iSen4 = int(fSen4 * 1000);
    
    char buff[]="0000";
    string sPut="put_dat=";
    sPut += conv_zeroStr(MC_ID ,4);
    sprintf(buff, "%d", iSen);
    sPut +=conv_zeroStr(buff ,4);
    sprintf(buff, "%d", iSen2);
    sPut +=conv_zeroStr(buff ,4);
    sprintf(buff, "%d", iSen3);
    sPut +=conv_zeroStr(buff ,4);
    sprintf(buff, "%d", iSen4);
    sPut +=conv_zeroStr(buff ,4);
    
    mPc.printf("%s\n", sPut.c_str() );
}
    
void proc_ser(){
    if( mPc.readable()) {
        char c= mPc.getc();
        mResponse3+= c;
    }else{
        if(mResponse3.length() > 31){
            printf("==== response ==== \n");
            string sRes4 = mResponse3.substr(8);
            printf("%s\n" ,sRes4.c_str() );
            mResponse3="";
            printf("==== response ====.END \n");
            struct stParam param;
            param.stat     = sRes4.substr(0,1);
            param.statCode = sRes4.substr(1,3);
            param.moi_num  = sRes4.substr(4,4);
            param.vnum_1   = sRes4.substr(8, 1);
            param.vnum_2   = sRes4.substr(9, 1);
            param.vnum_3   = sRes4.substr(10,1);
            param.vnum_4   = sRes4.substr(11,1);      
            param.kai_num_1= sRes4.substr(12,3);      
            param.kai_num_2= sRes4.substr(15,3);      
            param.kai_num_3= sRes4.substr(18,3);      
            param.kai_num_4= sRes4.substr(21,3);  
printf("stat=%s \n" , param.stat.c_str() );            
            if(param.stat=="1"){
printf("param.stat=1 \n");                
                if((param.vnum_1=="1") || (param.vnum_2=="1") || (param.vnum_3=="1") || (param.vnum_4=="1")){
                    mSTAT =mSTAT_VALVE;
                    mValveGet = atoi( param.kai_num_1.c_str());
                    if(param.vnum_1=="1"){
printf("vnum_1.STart\n");
                        outRelary_1=1;
                    }
                    if(param.vnum_2=="1"){
                        outRelary_2=1;
                    }
                    if(param.vnum_3=="1"){
                        outRelary_3=1;
                    }
                    if(param.vnum_4=="1"){
                        outRelary_4=1;
                    }
                }
            }
        }
    }
}

void proc_valve(){
    char buff[]="0000";
    sprintf(buff, "%d", mCtOpen);
printf("valve-open=%s\n" , buff);
    if(mCtOpen >= mValveGet){
        outRelary_1=0;
        outRelary_2=0;
        outRelary_3=0;
        outRelary_4=0;
        mResponse3="";
        mSTAT = mSTAT_CONECT;
        mCtOpen=0;
    }
    mResponse3="";
    mCtOpen ++;
    wait(1);
}
    
int main() {
    mSTAT = mSTAT_CONECT;
    mTimer.start();
    printf("start mc-typ-04.\n");
    while(1) {
        float sec= mTimer.read();
        if(mSTAT == mSTAT_CONECT){
            if(sec >= mTmMax){
                proc_start();
                mTimer.reset();
            }else{
                proc_ser();
            }
        }else if(mSTAT == mSTAT_VALVE){
           proc_valve();
        }
    }
}
