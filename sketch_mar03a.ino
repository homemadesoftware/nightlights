#include <ESP8266WiFi.h>

const char* ssid = "You will be hacked";
const char* password = "";

const bool isSonoff = true;
const char* host = "sunsetandsunrise.azurewebsites.net";

const char* api = "/api/sun";
//const char* api = "/api/mocksun";

const int timeIncrement = 60000;
//const int timeIncrement = 1000;

const int totalDayUnits = 1440;
//const int totalDayUnits = 40;

int currentTime = 0;
int sunRise = 1;
int sunSet = 2;


void setup()
{
    if (!isSonoff)
    {
        // Adafruit
        // Button and LED
        pinMode(0, OUTPUT);
    }
    else
    {
        // Sonoff
        pinMode(12, OUTPUT);
        pinMode(13, OUTPUT);
    }


    Serial.begin(115200);
    delay(1000);
}



void loop()
{
    if (currentTime == 0)
    {
        if (connectToWifi())
        {
            calibrate();
        }
    }


    delay(timeIncrement);

    if (++currentTime >= totalDayUnits)
    {
        currentTime = 0;
    }
    
    Serial.print("current time ");
    Serial.print(String(currentTime));
    Serial.println();


    bool lightsOn = (currentTime < sunRise || currentTime > sunSet);

    if (isSonoff)
    {
        digitalWrite(12, lightsOn ? HIGH : LOW);
        digitalWrite(13, lightsOn ? LOW : HIGH);
    }
    else
    {
        digitalWrite(0, lightsOn ? LOW : HIGH);
    }

}


bool calibrate()
{
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return false;
    }

    // We now create a URI for the request
    String url = api;
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    int attempts = 60;
    while (attempts-- > 0)
    {
      delay(1000);

      while (client.available())
      {
          String line = client.readStringUntil('\r');
          Serial.print(line);

          if (readOutput(line))
          {
            goto complete;
          }
          
          
      }
    }
                 
    complete: 
    
    Serial.println();
    Serial.println("closing connection");
    return true;
}

bool readOutput(String line)
{
  int index = line.indexOf("CT");
  if (index >= 0)
  {
    currentTime = line.substring(index + 2).toInt();
    return false;
  }
  index = line.indexOf("SR");
  if (index >= 0)
  {
    sunRise = line.substring(index + 2).toInt();
    return false;
  }

  index = line.indexOf("SS");
  if (index >= 0)
  {
    sunSet = line.substring(index + 2).toInt();
    return true;
  }
  return false;
}

bool connectToWifi()
{
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int attempts = 240;
    while (WiFi.status() != WL_CONNECTED)
    {
        if (--attempts < 0)
        {
            return false;
        }
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    return true;
}


