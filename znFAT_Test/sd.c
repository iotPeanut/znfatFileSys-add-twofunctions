#include "sd.h"
#include "myfun.h" 
#include "uart.h"

//变量定义
//--------------------------------------------------------------
bit is_init;  //在初始化的时候设置此变量为1,同步数据传输（SPI）会放慢 

unsigned char Addr_Mode=0;        // 寻址方式，1为块寻址，0为字节寻址（使用时需将地址左移9位）
unsigned char SD_Ver=SD_VER_ERR;  // SD卡的版本
//---------------------------------------------------------------

unsigned char bdata _dat; //用于优化模拟SPI的中间bdata类型变量，方便取各个位
sbit _dat7=_dat^7;
sbit _dat6=_dat^6;
sbit _dat5=_dat^5;
sbit _dat4=_dat^4;
sbit _dat3=_dat^3;
sbit _dat2=_dat^2;
sbit _dat1=_dat^1;
sbit _dat0=_dat^0;

unsigned char IO_SPI_Init(void)  // 对SPI有关初始化
{
	//P1M1=0xf0;	  // 1111 0000	 // 设置P1.5.6.7.8 开漏输出
	//P1M0=0xf0;	  // 1111 0000	
	SPI_SO=1;	  // IO配置 	
	return 0;
}

/******************************************************************
功能：IO模拟SPI，发送一个字节
参数：x是要发送的字节
      其中is_init为1时，写的速度放慢，初始化SD卡SPI速度不能太高
 ******************************************************************/

void SD_SPI_WByte(unsigned char x) 
{
	_dat=x;
	
	SPI_SI=_dat7;
	SPI_CLK=0; 
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat6;
	SPI_CLK=0; 
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat5;
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat4;
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat3;
	SPI_CLK=0; 
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat2;
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat1;
	SPI_CLK=0; 
	if(is_init) delay(DELAY_TIME); 
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	
	SPI_SI=_dat0;
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);  
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
}

/******************************************************************
功能：IO模拟SPI，读取一个字节
返回：返回读到的字节
 ******************************************************************/
unsigned char SD_SPI_RByte() //SPI读一个字节
{  
	SPI_SO=1;
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME); 
	_dat7=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat6=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat5=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat4=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME); 
	_dat3=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat2=SPI_SO; 
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat1=SPI_SO;
	
	SPI_CLK=1;
	if(is_init) delay(DELAY_TIME);
	SPI_CLK=0;
	if(is_init) delay(DELAY_TIME);
	_dat0=SPI_SO; 
	
	return (_dat);
}

/******************************************************************
功能：向SD卡写命令
参数：SD卡的命令是6个字节，pcmd是指向命令字节序列的指针
返回：命令写入后，调用不成功，将返回0xff
 ******************************************************************/

unsigned char SD_Write_Cmd(unsigned char *pcmd) 
{
	unsigned char temp,time=0;
	
	SPI_CS=1;
	SD_SPI_WByte(0xff); //提高兼容性，如果没有这里，有些SD卡可能不支持   
	SPI_CS=0;
	while(0XFF!=SD_SPI_RByte()); //等待SD卡准备好，再向其发送命令

	SD_SPI_WByte(pcmd[0]);
	SD_SPI_WByte(pcmd[1]);
	SD_SPI_WByte(pcmd[2]);
	SD_SPI_WByte(pcmd[3]);
	SD_SPI_WByte(pcmd[4]);
	SD_SPI_WByte(pcmd[5]);
	if(pcmd[0]==0X1C) SD_SPI_RByte(); // 如果是停止命令，跳过多余的字节
	do 
	{  
		temp = SD_SPI_RByte();        // 一直读，直到读到的不是0xff或超时
		time++;
	}while((temp==0xff)&&(time<TRY_TIME)); 
	
	return(temp);
}

 /******************************************************************
功能：SD卡初始化，针对于不同的SD卡，如MMC、SD或SDHC，初始化方法是不同的
返回：调用成功，返回0x00，否则返回错误码
 ******************************************************************/
