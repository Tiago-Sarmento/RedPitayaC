#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "test.mosquitto.org"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MakerIOTopicReceipt"
//#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int send_mqtt(char payload);
