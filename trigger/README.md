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
```
git clone https://github.com/RedPitaya/RedPitaya.git

cd RedPitaya

make api

cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg

LD_LIBRARY_PATH=/opt/redpitaya/lib
```

install paho-eclipse-mqtt (verify installation by running paho_c_pub, if it recognises then installation successful) https://github.com/eclipse/paho.mqtt.c

```
cd ~

git clone https://github.com/eclipse/paho.mqtt.c

cd paho.mqtt.c

make

sudo make install
```
install this repository and run code

```
git clone https://github.com/Tiago-Sarmento/RedPitayaC

cd RedPitayaC/trigger

gcc -o executable trigger.c fpga_osc.c mqtt_test.c -lm -lpaho-mqtt3c

./executable
```

## Basic Usage

The failed pulse counter should start running on boot.

To manually run the failed pulse counter as it is, ssh into the red pitaya, navigate to the 'trigger' directory and type ./trigger into the terminal.
Verify by looking at the channel defined in trigger.c

To compile:
gcc -o executable trigger.c fpga_osc.c mqtt_test.c -lm -lpaho-mqtt3c

## Adaptation
The key script defining the failed pulse counter is trigger.c

fpga_osc files handle fpga control. Functions used in trigger.c are defined here

mqtt_test files handle MQTT messaging. Also defines functions. Requires paho.mqtt to be installed to work.

If a file is modified it will need recompiling


## Troubleshooting
'bus error' indicates the FPGA needs loading. Type cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg

```
gcc -lpaho-mqtt3c -lm
markdown code block
```

Edit the MQTT channel, message in 

Edit frequency of messages, thresholds defining pulse failure, and anything else in trigger.c




  
