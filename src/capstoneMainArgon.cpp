/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/benja/OneDrive/Desktop/visualStudioCode/summerIOT/capstoneMainArgon/src/capstoneMainArgon.ino"
void setup();
void loop();
void format_display ();
#line 1 "c:/Users/benja/OneDrive/Desktop/visualStudioCode/summerIOT/capstoneMainArgon/src/capstoneMainArgon.ino"
SYSTEM_THREAD(ENABLED);

#include "MQTT.h"
#include "oled-wing-adafruit.h"

void callback(char *topic, byte *payload, unsigned int length);
MQTT client("lab.thewcl.com", 1883, callback);

//MQTT Topics
String argon2 = "BAR/argon2/RSSI";
String argon3 = "BAR/argon3/RSSI";

int argon2_distance = 1000;
int argon3_distance = 1000;

OledWingAdafruit display;

SerialLogHandler logHandler;

void connectedCallback(const BlePeerDevice& peer, void* context);
void disconnectedCallback(const BlePeerDevice& peer, void* context);

BleUuid serviceUuid("afe7acc5-33a9-478f-bbe1-8944aa08e884");

void setup() {

  display.setup();

  client.subscribe(argon2);
  client.subscribe(argon3);

	BLE.on();

	BLE.onConnected(connectedCallback, NULL);
	BLE.onDisconnected(disconnectedCallback, NULL);

  BleAdvertisingData advData;
  advData.appendServiceUUID(serviceUuid);
  BLE.advertise(&advData);
}

void loop() {
  display.loop();

  if (client.isConnected()) {
    client.loop();

  } else {
    //subscribes and connects if not connected
    client.connect(System.deviceID());
    client.subscribe(argon2);
    client.subscribe(argon3);
  }

  format_display();

  display.println("A2 dist : A3 dist");

  display.print(argon2_distance);
  display.print(" : ");
  display.print(argon3_distance);
  display.display();
}

void callback(char *topic, byte *payload, unsigned int length) {
  //Function called when ever data is recieved from MQTT subscriptions

  //formats and extracts data from mqtt 
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String s = p;
  double value = s.toFloat();

  String callbackTopic = topic; //makes the mqtt topic of the data into an arduino string

  if (callbackTopic.compareTo(argon2)) {
    argon2_distance = value;
  } else {
    argon3_distance = value;
  }

}

void connectedCallback(const BlePeerDevice& peer, void* context) {
	Log.info("connected");

}

void disconnectedCallback(const BlePeerDevice& peer, void* context) {
	Log.info("disconnected");

}

void format_display () {
  //sets up basic formatting for when using the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
}