unsigned char SD_Init()	
{  
	unsigned char time=0,temp=0,i=0; 
	unsigned char rbuf[4]={0};
	
	unsigned char pCMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0，将SD卡从默认上电后的SD模式切换到SPI模式，使SD卡进入IDLE状态
	unsigned char pCMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1，MMC卡使用CMD1命令进行初始化
	unsigned char pCMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8，用于鉴别SD卡的版本，并可从应答得知SD卡的工作电压
	unsigned char pCMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16，设置扇区大小为512字节，此命令用于在初始化完成之后进行试探性的操作，
	                                                         //如果操作成功，说明初始化确实成功
	unsigned char pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD，即应用层命令 CMD55+ACMD41配合使用
	                                                         //MMC卡使用CMD1来进行初始化，而SD卡则使用CMD55+ACMD41来进行初始化
	unsigned char pACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对2.0的SD卡
	unsigned char pACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对1.0的SD卡
	
	unsigned char pCMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58，用于鉴别SD2.0到底是SDHC，还是普通的SD卡，二者对扇区地址的寻址方式不同
 
	//******************** 复位SD卡开始，用到CMD0，使用SD卡切换到SPI模式********************// 
	IO_SPI_Init();             // 对SPI有关IO口初始化
	is_init=1;                 // is_init置为1，让SPI速度慢下来
	SPI_CS=1;
	for(i=0;i<0x0f;i++)        // 首先要发送最少74个时钟信号，这是必须的！激活SD卡
	{
		SD_SPI_WByte(0xff);    // 120个时钟
	}  
	time=0;				
	do
	{ 
		temp=SD_Write_Cmd(pCMD0);     //写入CMD0
		time++;
		if(time==TRY_TIME) 
		{ 
			return(INIT_CMD0_ERROR); //CMD0写入失败
		}
	}while(temp!=0x01);
	//*************************** 复位SD卡结束 ********************************************// 

	if(1==SD_Write_Cmd(pCMD8))      //写入CMD8，如果返回值为1，则SD卡版本为2.0
	{
		rbuf[0]=SD_SPI_RByte(); rbuf[1]=SD_SPI_RByte(); //读取4个字节的R7回应，通过它可知此SD卡是否支持2.7~3.6V的工作电压
		rbuf[2]=SD_SPI_RByte(); rbuf[3]=SD_SPI_RByte(); 
		 
		if(rbuf[2]==0X01 && rbuf[3]==0XAA)  //SD卡是否支持2.7~3.6V,条件成立则支持
		{		
			time=0;
			do
			{								   
				SD_Write_Cmd(pCMD55);       // 写入CMD55
				temp=SD_Write_Cmd(pACMD41H);   // 写入ACMD41，针对SD2.0
				time++;
				if(time>=TRY_TIME) 
				{ 
					return(INIT_SDV2_ACMD41_ERROR);//对SD2.0使用ACMD41进行初始化时产生错误
				}
			}while(temp!=0);	
			
			if(0==SD_Write_Cmd(pCMD58)) //写入CMD58，开始鉴别SD2.0
			{
				rbuf[0]=SD_SPI_RByte(); rbuf[1]=SD_SPI_RByte();  //读取4个字节的OCR，其中CCS指明了是SDHC还是普通的SD
				rbuf[2]=SD_SPI_RByte(); rbuf[3]=SD_SPI_RByte(); 	
				
				if(rbuf[0]&0x40) 
				{
					SD_Ver=SD_VER_V2HC; //SDHC卡	
					Addr_Mode=1; //SDHC卡的扇区寻址方式是扇区胤
				}	
				else SD_Ver=SD_VER_V2; //普通的SD卡，2.0的卡包含SDHC和一些普通的卡				
			}
		}
	}
	else                          // SD V1.0或MMC 
	{
		// SD卡使用ACMD41进行初始化，而MMC使用CMD1来进行初始化，依此来进一步判断是SD还是MMC
		SD_Write_Cmd(pCMD55);            // 写入CMD55
		temp=SD_Write_Cmd(pACMD41S);     // 写入ACMD41，针对SD1.0
		
		if(temp<=1) //检查返回值是否正确，如果正确，说明ACMD41命令被接受，即为SD卡
		{
			SD_Ver=SD_VER_V1; //普通的SD1.0卡，一般来说容量不会超过2G				
			time=0;
			do
			{
				SD_Write_Cmd(pCMD55);//写入CMD55
				temp=SD_Write_Cmd(pACMD41S);//写入ACMD41，针对SD1.0
				time++;
				if(time>=TRY_TIME) 
				{ 
					return(INIT_SDV1_ACMD41_ERROR);//对SD1.0使用ACMD41进行初始化时产生错误
				}
			}while(temp!=0);			 
		}
		else //否则为MMC	
		{
			SD_Ver=SD_VER_MMC; //MMC卡，它不支持ACMD41命令，而是使用CMD1进行初始化				
			time=0;
			do
			{ 
				temp=SD_Write_Cmd(pCMD1);//写入CMD1
				time++;
				if(time>=TRY_TIME) 
				{ 
					return(INIT_CMD1_ERROR);//MMC卡使用CMD1命令进行初始化中产生错误
				}
			}while(temp!=0);			
		}
	}
	
	if(0!=SD_Write_Cmd(pCMD16)) //SD卡的块大小必须为512字节
	{
		SD_Ver=SD_VER_ERR; //如果不成功，则此卡为无法识别的卡
		return INIT_ERROR;
	}	
	
	SPI_CS=1;             // 关闭SD卡的片选 
	SD_SPI_WByte(0xFF);   // 按照SD卡的操作时序在这里补8个时钟 
	
	is_init=0;            // 初始化完毕，将is_init设置为0,为了提高以后的数据传输速度
	
	return 0;              //返回0,说明初始化操作成功
}  

