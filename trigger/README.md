# Red Pitaya based failed pulse counter with MQTT

A folder adapted from https://github.com/Grozomah/trigger to quickly read analogue input to Red Pitaya and send MQTT messages.
Requires installation of paho-eclipse-mqtt https://github.com/eclipse/paho.mqtt.c 

Will read signals from both input channels, count the number of points above a defined value, and determine if a pulse is present. 
Every 2 seconds a MQTT message is sent counting the number of failed pulses.
Thresholds and scope behaviour can be modified in trigger.c
Succesfully runs at 50Hz.

## Installation

ssh into Red Pitaya https://redpitaya.readthedocs.io/en/latest/developerGuide/os/ssh/ssh.html

install Red Pitaya content https://github.com/RedPitaya/RedPitaya/blob/master/Examples/C/README.md
in short: 
git clone https://github.com/RedPitaya/RedPitaya.git
cd RedPitaya
make api
cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg

install paho-eclipse-mqtt (verify installation by running paho_c_pub, if it recognises then installation successful) https://github.com/eclipse/paho.mqtt.c

clone this directory anywhere under ~ folder 

## Basic Usage

The failed pulse counter should start running on boot.

To manually run the failed pulse counter as it is, ssh into the red pitaya, navigate to the 'trigger' directory and type ./trigger into the terminal.
Verify by looking at the channel defined in trigger.c

## Adaptation
The key script defining the failed pulse counter is trigger.c

fpga_osc files handle fpga control. Functions used in trigger.c are defined here

mqtt_test files handle MQTT messaging. Also defines functions. Requires paho.mqtt to be installed to work.

If a file is modified it will need recompiling

```
gcc -lpaho-mqtt3c -lm
markdown code block
```

Edit the MQTT channel, message in 

Edit frequency of messages, thresholds defining pulse failure, and anything else in trigger.c




  
