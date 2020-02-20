#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

void led_toggle(void);

Ticker time_up;
DigitalOut led1(LED1);
TS_StateTypeDef  TS_State = {0};

uint8_t counter = 0;
 
void led_toggle() 
{
    counter = 0;
}
 
int main()
{
    counter = 0;
    char snum[5];
    
    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }

    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 10);
    BSP_LCD_SetFont(&Font20);
    
    time_up.attach(&led_toggle, 5);
    while(true) 
    {

        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected ){
            if(TS_State.touchX[0] && TS_State.touchY[0]){
                counter += 1;
            }
        }

        printf("Counter : %d\n", counter/4);
        
        BSP_LCD_ClearStringLine(0);
        BSP_LCD_ClearStringLine(1);
        BSP_LCD_DisplayStringAtLine(0, (uint8_t *) "Counter : " );
        sprintf(snum,"%d",counter/4);
        BSP_LCD_DisplayStringAtLine(1, (uint8_t *) snum);
        
        
    }
}