/****************************************************************************
功能：将buffer指向的512个字节的数据写入到SD卡的addr扇区中
参数：addr:扇区地址
      buffer:指向数据缓冲区的指针
返回：调用成功，返回0x00，否则返回错误码
      SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
 ****************************************************************************/
unsigned char SD_Write_Sector(unsigned long addr,unsigned char *buffer)	//向SD卡中的指定地址的扇区写入512个字节，使用CMD24（24号命令）
{  
	unsigned char temp,time;
	unsigned char i=0;
	unsigned char pCMD24[]={0x58,0x00,0x00,0x00,0x00,0xff}; //向SD卡中单个块（512字节，一个扇区）写入数据，用CMD24
	
	if(!Addr_Mode) addr<<=9; //addr = addr * 512	将块地址（扇区地址）转为字节地址
	
	pCMD24[1]=addr>>24; //将字节地址写入到CMD24字节序列中
	pCMD24[2]=addr>>16;
	pCMD24[3]=addr>>8;
	pCMD24[4]=addr;		
	time=0;
	do
	{  
		temp=SD_Write_Cmd(pCMD24);
		time++;
		if(time==TRY_TIME) 
		{ 
			return(temp); //命令写入失败
		}  
	}while(temp!=0); 	
	while(0XFF!=SD_SPI_RByte()); //等待SD卡准备好，再向其发送命令及后续的数据 	
	SD_SPI_WByte(0xFE);          //写入开始字节 0xfe，后面就是要写入的512个字节的数据
		
	for(i=0;i<4;i++) //将缓冲区中要写入的512个字节写入SD卡，减少循环次数，提高数据写入速度
	{
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
	}
	
	SD_SPI_WByte(0xFF); 
	SD_SPI_WByte(0xFF); //两个字节的CRC校验码，不用关心	   
	temp=SD_SPI_RByte();   //读取返回值
	if((temp & 0x1F)!=0x05) //如果返回值是 XXX00101 说明数据已经被SD卡接受了
	{
		return(WRITE_BLOCK_ERROR); //写块数据失败
	} 	
	while(0xFF!=SD_SPI_RByte());  //等到SD卡不忙（数据被接受以后，SD卡要将这些数据写入到自身的FLASH中，需要一个时间）
						                 //忙时，读回来的值为0x00,不忙时，为0xff 	
	SPI_CS=1;
	SD_SPI_WByte(0xFF); //按照SD卡的操作时序在这里补8个时钟  	
	return(0);		 //返回0,说明写扇区操作成功
} 

