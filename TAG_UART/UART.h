#ifndef UART_H
#define UART_H

#define UART_NB   USART3  
#define USART_FOR_NB	USART3
#define USART_FOR_BT	USART1
#define UART_DEBUG      USART2

void UART_disable(void);
void UART_enable(void);

void sendUsartData(USART_TypeDef* USARTx,char *pSendBuf, uint16_t len);
void enableUsartBT(void);
void disableUsartBT(void);
void enableUsartNBIot(void);
void disableUsartNBIot(void);
void init_UART(void);

void MyUsart_InitforBT(void);

//for print function
void initUsart2();
void print(char* fmt, ...);
void printch(char ch);
void printdec(int dec);
void printflt(double flt);
void printstr(char* str);
void printbin(int bin);
void printhex(int hex);

#endif