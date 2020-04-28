#include "mbed.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "string"

Thread thread;
WiFiInterface *wifi;
TS_StateTypeDef  TS_State = {0};
Serial UART(SERIAL_TX, SERIAL_RX);

int arrivedcount = 0;
int num_count = 0;
int i=0;
char *data[5];


void get_the_payload(char payload[]){
    char * token = strtok(payload, " ");

    while(token)
    {
        data[num_count++] = token;
        token = strtok(NULL, " ");
    }

    for(i=0; i<5; i++)
    {
        printf("data[%i] = %s\n", i, data[i]);
    }
    
    //DISPLAY_SETTING
    //CLEAR
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_ClearStringLine(4);
    BSP_LCD_ClearStringLine(7);
    BSP_LCD_ClearStringLine(10);

    BSP_LCD_SetFont(&Font12);
    BSP_LCD_ClearStringLine(18);
    BSP_LCD_ClearStringLine(19);

    //SHOW
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_SetFont(&Font16);

    BSP_LCD_DisplayStringAtLine(3,(uint8_t*) "Confirmed : ");
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_DisplayStringAtLine(4,(uint8_t*) data[0]);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);


    BSP_LCD_DisplayStringAtLine(6,(uint8_t*) "Recovered : ");
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_DisplayStringAtLine(7,(uint8_t*) data[1]);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);


    BSP_LCD_DisplayStringAtLine(9,(uint8_t*) "Death : ");
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAtLine(10,(uint8_t*) data[2]);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

    BSP_LCD_DisplayStringAtLine(12,(uint8_t*) "Time : ");
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAtLine(18,(uint8_t*) data[3]);
    BSP_LCD_DisplayStringAtLine(19,(uint8_t*) data[4]);

    
}

void subscribeCallback(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    // printf("Payload %.*s\n", message.payloadlen, (char*)message.payload);
    get_the_payload((char*)message.payload);
    arrivedcount++;
}

void touchscreen_display(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    /* Set Touchscreen Demo1 description */
    // BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize()); 
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK); 
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"COVID_19 - Report", CENTER_MODE);
    BSP_LCD_DrawHLine(0, 33, BSP_LCD_GetXSize());
    
    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);
    
}

int main()
{
    //DISPLAY_SETTING
    UART.baud(115200);
    BSP_LCD_Init();

    //DISPLAY_SHOW
    //THREAD
    thread.start(touchscreen_display);

    char* topic = "/Covid_19";

    printf("WiFi MQTT example\n");

    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) 
    {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) 
    {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi->get_mac_address());
    printf("IP: %s\n", wifi->get_ip_address());
    printf("Netmask: %s\n", wifi->get_netmask());
    printf("Gateway: %s\n", wifi->get_gateway());
    printf("RSSI: %d\n\n", wifi->get_rssi());

    MQTTNetwork mqttNetwork(wifi);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.1.101";
    int port = 1883;
    printf("[Subscribe] Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("[Subscribe] rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-subscribe";
    data.username.cstring = "";
    data.password.cstring = "";


    if ((rc = client.connect(data)) != 0)
    {
        printf("[Subscribe]rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("[Subscribe] Client Connected.\r\n");
    }

    if ((rc = client.subscribe(topic, MQTT::QOS0, subscribeCallback)) != 0)
    {
        printf("[Subscribe] rc from MQTT subscribe is %d\r\n", rc);
    }
    else
    {
        printf("[Subscribe] Client subscribed.\r\n");
    }
        

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    int test_subscribe = 0;
    while(true){
        client.yield(10000);
        // printf("Test subscribe :: %d\n", test_subscribe);
        test_subscribe += 1; 
    }
    
    printf("[Subscribe] Finishing with %d messages received\n", arrivedcount);
    
    mqttNetwork.disconnect();

    if ((rc = client.disconnect()) != 0)
    {
        printf("[Subscribe] rc from disconnect was %d\r\n", rc);
        printf("[Subscribe] Client Disconnected.\r\n");
    }

    wifi->disconnect();

    printf("\n[Subscribe] Done\n");
}