/****************************************************************************
功能：读取addr扇区的512个字节到buffer指向的数据缓冲区
参数：addr:扇区地址
      buffer:指向数据缓冲区的指针
返回：调用成功，返回0x00，否则返回错误码
      SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
 ****************************************************************************/

unsigned char SD_Read_Sector(unsigned long addr,unsigned char *buffer)//从SD卡的指定扇区中读出512个字节，使用CMD17（17号命令）
{
	 unsigned char i;
	 unsigned char time,temp;		
	 unsigned char pCMD17[]={0x51,0x00,0x00,0x00,0x00,0x01}; //CMD17的字节序列	   
	 if(!Addr_Mode) addr<<=9; //sector = sector * 512	   将块地址（扇区地址）转为字节地址
	
	 pCMD17[1]=addr>>24; //将字节地址写入到CMD17字节序列中
	 pCMD17[2]=addr>>16;
	 pCMD17[3]=addr>>8;
	 pCMD17[4]=addr;	
	
	 time=0;
	 do
	 {  
		temp=SD_Write_Cmd(pCMD17); //写入CMD17
		time++;
		if(time==TRY_TIME) 
		{
			return(READ_BLOCK_ERROR); //读块失败
		}
	 }while(temp!=0); 	   			
	 while(SD_SPI_RByte()!= 0xFE); //一直读，当读到0xfe时，说明后面的是512字节的数据了
	
	 for(i=0;i<4;i++)	 //将数据写入到数据缓冲区中
	 {	
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	  *(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
	 }
	
	 SD_SPI_RByte();
	 SD_SPI_RByte();//读取两个字节的CRC校验码，不用关心它们	 	
	 SPI_CS=1;
	 SD_SPI_WByte(0xFF); //按照SD卡的操作时序在这里补8个时钟 	
	 return 0;
}

///****************************************************************************
// 功能：向addr扇区开始的nsec个扇区写入数据（★硬件多扇区写入）
// 参数：nsec:扇区数
//       addr:开始扇区地址
//       buffer:指向数据缓冲区的指针
// 返回：调用成功，返回0x00，否则返回错误码
//       SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
// ****************************************************************************/	 
//unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)	
//{  
//	unsigned char temp,time;
//	unsigned long i=0,j=0;		
//	unsigned char pCMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25用于完成多块连续写
//	unsigned char pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD,CMD55+ACMD23
//	unsigned char pACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23，多块连续预擦除	
//	
//	if(!Addr_Mode) addr<<=9; 	
//	pCMD25[1]=addr>>24;
//	pCMD25[2]=addr>>16;
//	pCMD25[3]=addr>>8;
//	pCMD25[4]=addr;
//	
//	pACMD23[1]=nsec>>24;
//	pACMD23[2]=nsec>>16;
//	pACMD23[3]=nsec>>8;
//	pACMD23[4]=nsec; 
//	
//	if(SD_Ver!=SD_VER_MMC) //如果不是MMC卡，则首先写入预擦除命令，CMD55+ACMD23，这样后面的连续块写的速度会更快
//	{
//		SD_Write_Cmd(pCMD55);
//		SD_Write_Cmd(pACMD23);
//	}
//	
//	time=0;
//	do
//	{  
//		temp=SD_Write_Cmd(pCMD25);
//		time++;
//		if(time==TRY_TIME) 
//		{ 
//			return(WRITE_CMD25_ERROR); //命令写入失败
//		}
//	}while(temp!=0); 	
//	while(0XFF!=SD_SPI_RByte()); //等待SD卡准备好，再向其发送命令及后续的数据 	
//	for(j=0;j<nsec;j++)
//	{
//		SD_SPI_WByte(0xFC);//写入开始字节 0xfc，后面就是要写入的512个字节的数据	
//		
//		for(i=0;i<4;i++) //将缓冲区中要写入的512个字节写入SD卡
//		{
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));SD_SPI_WByte(*(buffer++));
//		}
//		
//		SD_SPI_WByte(0xFF); 
//		SD_SPI_WByte(0xFF); //两个字节的CRC校验码，不用关心
//		   
//		temp=SD_SPI_RByte();   //读取返回值
//		if((temp & 0x1F)!=0x05) //如果返回值是 XXX00DELAY_TIME1 说明数据已经被SD卡接受了
//		{
//			return(WRITE_NBLOCK_ERROR); //写块数据失败
//		}  	
//		while(0xFF!=SD_SPI_RByte());//等到SD卡不忙（数据被接受以后，SD卡要将这些数据写入到自身的FLASH中，需要一个时间）
//							                  //忙时，读回来的值为0x00,不忙时，为0xff
//	} 	
//	SD_SPI_WByte(0xFD);		
//	while(0xFF!=SD_SPI_RByte());
//	
//	SPI_CS=1;      //关闭片选
//	
//	SD_SPI_WByte(0xFF);//按照SD卡的操作时序在这里补8个时钟
//	
//	return(0);		 //返回0,说明写扇区操作成功
//} 
//
///****************************************************************************
// 功能：读取addr扇区开始的nsec个扇区的数据（★硬件多扇区读取）
// 参数：nsec:扇区数
//       addr:开始扇区地址
//       buffer:指向数据缓冲区的指针
// 返回：调用成功，返回0x00，否则返回错误码
//       SD卡初始化成功后，读写扇区时，尽量将SPI速度提上来，提高效率
// ****************************************************************************/
//
//unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer)
//{
//	 unsigned char temp,time;
//	 unsigned long i=0,j=0;		
//	 unsigned char pCMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18的字节序列
//	 unsigned char pCMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; //CMD12，强制停止命令	   
//	 if(!Addr_Mode) addr<<=9; //sector = sector * 512	   将块地址（扇区地址）转为字节地址
//	
//	 pCMD18[1]=addr>>24; //将字节地址写入到CMD17字节序列中
//	 pCMD18[2]=addr>>16;
//	 pCMD18[3]=addr>>8;
//	 pCMD18[4]=addr;	
//	 time=0;
//	 do
//	 {  
//		temp=SD_Write_Cmd(pCMD18); //写入CMD18
//		time++;
//		if(time==TRY_TIME) 
//		{
//			return(READ_CMD18_ERROR); //写入CMD18失败
//		}
//	 }while(temp!=0); 
//	 	 
//	 for(j=0;j<nsec;j++)
//	 {  
//		while(SD_SPI_RByte()!= 0xFE); //一直读，当读到0xfe时，说明后面的是512字节的数据了 
//				
//		for(i=0;i<4;i++)	 //将数据写入到数据缓冲区中
//		{	
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();*(buffer++)=SD_SPI_RByte();
//		} 		
//		SD_SPI_RByte();
//		SD_SPI_RByte();//读取两个字节的CRC校验码，不用关心它
//	 } 
//	 	
//	 SD_Write_Cmd(pCMD12); //写入CMD12命令，停止数据读取 	
//	 SPI_CS=1;
//	 SD_SPI_WByte(0xFF); //按照SD卡的操作时序在这里补8个时钟	
//	 return 0;
//}
//
//
///******************************************************************
// 功能：对SD卡若干个扇区进行擦除，擦除后扇区中的数据大部分情况
//       下为全0（有些卡擦除后为全0XFF，如要使用此函数，请确认）
// 参数：addr_sta：开始扇区地址   addr_end：结束扇区地址
// 返回：调用成功，返回0x00，否则返回错误码
// ******************************************************************/
//unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end)
//{
//	unsigned char temp,time;
//	unsigned char i=0;
//	unsigned char pCMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //设置擦除的开始扇区地址
//	unsigned char pCMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //设置擦除的结束扇区地址
//	unsigned char pCMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //擦除扇区
//	
//	if(!Addr_Mode) {addr_sta<<=9;addr_end<<=9;} //addr = addr * 512	将块地址（扇区地址）转为字节地址
//	
//	pCMD32[1]=addr_sta>>24; //将开始地址写入到CMD32字节序列中
//	pCMD32[2]=addr_sta>>16;
//	pCMD32[3]=addr_sta>>8;
//	pCMD32[4]=addr_sta;	 
//	
//	pCMD33[1]=addr_end>>24; //将开始地址写入到CMD32字节序列中
//	pCMD33[2]=addr_end>>16;
//	pCMD33[3]=addr_end>>8;
//	pCMD33[4]=addr_end;	
//	
//	time=0;
//	do
//	{  
//		temp=SD_Write_Cmd(pCMD32);
//		time++;
//		if(time==TRY_TIME) 
//		{ 
//			return(temp); //命令写入失败
//		}
//	}while(temp!=0);  
//	
//	time=0;
//	do
//	{  
//		temp=SD_Write_Cmd(pCMD33);
//		time++;
//		if(time==TRY_TIME) 
//		{ 
//			return(temp); //命令写入失败
//		}
//	}while(temp!=0);  
//	
//	time=0;
//	do
//	{  
//		temp=SD_Write_Cmd(pCMD38);
//		time++;
//		if(time==TRY_TIME) 
//		{ 
//			return(temp); //命令写入失败
//		}
//	}while(temp!=0); 	
//	return 0; 	 
//}
//
//
/****************************************************************************
 功能：获取SD卡的总扇区数（通过读取SD卡的CSD寄器组计算得到总扇区数）
 返回：返回SD卡的总扇区数
 ****************************************************************************/
