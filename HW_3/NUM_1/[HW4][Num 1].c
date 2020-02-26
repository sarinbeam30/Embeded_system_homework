#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
 
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
Thread thread1, thread2, thread3;

TS_StateTypeDef  TS_State = {0};
void led2_thread() 
{
    while (true) 
    {
        led2 = !led2;
        wait(0.3);
    }
}

void led3_thread()
{
    while(true)
    {
        led3 = !led3;
        wait(0.4);
    }
}

void show_display()
{
    BSP_LCD_Init();
     /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    while(true)
    {
        thread_sleep_for(2000);
        BSP_LCD_Clear(LCD_COLOR_MAGENTA);
        thread_sleep_for(2000);
        BSP_LCD_Clear(LCD_COLOR_YELLOW);
    }
}
 
int main()
{
    thread1.start(led2_thread);
    thread2.start(led3_thread);
    thread3.start(show_display);
    
    while (true) 
    {
        led1 = !led1;
        wait(0.2);
    }
}