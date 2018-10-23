/**
 * Telstra Arduino MKR NB1500 Cumulocity example
 * 
 * Takes temperature and humidity readings
 * from a DHT11 sensor, and pushes a
 * measurement message to Cumulocity.
 * 
 * This sketch requires the SimpleDHT library
 * https://github.com/winlinvip/SimpleDHT
 * 
 * Created 23 October 2018
 * By Brendan Myers
 * 
 * https://github.com/telstra/arduino-mkr-nb1500
 * 
 */
#include <SimpleDHT.h>

#define LOG true
#define l(x) if (LOG) Serial.println(x);
#define DELAY (15 * 1000)
#define TIMEOUT (5 * 1000)

unsigned long baud = 115200;

byte temperature;
byte humidity;

// Cumulocity tenancy config
String username = "";
String password = "";
String tenancy = "";
String deviceId = "";

const int PIN_DHT = 11;
SimpleDHT11 dht11(PIN_DHT);


/**
 * 
 */
void setup() {
  // reset the ublox module
  pinMode(SARA_RESETN, OUTPUT);
  digitalWrite(SARA_RESETN, HIGH);
  delay(100);
  digitalWrite(SARA_RESETN, LOW);

  Serial.begin(baud);
  SerialSARA.begin(baud);

  l("Initializing board");

  cmd("AT");

  cmd("AT+UHTTP=2,1,\"" + tenancy + ".iot.telstra.com\"");
  cmd("AT+UHTTP=2,5,443");
  cmd("AT+UHTTP=2,6,1"); // TLS, 0 == no, 1 == yes
  cmd("AT+UHTTP=2,4,1"); // Auth type, 1 == Basic
  cmd("AT+UHTTP=2,2,\"" + username + "\"");
  cmd("AT+UHTTP=2,3,\"" + password + "\"");
}


/**
 * 
 */
void loop() {
  // from SimpleDHT sample sketch
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    l("Read DHT11 failed, err=" + err);
    delay(DELAY);
    return;
  }
  
  String date = getDate();
  
  preparePayload(date, (int)temperature, (int)humidity);
  sendUpdate();
  
  delay(DELAY);
}


/**
 * Send AT command to u-blox module
 */
String cmd(String cmd) {
  l("cmd: " + cmd);
  
  SerialSARA.println(cmd);

  int timeout = TIMEOUT;
  while (!SerialSARA.available() & timeout > 0) {
    timeout--;
    delay(1);
  }

  if (timeout == 0 || timeout < 0) {
    l("Timed out");
    return "";
  }

  String result = ""; 
  
  do {
    result += char(SerialSARA.read());
    delay(5);
  } while(SerialSARA.available());

  l("rsp: " + result);

  return result;
}


/**
 * Get the network date, and return it 
 * formatted for Cumulocity
 */
String getDate() {
  String networkTime = cmd("at+cclk?");
  networkTime.trim();

  String date = "20";
  date += networkTime.substring(19,21); // year
  date += "-";
  date += networkTime.substring(22,24); // month
  date += "-";
  date += networkTime.substring(25,27); // day
  date += "T";
  date += networkTime.substring(28,30); // hour
  date += ":";
  date += networkTime.substring(31,33); // minute
  date += ":";
  date += networkTime.substring(34,36); // second
  date += ".000+00:00";
  
  l("Formatted date: " + date);

  return date;
}


/**
 * Prepare the Cumulocity payload, and save
 * it to the u-blox module
 */
void preparePayload(String date, int temperature, int humidity) {
  cmd("AT+UDELFILE=\"req\"");
  cmd("AT+UDELFILE=\"res\"");

  // format json payload
  String c = "";
  c += "{";
  c +=    "\"source\":{\"id\":\"";
  c +=    deviceId;
  c +=    "\"},";
  c +=    "\"time\": \"";
  c +=    date;
  c +=    "\",";
  c +=    "\"type\":\"arudino_measurement\",";
  c +=    "\"dht11\":{";
  c +=       "\"temperature\":{\"value\":";
  c +=       String(temperature);
  c +=       ",\"unit\":\"celcius\"},";
  c +=       "\"humidity\":{\"value\":";
  c +=       String(humidity);
  c +=       ",\"unit\":\"%\"}";
  c +=    "}";
  c += "}";

  String storePayload = "AT+UDWNFILE=\"req\",";
  storePayload += String(c.length());
  
  cmd(storePayload);
  cmd(c);
}


/**
 * Send the stored payload to Cumulocity
 */
void sendUpdate() {
  String c = "";
  c = "AT+UHTTPC=";
  c += "2,"; // profile_id
  c += "4,"; // http_command POST FILE
  c += "\"/measurement/measurements\","; // path
  c += "\"res\","; // filename
  c += "\"req\","; // param1
  c += "6,"; // param2 custom content type
  c += "\"application/vnd.com.nsn.cumulocity.measurement+json\""; // param3 content-type

  cmd(c);
}