unsigned long SD_GetTotalSec(void)
{
	unsigned char pCSD[16];
	unsigned long Capacity;  
	unsigned char n,i;
	unsigned short csize; 	
	unsigned char pCMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	
	
	if(SD_Write_Cmd(pCMD9)!=0) //写入CMD9命令
	{
		return GET_CSD_ERROR; //获取CSD时产生错误
	} 	
	while(SD_SPI_RByte()!= 0xFE); //一直读，当读到0xfe时，说明后面的是16字节的CSD数据 	
	for(i=0;i<16;i++) pCSD[i]=SD_SPI_RByte(); //读取CSD数据	
	SD_SPI_RByte();
	SD_SPI_RByte();      // 读取两个字节的CRC校验码，不用关心它们
	
	SPI_CS=1;
	SD_SPI_WByte(0xFF);  // 按照SD卡的操作时序在这里补8个时钟 
	
	//如果为SDHC卡，即大容量卡，按照下面方式计算
	if((pCSD[0]&0xC0)==0x40)	 //SD2.0的卡
	{	
		csize=pCSD[9]+(((unsigned short)(pCSD[8]))<<8)+1;
		Capacity=((unsigned long)csize)<<10;                    // 得到扇区数	 		   
	}
	else //SD1.0的卡
	{	
		n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
		csize=(pCSD[8]>>6)+((unsigned short)pCSD[7]<<2)+((unsigned short)(pCSD[6]&0x03)<<0x0A)+1;
		Capacity=(unsigned long)csize<<(n-9);                  // 得到扇区数   
	}
	return Capacity;
}