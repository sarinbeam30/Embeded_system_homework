#include "mbed.h"
#include <stdlib.h>
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#define PressButton 1

Semaphore one_slot(1);

Mutex stdio_mutex;
Thread thread_A, thread_B, thread_C;
DigitalIn  User_Button(PA_0,PullNone);

//DISPLAY
TS_StateTypeDef  TS_State = {0};  

int temp , humidity, light;
char string_temp[5], string_humidity[5], string_light[5];
int counter = 0;

void screen_setup(uint16_t text, uint16_t background){
    BSP_LCD_Clear(background);
    BSP_LCD_SetTextColor(text);
    BSP_LCD_SetBackColor(background);
    BSP_LCD_SetFont(&Font20);
}

void generate_temp(){
    temp = rand() % (100 + 1 - 0) + 0;
    sprintf(string_temp, "%d", temp);
    screen_setup(LCD_COLOR_BLACK, LCD_COLOR_GREEN);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Temperature : ", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 165, (uint8_t *) string_temp, CENTER_MODE);
}

void generate_humidity(){
    humidity = rand() % (1000 + 1 - 100) + 100;
    sprintf(string_humidity, "%d", humidity);
    screen_setup(LCD_COLOR_BLACK, LCD_COLOR_MAGENTA);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Humidity : ", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 165, (uint8_t *) string_humidity, CENTER_MODE);
}

void generate_light(){
    light = rand() % (10000 + 1 - 1000) + 1000;
    sprintf(string_light, "%d", light);
    screen_setup(LCD_COLOR_BLACK, LCD_COLOR_ORANGE);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Light : ", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 165, (uint8_t *) string_light, CENTER_MODE);
}

void generate_three_bottom(){
    temp = rand() % (100 + 1 - 0) + 0;
    humidity = rand() % (1000 + 1 - 100) + 100;
    light = rand() % (10000 + 1 - 1000) + 1000;

    sprintf(string_temp, "%d", temp);
    sprintf(string_humidity, "%d", humidity);
    sprintf(string_light, "%d", light);

    BSP_LCD_SetFont(&Font12);

    BSP_LCD_DrawRect(0 ,205, 50, 35);
    BSP_LCD_DrawRect(75, 205, 85, 35);
    BSP_LCD_DrawRect(180, 205, 60, 35);

    BSP_LCD_DisplayStringAt(0, 210, (uint8_t *) "Temp : " , LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 225, (uint8_t *) string_temp, LEFT_MODE);

    BSP_LCD_DisplayStringAt(0, 210, (uint8_t *) "Humidity : " , CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 225, (uint8_t *) string_humidity, CENTER_MODE);

    BSP_LCD_DisplayStringAt(0, 210, (uint8_t *) "Light : " , RIGHT_MODE);
    BSP_LCD_DisplayStringAt(0, 225, (uint8_t *) string_light, RIGHT_MODE);
}


void notify(const char* args) 
{
    
    one_slot.wait();
    wait(1);
    generate_three_bottom();

    if(User_Button.read() == PressButton && counter == 0){
        generate_temp();
        printf("KAO LOOP LEAW_1\n");
        counter += 1;

    }
    else if(User_Button.read() == PressButton && counter == 1){
        generate_humidity();
        printf("KAO LOOP LEAW_2\n");
        counter += 1;
    }

    else if(User_Button.read() == PressButton && counter == 2){
        generate_light();
        printf("KAO LOOP LEAW_3\n");
        counter -= 2;
    }

    // stdio_mutex.unlock();
    one_slot.release();
    thread_sleep_for(750);
}

void test_thread(void const *args)
{
    while (true) 
    { 
        printf("KAO MA YOUNG\n");
        notify((const char*)args); 
        thread_sleep_for(750);
    }
}

int main() 
{
    BSP_LCD_Init();
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    thread_A.start(callback(test_thread, (void *)"temperature"));
    thread_B.start(callback(test_thread, (void *)"humidity"));
    thread_C.start(callback(test_thread, (void *)"lightIntensity"));  
}