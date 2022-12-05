#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MQTT_Test.h"

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *topic = TOPIC;

//broker adress, going to use mqtt test server for testing
const char *broker  = mqtt_server;

WiFiClient espClient;
PubSubClient client (espClient);

//button state stuff
//boolean values for now

//need to add button debouncing

//need to make them float for analog values
unsigned long debounceDuration = 100;
bool prevSwitch_State = 0;
bool switch_State = 0;
String button_pressed = "Button is not pressed";
bool mqtt_status = 0;
bool prev_mqtt_status = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(button, INPUT);
  pinMode(Network_LED, OUTPUT);
  pinMode(MQTT_LED, OUTPUT);

  //wait for serial to connect
  while (!Serial){;}

  setup_wifi();
  client.setServer(broker, 1883);

  reconnect();
}

void setup_wifi()
{
  Serial.println();
  Serial.println("Connecting to network");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.print(".");
    Serial.print(WiFi.status());
  }

  Serial.println("You are connected");
  Serial.println();
  digitalWrite(Network_LED, HIGH);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
 {
  while(!client.connected())
  {
    Serial.println("Connecting to MQTT broker");

    if(client.connect("ESP32Client"))
    {
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
      Serial.println('\n');
    }
    else
    {
      Serial.print("Failed to connect; rc: ");
      Serial.print(client.state());
      Serial.println("trying again in 5 seconds");
      delay(5000);
      while(1);
    }
  }
 }

void loop() {
  // put your main code here, to run repeatedly:

  prevSwitch_State = switch_State;
  switch_State = digitalRead(button);

  prev_mqtt_status = mqtt_status;
  mqtt_status = client.connected();

  if (!client.connected())
  {
    // reconnect();
    digitalWrite(MQTT_LED, HIGH);
  }
  
  if(prevSwitch_State != switch_State)
  {
    if (switch_State == HIGH)
    {
      button_pressed = "Button is pressed";
    }
    else
    {
      button_pressed = "Button is not pressed";
    }
    Serial.println("Button send to topic: ");
    Serial.println(button_pressed);
    Serial.println(switch_State);

    client.publish(topic, button_pressed.c_str());

    while (digitalRead(switch_State) == 0)
    {
      yield();
      delay(20);
    }
  }
}

