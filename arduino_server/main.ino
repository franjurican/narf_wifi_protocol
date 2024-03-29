#include "narf_protocol/server.h"
#include "narf_protocol/some_fun.h"

#define BUTTON_PIN   4
#define LED_PIN      8

NarfWirelessProtocolServer narf;

void setup()
{
    // pinovi
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    // serial
    Serial.begin(9600);
    Serial.println("Pokrecem AP ...");

    // detect wifi module
    if(WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Ne mogu pristupiti WiFi modulu.");
        while(1){}
    }

    // create AP
    narf.initializeWiFiModuleAP(IPAddress(192, 168, 1, 24));
}

void loop()
{   
    // change led - manually
    changeLED(BUTTON_PIN, LED_PIN); 

    // wifi communication
    if(WiFi.status() == WL_AP_CONNECTED)
    {   
        narf.checkForProtocolMsg();
    }
    else 
        Serial.println(WiFi.status());

    // loop 20 Hz
    // delay(50);
}