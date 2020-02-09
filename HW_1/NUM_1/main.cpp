#include "mbed.h"

#define PressButton 1

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

DigitalIn  User_Button(PA_0,PullNone);
Serial UART(SERIAL_TX, SERIAL_RX);


int main()
{
    bool check_state = false;
    int count_press = 0;
    UART.baud(115200);
    while (true)
    {
        if(User_Button.read() == PressButton && check_state == false) // pressed button
        {
            count_press += 1;
            thread_sleep_for(3000);
            if(count_press >= 3){
                led1.write(1);
                check_state = true;
            }

            thread_sleep_for(1000);
        }

        else if(User_Button.read() == PressButton && check_state == true)
        {
            printf("CASE_2");
            led1.write(0);
            check_state = false;
            thread_sleep_for(1000);
        }
    
            
    }
}
