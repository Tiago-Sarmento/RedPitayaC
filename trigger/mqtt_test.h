#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "icxparrot.isis.rl.ac.uk" //"test.mosquitto.org"   // choose a mqtt broker, such as broker.hive.com
#define CLIENTID    "ExampleClientPub" // unique identifier for the client, can be whatever you like
#define TOPIC_A1    "vista/values/vespa/failed_acc_A" // accumulated count of failed pulses in 1 sec
#define TOPIC_A2    "vista/values/vespa/failed_ind_A" // individual failed pulses
#define TOPIC_B1    "vista/values/vespa/failed_acc_B" // same for second channel
#define TOPIC_B2    "vista/values/vespa/failed_ind_B" // 
#define TOPIC_Ext1    "vista/values/vespa/failed_acc_Ext" // same for second channel
#define TOPIC_Ext2    "vista/values/vespa/failed_ind_Ext" // 
//#define PAYLOAD     "Hello World!"
#define QOS         1 // quality of service - 0 means fire and forget - change to suit speed needs
#define TIMEOUT     10000L

void mqtt_connect(void);
void mqtt_disconnect(void);
int mqtt_sendAccA(char *payload);
int mqtt_sendIndA(char *payload);
int mqtt_sendAccB(char *payload);
int mqtt_sendIndB(char *payload);
int mqtt_sendAccExt(char *payload);
int mqtt_sendIndExt(char *payload);
