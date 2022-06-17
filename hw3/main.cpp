#include "mbed.h"
#include "drivers/DigitalOut.h"

#include "erpc_simple_server.h"
#include "erpc_basic_codec.h"
#include "erpc_crc16.h"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "blink_led_server.h"

#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"

#include "stm32l475e_iot01_accelero.h"

/**
 * Macros for setting console flow control.
 */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)



int flag = -1;

WiFiInterface *wifi;
NetworkInterface* net = wifi;
MQTTNetwork mqttNetwork(net);
MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

volatile bool closed = false;

const char* topic = "Mbed";

Thread mqtt_thread(osPriorityHigh);
EventQueue mqtt_queue;

int16_t pDataXYZ[3] = {0};


void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) {
    MQTT::Message message;
    char buff[100];
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    sprintf(buff, "%d, %d, %d\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
    printf("%d, %d, %d\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*) buff;
    message.payloadlen = strlen(buff) + 1;
    int rc = client->publish(topic, message);

    printf("rc:  %d\r\n", rc);
    printf("Puslish message: %s\r\n", buff);
}


void led_on(uint8_t led) {
    flag = 1;
}

void led_off(uint8_t led) {
    flag = 0;
}

/** erpc infrastructure */
ep::UARTTransport uart_transport(D1, D0, 9600);
ep::DynamicMessageBufferFactory dynamic_mbf;
erpc::BasicCodecFactory basic_cf;
erpc::Crc16 crc16;
erpc::SimpleServer rpc_server;

/** LED service */
LEDBlinkService_service led_service;


void rpc_service(){
    uart_transport.setCrc16(&crc16);
    // Set up hardware flow control, if needed
    #if CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
        uart_transport.set_flow_control(mbed::SerialBase::RTS, STDIO_UART_RTS, NC);
    #elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
        uart_transport.set_flow_control(mbed::SerialBase::CTS, NC, STDIO_UART_CTS);
    #elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
        uart_transport.set_flow_control(mbed::SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
    #endif

    printf("Initializing server.\n");
    rpc_server.setTransport(&uart_transport);
    rpc_server.setCodecFactory(&basic_cf);
    rpc_server.setMessageBufferFactory(&dynamic_mbf);

    printf("Adding LED server.\n");
    rpc_server.addService(&led_service);

    printf("Running server.\n");
    rpc_server.run();

}

int main(void) {


  // Initialize the rpc server
    
    BSP_ACCELERO_Init();
    

    Thread erpc_thread;

    //mqtt
    
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
            printf("ERROR: No WiFiInterface found.\r\n");
            return -1;
    }

    printf("\nConnecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
            printf("\nConnection error: %d\r\n", ret);
            return -1;
    }

    NetworkInterface* net = wifi;
    MQTTNetwork mqttNetwork(net);
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* host = "192.168.28.185";
    const int port=1883;
    printf("Connecting to TCP network...\r\n");
    printf("address is %s/%d\r\n", host, port);

    int rc = mqttNetwork.connect(host, port);//(host, 1883);
    if (rc != 0) {
            printf("Connection error.");
            return -1;
    }
    printf("Successfully connected!\r\n");

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    
    data.MQTTVersion = 3;
    data.clientID.cstring = "Mbed";

    if ((rc = client.connect(data)) != 0){
            printf("Fail to connect MQTT\r\n");
    }

    mqtt_thread.start(callback(&mqtt_queue, &EventQueue::dispatch_forever));
    mqtt_queue.event(&publish_message, &client);
    
    erpc_thread.start(rpc_service);
    
    printf("Ready to Start\n");

    while(true){
        if(flag == 1){
            client.yield(500);
            mqtt_queue.event(&publish_message, &client);
        }
        ThisThread::sleep_for(100ms);
        printf("%d\n",flag);
        if(flag == 0)
            break;
    }
    
    return 0;
}