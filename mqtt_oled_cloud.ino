#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char *ssid = "****";
const char *password = "****";

const char *mqtt_server = "9ed9866705fd4cbca12d5d806169baa5.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "123qwe";
const char *mqtt_pass = "Qwerty123";
const char *mqtt_topic = "get/text";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

unsigned long lastMsg = 0;
const long msgInterval = 5000;

void connectWiFi()
{
    Serial.print("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi");
    }
}

void connectMQTT()
{
    secureClient.setInsecure();
    client.setServer(mqtt_server, mqtt_port);

    int attempts = 0;
    while (!client.connected() && attempts < 5)
    {
        Serial.print("Connecting to MQTT...");
        String clientId = "ESP8266Client-" + String(ESP.getChipId());
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" - retrying in 5 seconds");
            delay(5000);
            attempts++;
        }
    }
    if (!client.connected())
    {
        Serial.println("MQTT connect failed after 5 attempts");
    }
}

void setup()
{
    Serial.begin(115200);
    connectWiFi();
    connectMQTT();
    randomSeed(analogRead(0));
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi lost, reconnecting...");
        connectWiFi();
    }
    if (!client.connected())
    {
        connectMQTT();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastMsg > msgInterval)
    {
        lastMsg = now;
        // Генеруємо випадкове повідомлення
        int randNum = random(1000, 9999);
        String msg = "Random message: " + String(randNum);
        if (client.publish(mqtt_topic, msg.c_str()))
        {
            Serial.print("Published: ");
            Serial.println(msg);
        }
        else
        {
            Serial.println("Failed to publish");
        }
    }
}