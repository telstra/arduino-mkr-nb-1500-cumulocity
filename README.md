# Telstra Arduino MKR NB 1500 Examples

These sketches demonstrate how to;

- Register an MKR NB 1500 on Cumulocity
- Send measurements from an MKR NB 1500 to Cumulocity


## Prerequisites

1. A Telstra Arduino MKR NB 1500 board.
1. A Telstra SIM card with an active subscription.
1. A Cumulocity account.
1. A user with permissions to register devices.
1. (Optional) A DHT11 temperature and humidity sensor.
1. (Optional) The [SimpleDHT library](https://github.com/winlinvip/SimpleDHT).


## How to run

### Register the MKR NB 1500

Open `register_device`. Enter your Cumulocity tenancy and user information on lines 20 - 22. 

```
// Cumulocity tenancy config
String username = "";
String password = "";
String tenancy = "";
```

On line 23, give your device a name (eg _MyArduino_).
```
String deviceName = "";
```

Upload and run the sketch. Once successfully registered, the serial monitor will continuously print the `deviceId`; note this down.


### Send measurements to Cumulocity

Open `send_measurements`. Enter your Cumulocity tenancy, user information, and `deviceId` on lines 30 - 33.

```
// Cumulocity tenancy config
String username = "";
String password = "";
String tenancy = "";
String deviceId = "";
```

Connect your DHT to GND, 5V, and pin 11.

Upload and run the sketch. You will now see measurements in Cumulocity.