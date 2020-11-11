#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "icxparrot.isis.rl.ac.uk" //"test.mosquitto.org"   // choose a mqtt broker, such as broker.hive.com
#define CLIENTID    "ExampleClientPub" // unique identifier for the client, can be whatever you like
#define TOPIC_A       "/vista/values/failed_pulse_count"//"MakerIOTopicReceipt" // topic to send message to
#define TOPIC_B       "/vista/values/failed_pulse_count2"//"MakerIOTopicReceipt" // topic to send messag
//#define PAYLOAD     "Hello World!"
#define QOS         1 // quality of service - 0 means fire and forget - change to suit speed needs
#define TIMEOUT     10000L

void mqtt_connect(void);
void mqtt_disconnect(void);
int mqtt_sendA(char *payload);
int mqtt_sendB(char *payload);
