#ifndef _DEVICE_IO_H_
#define _DEVICE_IO_H_
struct znFAT_IO_Ctl //底层驱动接口的IO频度控制体 
{
 UINT32 just_sec;
 UINT8  just_dev;
};

UINT8 znFAT_Device_Init(void);
UINT8 znFAT_Device_Read_Sector(UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Write_Sector(UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer);
UINT8 znFAT_Device_Clear_nSector(UINT32 nsec,UINT32 addr);

#endif