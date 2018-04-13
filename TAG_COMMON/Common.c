#include "Common.h"
#include "Type.h"


void minidelay(uint8_t cc)//延时cc us，最小值2us
{
    uint8_t i;
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	    
    asm("nop");	
    asm("nop");	
    asm("nop");	
    asm("nop");	
	for ( i = 2; i < cc; i++)
    {
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");	
        asm("nop");
        asm("nop");
        asm("nop");
    }

}
//延时约nn mS
void delay(U32 nn)
{   
	U32 i;
	int j=0;
	for(i=0; i<nn; i++ )
	{
		for (j=0; j<409; j++)//352
		{
			minidelay(2);
		}
	}
}
char *itoa(int num, char *str, int radix)   
{
    char  string[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char* ptr = str;
    int denom = 0;  //余数
    int count = -1;
    int i;
    int j;

    while (num >= radix)
    {
        denom   = num % radix;
        num    /= radix;

        *ptr++  = string[denom];
        count++;
    }

    if (num)
    {
        *ptr++ = string[num];
        count++;
    }
    *ptr = '\0';
    j    = count;

    for (i = 0; i < (count + 1) / 2; i++)
    {
        int temp = str[i];
        str[i]   = str[j];
        str[j--] = temp;
    }

    return str;
} 
