#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "icxparrot.isis.rl.ac.uk" //"test.mosquitto.org"   // choose a mqtt broker, such as broker.hive.com
#define CLIENTID    "ExampleClientPub" // unique identifier for the client, can be whatever you like
#define TOPIC       "/vista/values/failed_pulse_count"//"MakerIOTopicReceipt" // topic to send message to
//#define PAYLOAD     "Hello World!"
#define QOS         1 // quality of service - 0 means fire and forget - change to suit speed needs
#define TIMEOUT     10000L

void mqtt_connect(void);
void mqtt_disconnect(void);
int mqtt_send(char *payload);
