# Red Pitaya based failed pulse counter with MQTT

A folder adapted from https://github.com/Grozomah/trigger to quickly read analogue input to Red Pitaya and send MQTT messages.
Requires installation of paho-eclipse-mqtt https://github.com/eclipse/paho.mqtt.c 

Will read signals from both input channels, count the number of points above a defined value, and determine if a pulse is present. 
Every 2 seconds a MQTT message is sent counting the number of failed pulses.
Thresholds and scope behaviour can be modified in trigger.c
Succesfully runs at 50Hz.

## Installation

ssh into Red Pitaya https://redpitaya.readthedocs.io/en/latest/developerGuide/os/ssh/ssh.html
install paho-eclipse-mqtt (verify installation by running paho_c_pub, if it recognises then installation successful)
clone anywhere under ~ folder 

## Usage 

```
gcc -lpaho-mqtt3c -lm
markdown code block
```
  
