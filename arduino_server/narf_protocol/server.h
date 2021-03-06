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
        bool initializeWiFiModuleAP(IPAddress arduinoIP);

    private:
        /*
            Responde to protocol msg.
            \param client connection with client
            \param pack_num packet number
            \param lenght response data lenght
            \param response_code response code
            \param data response data 
        */
        void respondeToMsg(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t response_code, uint8_t data[]);

        /*  
            Execute request cmd. If there is error with packet(eror code is in lenght!!), close conection!
            \param client connection with client
            \param pack_num packet number
            \param lenght command data lenght
            \param cmd command
            \param data command data  
        */
        void executeRequest(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t cmd, uint8_t data[]);

        /*
            Get msg packet secure. This method gets msg packet, if there is some error with packet, 
            connection must be closed!!
            \param client connection with client
            \param pack_num packet number
            \param cmd command
            \param data command data
            \return data lenght, on error returns error code: -1 for timeout, -2 for data lenght, -4 for communication error
        */
        int getMsgPacketSecure(WiFiClient &client, uint8_t &pack_num, uint8_t &cmd, uint8_t data[]);

        /* 
            Close connection.
            \param client connection with client
        */
        void closeConnection(WiFiClient &client);
        
        //////////////////////////////
        // POSSIBLE REQUEST METHODS //
        //////////////////////////////
        /*
            Request method for reading digital PINs logical values. Async PIN read!!
            \param client connection with client
            \param pack_num packet number
            \param lenght data lenght - number of PINs
            \param data request data - PIN numbers(one byte per PIN)
        */
        void reqReadPinsD(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t data[]);

        /*
            Request method for writing to digital PINs. Async PIN write!!
            \param client connection with client
            \param pack_num packet number
            \param lenght data lenght
            \param data request data - PIN numbers(one byte per PIN)
        */
        void reqWritePinsD(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t data[]);

        // private variables
        /* current connection to client */
        WiFiClient client;

        /* server for protocol */
        WiFiServer server;
};

#endif // NARF_WIRELESS_PROTOCOL_SERVER_H