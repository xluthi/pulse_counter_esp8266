# Pulse counter with ESP8266 #

This simple sketch detects magnetic pulse thanks to a reed switch, and sends it immediately to a [Domoticz](http://domoticz.com/) instance AND an MQTT broker.

It is used with a Kent V100 (PSM) water meter, which is heavily used in Belgium.  It generates one pulse per 0.5 liter.

The hardware schematic is quite simple:

![Schematic](schematic/pulse_counter_ESP8266_schematic.png)

![Breadboard](schematic/pulse_counter_ESP8266_bb.png)


Digital input D1 is set to ground thanks to a pull down resistor R2.  When the reed switch is closed, the input is raised to VCC and an interrupt is detected.  The LED1 is flashing to provide a visual control when a pulse is detected.

## MQTT ##
Initially, the sketch was foreseen to Domoticz only.  But MQTT has been added in order to use Grafana & InfluxDB as database and visualization tool, allowing greater flexibility in the aggregation and custom made dashboards.


## Compilation ##

Before compiling, create a `private.h` file based on the provided `private_example.h` file: it should contain your Wifi settings, as well as the Domoticz parameters.  The `domoticz_counter_idx` parameter refers to the ID of the "incremental counter" created in Domoticz.

The code contains a lot of debug statements. If those are not required, comment the `#define DEBUG` at the beginning of the main file.
