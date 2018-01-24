#include <iostream>
#include "visa.h"
using namespace std;
ViSession vidg,vig;
const float PI=3.1415f;
const int PulseCount=4;//频率点数量
void CreateBurst(float f,float v,float wid,float perio);//产生正弦脉冲
void CreateMulFrePulse(int fs,float f1,float delf,float wid,float f0,float v,float repeat);//下载任意波脉冲
int main()
{
	long status=-1;
	viOpenDefaultRM(&vidg);
	status=viOpen(vidg,"TCPIP0::10.193.49.239::inst0::INSTR",VI_NULL,VI_NULL,&vig);
	if(status!=0)
	{
		cout<<"函数发生器连接不成功，请IP地址或本地连接再重新连接！"<<endl;
		viClose(vig);
		viClose(vidg);
		return 0;
	}
	else
	{
		viClear(vig);
		viPrintf(vig,"*rst\n");
		cout<<"Choose which signal do you want?1-pulse,2-arb."<<endl;
		int i;
		cin>>i;
		cin.get();
		if(i==1) 
		{
			float f, v, wid, period;
			cout<<"Input the freq,volt,width and period:"<<endl;
		    cin>>f>>v>>wid>>period;
		    cin.get();//接收最后的那个回车
			CreateBurst(f,v,wid,period);
		}
		else if(i=2)
		{
			float f1,delf,wid, v,repeat;
			cout<<"Input f1,delf,wid,v and repeat:"<<endl;
		    cin>>f1>>delf>>wid>>v>>repeat;
		    cin.get();//接收最后的那个回车
			CreateMulFrePulse(100000,f1,delf,wid,25,v,repeat);
		}
	}
	cin.get();//暂停，等待你输入一个字符
	viClose(vig);
	viClose(vidg);
	return 0;
}
void CreateBurst(float f,float v,float wid,float perio)
{
	float ncyd=f*wid;
	int ncyi=(int) ncyd;
	if(ncyd-ncyi>=0.5) ncyi+=1;
	//viPrintf(vig,"function sin\n");
	viPrintf(vig,"frequency %lf\n",f);
	viPrintf(vig,"voltage %lf\n",v);
	viPrintf(vig,"burst:mode triggered\n");
	viPrintf(vig,"burst:ncycles %d\n",ncyi);
	viPrintf(vig,"burst:internal:period %lf\n",perio);
	viPrintf(vig,"burst:phase 0\n");
	viPrintf(vig,"trigger:source immediate\n");
	viPrintf(vig,"burst:state on\n");
	viPrintf(vig,"output:sync on\n");
	viPrintf(vig,"output on\n");
}
void CreateMulFrePulse(int fs,float f1,float delf,float wid,float f0,float v,float repeat)
{
	char SCPIcmd[1000000];
	viPrintf(vig,"*rst\n");
	viPrintf(vig,"*cls\n");
	int point=(int)(wid*fs);//每个脉冲的点数
	int points=(int)(5*wid*fs);//每个频率段的点数
	strcpy_s(SCPIcmd,"data volatile");
	float f;
	for(int i=0;i<PulseCount;i++)
	{
		f=f1+i*delf;
		
		for(int j=0;j<point;j++)
		{
			if(0==i&&0==j) sprintf_s(SCPIcmd,"%s, %4.2f",SCPIcmd,sin(2*PI*f*j/fs));
			//data volatile，后面有一个空格，之后才是数据
			else sprintf_s(SCPIcmd,"%s,%4.2f",SCPIcmd,sin(2*PI*f*j/fs));
			//使用ASCII码的方式载入数据
		}
		for(int k=point;k<points;k++)
			strcat_s(SCPIcmd,",0.00");
	}
	strcat_s(SCPIcmd,"\n");
	viPrintf(vig,SCPIcmd);
	viPrintf(vig,"data:copy M_ARB, volatile\n");
	viPrintf(vig,"function:user M_ARB\n");
	viPrintf(vig,"function:shape user\n");
	viPrintf(vig,"output:load 50\n");
	viPrintf(vig,"frequency %f;voltage %f\n",f0,v);
	viPrintf(vig,"burst:mode triggered\n");
	viPrintf(vig,"burst:ncycles 1\n");
	viPrintf(vig,"burst:internal:period %f\n",repeat);
	viPrintf(vig,"burst:phase 0\n");
	viPrintf(vig,"trigger:source immediate\n");
	viPrintf(vig,"burst:state on\n");
	viPrintf(vig,"output:sync on\n");
	viPrintf(vig,"output on\n");
	
}