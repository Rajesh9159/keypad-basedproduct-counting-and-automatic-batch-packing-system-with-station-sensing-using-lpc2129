                                                                                                                            #include <LPC21XX.H>
#include "lcd_header.h"
#include "keypad.h"


#define led1   (1<<18)
#define led2   (1<<19)

#define BUZZER (1<<20)

#define MOTOR1 (1<<16)
#define MOTOR2 (1<31)

#define IR2    (1<<18)

volatile int start_flag = 0;
volatile int product_count = 0;
volatile int ir1_flag = 0;

int res = 0;


void EXT_INT0_isr(void) __irq
{
    // Clear interrupt flag
    EXTINT = (1<<0);

    if(start_flag == 1)
    {
        ir1_flag = 1;
    }

    // End of interrupt
    VICVectAddr = 0;
}


void EXTINT0_INIT(void)
{
    // Configure P0.16 as EINT0
  
    PINSEL1 |= (1<<0);

    // Configure IRQ
    VICIntSelect =0X00;

    // Configure VIC slot 0
    VICVectCntl0 = (1<<5)|14;
    VICVectAddr0 = (int)EXT_INT0_isr;

    // Configure falling edge
    EXTMODE =0X01;
    EXTPOLAR =0X00;

    // Clear interrupt flag
   // EXTINT = (1<<0);

    // Enable EINT0
    VICIntEnable = (1<<14);
}


int GET_COUNT(void);

int main(void)
{
    int i;
    int displayed_count = -1;
	

    // Configure IR2 as input
    IODIR0 &= ~(IR2);

    // Configure motor, buzzer and LEDs as output
    IODIR1 |= MOTOR1|MOTOR2|BUZZER|led1|led2;

    IOSET1 = BUZZER;
		IOSET1 = led1|led2;
	 IOSET1 |=MOTOR1|MOTOR2;
    LCD_INIT();

	// Initialize external interrupt
    EXTINT0_INIT();

    LCD_COMMAND(0x80);
    LCD_STRING("SMART PACKING SYSTEM");

    delay_ms(1000);

    LCD_COMMAND(0x01);

    // Get required count
    res = GET_COUNT();

   
    // Wait for START key
    LCD_COMMAND(0x80);
    LCD_STRING("PRESS START");

    while(keyscan() != 16);

    product_count = 0;
    ir1_flag = 0;

    

    IOSET1 = MOTOR1;
    IOCLR1 = MOTOR2;
	delay_ms(500);
    IOCLR1 = led1;
	IOSET1 = led2;
	delay_ms(500);

    start_flag = 1;

    LCD_COMMAND(0x01);

    while(1)
    {
        // Display product count
        if(displayed_count != product_count)
        {
            displayed_count = product_count;

            LCD_COMMAND(0x80);
            LCD_STRING("PRODUCT COUNT:");

            LCD_COMMAND(0xC0);
            LCD_INTEGER(displayed_count);
        }

        // IR1 detected through interrupt
        if(ir1_flag == 1)
        {
            // Wait for IR2 detection
            i = 0;

            while((IOPIN0 & IR2) != 0)
            {
                delay_ms(2);
                i++;

                if(i >= 2000)
                {
                    start_flag = 0;

                    LCD_COMMAND(0x01);
                    LCD_COMMAND(0x80);
                    LCD_STRING("BELT FAULT");

                    IOCLR1 = MOTOR1|MOTOR2;
				           	delay_ms(500);                    
                  
                }
            }

            // Both sensors detected one product
            if(product_count < res)
            {
                product_count++;
            }

            // Wait until IR2 becomes inactive
            while((IOPIN0 & IR2) == 0);

            // Reset flag for next product
            ir1_flag = 0;
        }

        // Packing completed
        if(product_count >= res)
        {
            start_flag = 0;

            LCD_COMMAND(0x01);
            LCD_COMMAND(0x80);
            LCD_STRING("PACKING COMPLETED");

            IOCLR1 = BUZZER;

            IOCLR1 = MOTOR1|MOTOR2;
            
			IOSET1 = led1;
            IOCLR1 = led2;
			delay_ms(500);

            while(1);
        }
    }
}


// Get required product count
int GET_COUNT(void)
{
//    int n = 0;
    unsigned char key;

    LCD_COMMAND(0x80);
    LCD_STRING("ENTER COUNT:");

    LCD_COMMAND(0xC0);

   
        key = keyscan();
				LCD_INTEGER(key);
				delay_ms(500);
				LCD_COMMAND(0X01);

    
		return key;
}
