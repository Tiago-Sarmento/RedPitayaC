// code adapted from example given by paho in https://www.eclipse.org/paho/clients/c/
// uses paho mqtt c library

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <MQTTClient.h>

#include "mqtt_test.h"

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;
int rc;

void mqtt_connect(void)
{
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    else { printf("connected to MQTT");}
    MQTTClient_setCallbacks(NULL,NULL,NULL,NULL,NULL); // apparently calling this sets MQTT to run asynchronously
}

void mqtt_disconnect(void)
{
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    printf("Disconnected MQTT");   
}


int mqtt_send(char *message)
{
    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = 0; // lowest quality of service ensures high speed messaging (0 - 'fire and forget') - set to 1 or 2 for higher assurangce
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    //printf("Waiting for up to %d seconds for publication of %s\n"
    //        "on topic %s for client with ClientID: %s\n",
    //        (int)(TIMEOUT/1000), payload, TOPIC, CLIENTID);
    //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    //printf("Message with delivery token %d delivered\n", token);
    return rc;
}
