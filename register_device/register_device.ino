/**
 * Telstra Arduino MKR NB1500 Cumulocity example
 * 
 * Registers a device on Cumulocity
 * 
 * Created 23 October 2018
 * By Brendan Myers
 * 
 * https://github.com/telstra/arduino-mkr-nb1500
 * 
 */
#define LOG true
#define l(x) if (LOG) Serial.println(x);
#define DELAY (15 * 1000)
#define TIMEOUT (5 * 1000)

unsigned long baud = 115200;

// Cumulocity tenancy config
String username = "";
String password = "";
String tenancy = "";
String deviceName = "";

String deviceId = "";

/**
 * TODO check that the HTTP result is 201
 * TODO error properly!
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

  // TODO store deviceId on device
  
  cmd("AT+UDELFILE=\"req\"");
  cmd("AT+UDELFILE=\"res\"");
  
  String data = "";
  data += "{";
  data += "    \"name\": \"";
  data +=       deviceName;
  data +=       "\",";
  data += "    \"type\": \"arduino_mkrnb1500\",";
  data += "    \"c8y_IsDevice\": {},";
  data += "    \"com_cumulocity_model_Agent\": {},";
  data += "    \"c8y_SupportedOperations\": [],";
  data += "    \"c8y_Hardware\": {},";
  data += "    \"c8y_Configuration\": {},";
  data += "    \"c8y_Mobile\": {},";
  data += "    \"c8y_Firmware\": {},";
  data += "    \"c8y_Software\": {}";
  data += "}";

  String storePayload = "AT+UDWNFILE=\"req\",";
  storePayload += String(data.length());
  cmd(storePayload);
  cmd(data);

  String c = "";
  c += "AT+UHTTPC=";
  c += "2,"; // profile_id
  c += "4"; // http_command POST DATA
  c += ",\"/inventory/managedObjects\","; // path
  c += "\"res\","; // filename
  c += "\"req\","; // param1
  c += "6,";
  c += "\"application/vnd.com.nsn.cumulocity.managedObject+json\""; // param3 content-type

  cmd(c);

  String res = cmd("at+urdfile=\"res\"");;
  int timeout = 10;

  while (res.length() < 50 && timeout > 0) {
    timeout--;
    delay(1000);
    res = cmd("at+urdfile=\"res\"");
  }

  if (timeout == 0 || timeout < 0) {
    l("Timed out");
  }

  deviceId = res.substring(261,270);
}


/**
 * 
 */
void loop() {
  l("Device id: \"" + deviceId + "\"");

  delay(2500);
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


