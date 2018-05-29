#include "znfat/ZNFAT.H"
#include "sd.h"
#include "uart.h"
#include "string.h"
unsigned int myTimer=0,first,last;
struct znFAT_Init_Args idata Init_Args; // 初始化参数集合
struct FileInfo idata fileinfo;         // 文件信息集合
unsigned char buf[21]={0};
void port_mode()            // 端口模式
{
	P0M1=0x00; P0M0=0x00;P1M1=0x00; P1M0=0x00;P2M1=0x00; P2M0=0x00;P3M1=0x00; P3M0=0x00;
	P4M1=0x00; P4M0=0x00;P5M1=0x00; P5M0=0x00;P6M1=0x00; P6M0=0x00;P7M1=0x00; P7M0=0x00; 
}
void Timer0Init(void)		//@22.1184MHz
{
	AUXR &= 0x7F;		
	TMOD &= 0xF0;		
	TL0 = 0xCD;		
	TH0 = 0xF8;		
	TF0 = 0;		
	TR0 = 1;		
	 ET0 = 1;
}
UINT32 fileNum=0;
int main()
{ 
	//unsigned int res=0;
	int test=0,i=0;
	unsigned int res=0,len=0; 
	Timer0Init();
	port_mode();	                   // 所有IO口设为准双向弱上拉方式。
	UART_Init(); 					   // 波特率：9600 /22.1184MHZ
	UART_Send_Str("串口设置完毕\r\n");
	znFAT_Device_Init();                     // 存储设备初始化
	UART_Send_Str("SD卡初始化完毕\r\n");
	znFAT_Select_Device(0,&Init_Args);       // 选择设备
	res=znFAT_Init();      // 文件系统初始化	 
	if(!res)               // 文件系统初始化成功
	{
		UART_Send_Str("文件系统初始化成功\r\n");
		UART_Send_StrNum("DBR扇区地址:",Init_Args.BPB_Sector_No);   
		UART_Send_StrNum("总存储容量(K字节):",Init_Args.Total_SizeKB); 
		UART_Send_StrNum("每扇区字节(个):",Init_Args.BytesPerSector); 
		UART_Send_StrNum("FAT扇区数:",Init_Args.FATsectors);  
		UART_Send_StrNum("每簇扇区(个):",Init_Args.SectorsPerClust); 
		UART_Send_StrNum("第一FAT扇区:",Init_Args.FirstFATSector); 
		UART_Send_StrNum("根目录开始扇区:",Init_Args.FirstDirSector); 
		UART_Send_StrNum("FSINFO扇区地址:",Init_Args.FSINFO_Sec);
		UART_Send_StrNum("下一空闲簇:",Init_Args.Next_Free_Cluster);
		UART_Send_StrNum("剩余空闲簇:",Init_Args.Free_nCluster); 
	}
	else                  // 文件系统初始化失败
	{
		UART_Send_StrNum("文件系统初始化失败 , 错误码:",res);
	}	
	res=znFAT_Modify_Name(&fileinfo,"/unsend*.txt","12345.txt",0,1);
	if(!res)
	{
		UART_Send_Str("修改成功！");
	}else
	{
		UART_Send_StrNum("错误代码：",res); 
	}
	first=myTimer;
	addFAT_Find_File_Num(&fileNum,"/test");
	znFAT_Flush_FS();     // 刷新文件系统
	
	UART_Send_StrNum("file num is :",fileNum);
	UART_Send_StrNum("\ntime is ",last-first);
	UART_Send_StrNum("\n times",i);
	while(1);
	return 0;
} 

void timer0() interrupt 1 
{
	myTimer++;
}



