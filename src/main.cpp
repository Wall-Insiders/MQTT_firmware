#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MQTT_Test.h"

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

const char *ssid = "Aiden's IPhone";
const char *pass = "d7ru9sq5xhp4d";
const char *topic = TOPIC;

//broker adress, going to use mqtt test server for testing
const char *broker  = mqtt_server;

WiFiClient espClient;
PubSubClient client (espClient);

//button state stuff
//boolean values for now
unsigned long debounceDuration = 100;
bool prevSwitch_State = 0;
bool switch_State = 0;

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
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
}

void reconnect()
 {
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker");

    if(client.connect("ESP8266Client"))
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
    }
  }
 }

void loop() {
  // put your main code here, to run repeatedly:

  prevSwitch_State = switch_State;
  switch_State = digitalRead(button);

  if (!client.connected())
  {
    reconnect();
    digitalWrite(MQTT_LED, HIGH);
  }
  
  if(digitalRead(button) == 0)
  {
    prevSwitch_State != switch_State;
    if (switch_State == 1)
    {
      client.publish(topic, "on");
      Serial.println((String)topic + " => on");
    }
    else
    {
      client.publish(topic, "off");
      Serial.println((String)topic + "=> off");
    }

    while (digitalRead(switch_State) == 0)
    {
      yield();
      delay(20);
    }
  }
}

