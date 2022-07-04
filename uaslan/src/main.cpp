#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int ldr =A0;

const char *ssid = "Black java cafe";            // sesuaikan dengan username wifi
const char *password = "pesanapa";        // sesuaikan dengan password wifi
const char *mqtt_server = "broker.hivemq.com"; // isikan server broker

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;
long lastMsg = 0;
int value =0;
String macAddr = "";


void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

void callback (char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("massage arrived [");
  Serial.print(topic);
  Serial.print("]");
  for (unsigned int i = 0 ; i < length ; i ++) {
    Serial.print ((char)payload[i]);
    string += ((char)payload[i]);
  }
  Serial.println(string);
  if ((char)payload[0]=='0') {
    Serial.println("lampu mati");
    digitalWrite(D5, LOW);
  }
  else if ((char)payload[0]=='1') {
    Serial.println("lampu nyala");
    digitalWrite(D5, HIGH);
  }

  Serial.println();
}

void setup()
{
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str()))
    {
      Serial.println("connected");
      client.subscribe ("lampu");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void loop()
{
  
  
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect(macAddr.c_str());
  }
  now = millis();
  if (now - lastMeasure > 5000)
  {
    lastMeasure = now;
    int nilai = analogRead(ldr);
    static char intensitasCahaya [7];
    dtostrf (nilai, 6 , 2 , intensitasCahaya);
    client.publish("cahaya", intensitasCahaya);
    
    Serial.print("cahaya =");
    Serial.print(nilai);
    Serial.println("v");
  }
}