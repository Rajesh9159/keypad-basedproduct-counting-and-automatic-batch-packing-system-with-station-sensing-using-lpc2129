#include<LPC21XX.H>
#define lcd_d (0xf)<<22
#define RS 1<<12
#define E 1<<13

void LCD_DATA( unsigned char);
void LCD_COMMAND(unsigned char);
void LCD_INIT(void);
void LCD_STRING(unsigned char*);
void LCD_INTEGER(int);
void delay_ms(unsigned int);


void LCD_INIT(void)
{
IODIR0=lcd_d|RS|E;
LCD_COMMAND(0X01);
LCD_COMMAND(0X02);
LCD_COMMAND(0X0C);
LCD_COMMAND(0X28);
}


void LCD_COMMAND(unsigned char cmd)
{
//upper nibble
IOCLR0=lcd_d;
IOSET0=(cmd & 0xf0)<<18;
IOCLR0=RS;
IOSET0=E;
delay_ms(5);
IOCLR0=E;

//lower nibble
IOCLR0=lcd_d;
IOSET0=(cmd & 0x0f)<<22;
IOCLR0=RS;
IOSET0=E;
delay_ms(5);
IOCLR0=E;
}


void LCD_DATA(unsigned char d)
{
//upper nibble
 IOCLR0=lcd_d;
 IOSET0=(d & 0xf0)<<18;
 IOSET0=RS;
 IOSET0=E;
 delay_ms(5);
 IOCLR0=E;

 //lower nibble
 IOCLR0=lcd_d;
 IOSET0=(d & 0x0f)<<22;
 IOSET0=RS;
 IOSET0=E;
 delay_ms(5);
 IOCLR0=E;
}


void LCD_STRING(unsigned char *s)
{
	int count=1;
	// int i=0;
	while(*s)
	{	
		count++;
		if(count==16)
			LCD_COMMAND(0XC0);

		LCD_DATA(*s++);
	}
}

void delay_ms(unsigned int ms)
{
	T0PR=15000-1;
	T0TCR=0X01;
	while(T0TC<ms);
	T0TCR|=(1<<1);
	T0TCR=0X00;
}

void LCD_INTEGER(int n)
{
	signed char i=0;
	unsigned char arr[5];
	if(n==0)
	LCD_DATA('0');
	else
	{
		if(n < 0)
		{
			LCD_DATA('-');
			n=-n;
		}
		while(n>0)
		{
			arr[i++]=n%10;
			n=n/10;
		}
		for(i=i-1;i>=0;i--)
		LCD_DATA(arr[i]+48);
	}
}
		

	


