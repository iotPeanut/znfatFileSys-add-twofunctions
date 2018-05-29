#ifndef __SDX_H__
#define __SDX_H__
#include "STC15W4K.H"
sbit SPI_CS =P1^4; // SD卡片选 	    输入
sbit SPI_SI =P1^5; // SD卡同步数据  输入
sbit SPI_SO =P1^7; // SD卡同步数据  输出	
sbit SPI_CLK=P1^6; // SD卡同步时钟  输入
				   
#define DELAY_TIME 2000 //SD卡的复位与初始化时SPI的延时参数，根据实际速率修改其值，否则会造成SD卡复位或初始化失败
#define TRY_TIME 100   //向SD卡写入命令之后，读取SD卡的回应次数，即读TRY_TIME次，如果在TRY_TIME次中读不到回应，产生超时错误，命令写入失败

//相关宏定义
//-------------------------------------------------------------
#define SD_VER_ERR     0X00				 // 无法识别的版本
#define SD_VER_MMC     0X01				 // 版本号：MMC
#define SD_VER_V1      0X02				 // 版本号：SD1.0
#define SD_VER_V2      0X03			     // 版本号：SD2.0(普通SD)
#define SD_VER_V2HC    0X04				 // 版本号：SD2.0(高容量SDHC)

#define INIT_ERROR                  0x01 //初始化错误
#define INIT_CMD0_ERROR             0x02 //CMD0错误
#define INIT_CMD1_ERROR             0x03 //CMD1错误
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41错误
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41错误

#define WRITE_CMD24_ERROR           0x06 //写块时产生CMD24错误
#define WRITE_BLOCK_ERROR           0x07 //写块错误

#define READ_BLOCK_ERROR            0x08 //读块错误

#define WRITE_CMD25_ERROR           0x09 //在连续多块写时产生CMD25错误
#define WRITE_NBLOCK_ERROR          0x0A //连续多块写失败

#define READ_CMD18_ERROR            0x0B //在连续多块读时产生CMD18错误
 
#define GET_CSD_ERROR               0x0C //读CSD失败 

//-------------------------------------------------------------
unsigned char SD_Init();                                                 //SD卡初始化
unsigned char SD_Write_Sector(unsigned long addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入地址为addr的扇区中
unsigned char SD_Read_Sector(unsigned long addr,unsigned char *buffer);	 //从地址为addr的扇区中读取数据到buffer数据缓冲区中
unsigned char SD_Write_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入起始地址为addr的nsec个连续扇区中
unsigned char SD_Read_nSector(unsigned long nsec,unsigned long addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入起始地址为addr的nsec个连续扇区中
unsigned char SD_Erase_nSector(unsigned long addr_sta,unsigned long addr_end);
unsigned long SD_GetTotalSec(void);                                     //获取SD卡的总扇区数

#endif