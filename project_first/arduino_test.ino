#include <DHT.h>

#define DHTPIN 8

DHT dht(DHTPIN, DHT11);

void setup()
{
    Serial.begin(9600);
    Serial.println("Pocinjem mjeriti temperaturu i vlagu ...");
    dht.begin();
}

void loop()
{
    delay(5000);
    float t, h, feel;

    t = dht.readTemperature();
    h = dht.readHumidity();

    if(isnan(t) || isnan(h))
    {
        Serial.println("Nema podataka za citanje! Pokusavam ponovno ...");
        return;
    } 

    feel = dht.computeHeatIndex(t, h, false);
    Serial.print("Vlaga: ");
    Serial.print(h);
    Serial.print(" %, temperatura: ");
    Serial.print(t);
    Serial.print(" C, osjecaj temperature: ");
    Serial.print(feel);
    Serial.println(" C");
}