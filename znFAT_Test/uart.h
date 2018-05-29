void UART_Init(void);			         // 串口1初始化：9600 /22.1184MHZ
void UART_Send_Str(char *s);	         // 发送字符串
void UART_Send_Num(unsigned long dat);	 // 发送数值
void UART_Send_StrNum(char *inf,unsigned long dat);	  // 发送字符串+数值
void UART_Send_Hex(unsigned int hex);				  // 发送16进制(整数范围)
void UART_Send_binary(unsigned char dat);			  // 发送2进制
