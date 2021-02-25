#ifndef NARF_WIRELESS_PROTOCOL_SERVER_H
#define NARF_WIRELESS_PROTOCOL_SERVER_H

// headers
#include <SPI.h>
#include <WiFiNINA.h>
#include "definitions.h"

class NarfWirelessProtocolServer
{
    public:
        /* Constructor */
        NarfWirelessProtocolServer(int port = NARF_PROT_PORT_NUM);

        /* 
            Check for protocol message on ALL clients. When client sends message, this method will execute 
            protocol message and send response to client. Non blocking!
            \param timeout time to wait for next client byte in miliseconds (only if client sends message) 
        */
        void checkForProtocolMsg(int timeout = NARF_PROT_BYTE_WAIT_TIMEOUT);

        /* 
            Initialize WiFi module as AP and connects arduino to AP.
            \param arduinoIP IP adress of arduino
        */
        void initializeWiFiModuleAP(IPAddress arduinoIP);

    private:
        /* 
            Checks for header "inside" header situations! 
            \param client connection with client
        */
        bool checkHeaderBody(WiFiClient &client);

        /* 
            Detects message header - start of message packet.
            \param client connection with client
         */
        bool detectMsgHeader(WiFiClient &client);

        /*
            Get body of protocol message packet.
            \param client connection with client
            \param cmd command(description) of packet load
            \param data actual data in message body
            \return lenght(bytes) of message body, on error returns negative value!
        */
        int getRawMsgBody(WiFiClient &client, uint8_t *cmd, uint8_t data[]);
        
        /*
            Responde to protocol msg.
            \param client connection with client
            \param lenght response data lenght
            \param response_code response code
            \param data response data 
        */
        void respondeToMsg(WiFiClient &client, int lenght, uint8_t response_code, uint8_t data[]);

        /*  
            Execute request cmd.
            \param client connection with client
            \param lenght command data lenght
            \param cmd command
            \param data command data  
        */
        void executeRequest(WiFiClient &client, int lenght, uint8_t cmd, uint8_t data[]);
        
        //////////////////////////////
        // POSSIBLE REQUEST METHODS //
        //////////////////////////////
        /*
            Request method for reading digital PINs logical values. Async PIN read!!
            \param client connection with client
            \param lenght data lenght - number of PINs
            \param data request data - PIN numbers(one byte per PIN)
        */
        void reqReadPinsD(WiFiClient &client, int lenght, uint8_t data[]);

        /*
            Request method for writing to digital PINs.
            \param client connection with client
            \param lenght data lenght
            \param data request data - PIN numbers(one byte per PIN)
        */
        void reqWritePinsD(WiFiClient &client, int lenght, uint8_t data[]);

        // private variables
        /* module init successfully */
        bool module_init;

        /* current connection to client */
        WiFiClient client;

        /* server for protocol */
        WiFiServer server;

        /* count recursive function calls - checkHeaderBody() */
        int head_rec_count;
};

#endif // NARF_WIRELESS_PROTOCOL_SERVER_H