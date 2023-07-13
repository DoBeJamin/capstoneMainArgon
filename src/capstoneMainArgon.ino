
#include "env.h"

#include "Particle.h"
#include "MQTT.h"
#include "oled-wing-adafruit.h"
#include "blynk.h"

SYSTEM_THREAD(ENABLED);

//MQTT
void callback(char *topic, byte *payload, unsigned int length) ;
MQTT client("lab.thewcl.com", 1883, callback);
//TOPICS
String A1_RSSI = "BAR/argon1/RSSI";
String A2_RSSI = "BAR/argon2/RSSI";
String A3_RSSI = "BAR/argon3/RSSI";
//Data
int Argon1_distance = 0;
int Argon2_distance = 0;
int Argon3_distance = 0;

//OLED
OledWingAdafruit display;

//BLE
SerialLogHandler logHandler;
BleUuid serviceUuid("afe7acc5-33a9-478f-bbe1-8944aa08e884");

unsigned long lastScan;
BleAddress peripheralAddr;
int rssi;
String string_rssi;

void scanResultCallback(const BleScanResult *scanResult, void *context);

void setup() {

  //OLED
  display.setup();

  //BLE
	BLE.on();
	BLE.setAdvertisingInterval(20);

  //MQTT
  client.connect(System.deviceID());
  client.subscribe(A1_RSSI);
  client.subscribe(A2_RSSI);
  client.subscribe(A3_RSSI);
  
  //BLYNK
  Blynk.begin(BLYNK_AUTH_TOKEN);
}

void loop() {

  display.loop();
  Blynk.run();

  //display
  format_display();

  display.print("A1 : ");
  if (!Argon1_distance) {
    display.println("No data");
  } else {
    display.println(Argon1_distance);
  }
  display.print("A2 : ");
  if (!Argon2_distance) {
    display.println("No data");
  } else {
    display.println(Argon2_distance);
  }
  display.print("A3 : ");
  if (!Argon3_distance) {
    display.println("No data");
  } else {
    display.println(Argon3_distance);
  }

  display.display();

  //BLE rssi
	rssi = 0;
	BLE.scan(scanResultCallback, NULL);

	if (rssi) {

  //MQTT
		if (client.isConnected()) {
			string_rssi = String(rssi);
			client.publish("BAR/argon3/RSSI", string_rssi);
		} 

  }
  //MQTT
	if (client.isConnected()) {
		client.loop();
	} else {
		client.connect(System.deviceID());
    client.subscribe(A1_RSSI);
    client.subscribe(A2_RSSI);
    client.subscribe(A3_RSSI);
	}

  //BLYNK
  Blynk.virtualWrite(V0, Argon1_distance);
  Blynk.virtualWrite(V1, Argon2_distance);
  Blynk.virtualWrite(V2, Argon3_distance);
	
}


void scanResultCallback(const BleScanResult *scanResult, void *context) {

	BleUuid foundServiceUuid;
	size_t svcCount = scanResult->advertisingData().serviceUUID(&foundServiceUuid, 1);


	Log.info("rssi=%d server=%02X:%02X:%02X:%02X:%02X:%02X",
			scanResult->rssi(),
			scanResult->address()[0], scanResult->address()[1], scanResult->address()[2],
			scanResult->address()[3], scanResult->address()[4], scanResult->address()[5]);

	peripheralAddr = scanResult->address();
	rssi = scanResult->rssi();
	
	BLE.stopScanning();
}

void format_display() {
  //sets up basic formatting for when using the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
}

void callback(char *topic, byte *payload, unsigned int length) {

  //formats and extracts data from mqtt 
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String s = p;
  int value = s.toFloat();

  String callbackTopic = topic; //makes the mqtt topic of the data into an arduino string

  if (callbackTopic.compareTo(A1_RSSI)) {
    Argon1_distance = value;
  } else if (callbackTopic.compareTo(A2_RSSI)) {
    Argon2_distance = value;
  } else if (callbackTopic.compareTo(A3_RSSI)) {
    Argon3_distance = value;
  }

}