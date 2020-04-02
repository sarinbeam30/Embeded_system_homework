#include "mbed.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include <stdlib.h>
#include <time.h>
#define randnum(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

WiFiInterface *wifi;

AnalogIn RandomIn (PA_4);
void setup(void)
{
    // perform initialisations
 
    // create a 32 bit number out of 32 LSBs from the ADC
    uint32_t seedValue = 0;
    uint16_t value;
    uint8_t counter;
 
    for (counter = 0; counter < 32; counter++) {
        seedValue = seedValue<<1;
        value = RandomIn.read_u16(); // reads a 10 bit ADC normalised to 16 bits.
        if (value & 0x0040)          // LSB of ADC output = 1
            seedValue++;
    }
 
    srand(seedValue);     // seed the random generator with the background noise of an analog input
}

int main()
{
    setup();
    int count = 0;
    char* topic = "/TempHumid-json";

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

    const char* hostname = "192.168.43.248";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sample";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("Client Connected.\r\n");
    }
        
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    while(count < 150) // client.isConnected()
    {
        srand(time(NULL));
        int temp = randnum(1, 50);
        int humidity = randnum(1, 100);
        
        // QoS 0
        // char buf[100];
        // sprintf(buf, "{\"Temp\" : \"%d\" , \"Humidity\" : \"%d\"}\n", count, count+5);
        // char * buf = "{\"Temp\": temp , \"Humi\": }";
        char buf[100];
        sprintf(buf, "{\"Temp\": %d, \"Humi\": %d}", temp, humidity);
        // sprintf(buf, "20,50");
        printf("Clien sent->%s\n",buf);
        message.payload = buf;
        message.payloadlen = strlen(buf);
        rc = client.publish(topic, message);
        count++;
        //Wait for 5 seconds.
        wait(5);
    }
    
    mqttNetwork.disconnect();

    if ((rc = client.disconnect()) != 0)
    {
        printf("rc from disconnect was %d\r\n", rc);
        printf("Client Disconnected.\r\n");
    }

    wifi->disconnect();

    printf("\nDone\n");
}