#include "narf_protocol/server.h"

// #define NARF_DEBUG_MODE 

#ifdef NARF_DEBUG_MODE
#define PRINT_SERIAL_MSG(msg) Serial.print(msg);
#define PRINT_SERIAL_DATA(data, serial_option) Serial.print(data, serial_option); Serial.print(" ");
#define PRINT_SERIAL_ARRAY(data, lenght, serial_option) for(int i = 0; i < lenght; i++) {Serial.print(data[i], serial_option); Serial.print(" ");}
#else
#define PRINT_SERIAL_MSG(msg)
#define PRINT_SERIAL_DATA(data, serial_option)
#define PRINT_SERIAL_ARRAY(data, lenght, serial_option)
#endif 

NarfWirelessProtocolServer::NarfWirelessProtocolServer(int port) : server(port)
{}

void NarfWirelessProtocolServer::checkForProtocolMsg(int timeout)
{
    // local vars
    int lenght;
    uint8_t packet_number, cmd, data[NARF_PROT_MAX_MSG_DATA_SIZE] = {0};

    // get connection with client and set timeout(between bytes!)
    this->client = this->server.available();
    this->client.setTimeout(timeout);

    // if client have sent data read protocol msg
    if(this->client) 
    {
        // get packet
        PRINT_SERIAL_MSG("\n")
        PRINT_SERIAL_MSG("Dostupno bytova na clientu: ")
        PRINT_SERIAL_DATA(this->client.available(), DEC);
        PRINT_SERIAL_MSG("\n")
        PRINT_SERIAL_MSG("Header: ")
        unsigned long int t1 = millis(), t2;
        lenght = this->getMsgPacketSecure(this->client, packet_number, cmd, data);
        this->executeRequest(client, packet_number, lenght, cmd, data);
        t2 = millis() - t1;

        Serial.print("Vrijeme izvrsavanja zahtjeva: ");
        Serial.println(t2);
        Serial.println();
    }
    else if(!this->client.connected())
    {
        this->closeConnection(this->client);
        PRINT_SERIAL_MSG("Klijent zatvorio konekciju, gasim socket!")
        PRINT_SERIAL_MSG("\n")
    }
}

bool NarfWirelessProtocolServer::initializeWiFiModuleAP(IPAddress arduinoIP)
{
    // create AP
    if(WiFi.beginAP(NARF_AP_SSID, NARF_AP_PASSWORD) != WL_AP_LISTENING)
    {
        Serial.println("Nisam uspio kreirati AP.");
        return false; 
    }
    
    // AP info
    Serial.println("AP kreiran uspijesno!");
    Serial.print("SSID: ");
    Serial.println(NARF_AP_SSID);
    Serial.print("Password: ");
    Serial.println(NARF_AP_PASSWORD);

    // connect arduino to AP and start protocol server
    WiFi.config(arduinoIP);
    this->server.begin();
    Serial.print("\nIP adresa arduina na kreiranom WLAN-u: ");
    Serial.println(WiFi.localIP());
    return true;
}

int NarfWirelessProtocolServer::getMsgPacketSecure(WiFiClient &client, uint8_t &pack_num, uint8_t &cmd, uint8_t data[])
{
    uint8_t buff, head[NARF_PROT_HEADER_SIZE] = {0}, up, down;
    int bytes_read, lenght;

    ////////////
    // header //
    ////////////
    bytes_read = client.readBytes(head, sizeof(head));
    PRINT_SERIAL_MSG("Header: ");
    PRINT_SERIAL_ARRAY(head, NARF_PROT_HEADER_SIZE, HEX)
    PRINT_SERIAL_MSG("\n")
    if(bytes_read != sizeof(head))
        return -1;

    if(head[0] != 0xAC || head[1] != 0x46 || head[2] != 0x72 || head[4] != 0x41 || head[5] != 0x6E || head[6] != 0x4A 
            || head[7] != NARF_PROT_VER_MAX || head[8] != NARF_PROT_VER_MIN || head[9] != 0xDC)
        return -4; 
    else
        pack_num = head[3];

    //////////
    // data //
    //////////
    // read lenght bytes
    bytes_read = client.readBytes(&up, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;

    bytes_read = client.readBytes(&down, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;

    // get lenght
    lenght = (int)up << 8 | (int)down;

    PRINT_SERIAL_MSG("Duljina podataka: ");
    PRINT_SERIAL_DATA(lenght, DEC);
    PRINT_SERIAL_MSG("\n")
    if(lenght > NARF_PROT_MAX_MSG_DATA_SIZE || lenght < 0)
        return -2;
    
    // read cmd
    bytes_read = client.readBytes(&cmd, sizeof(uint8_t));
    PRINT_SERIAL_MSG("Cmd: ");
    PRINT_SERIAL_DATA(cmd, HEX);
    PRINT_SERIAL_MSG("\n")
    if(bytes_read != sizeof(uint8_t))
        return -1;

    // read data only if there is data in msg
    if(lenght > 0)
    {
        bytes_read = client.readBytes(data, lenght);
        PRINT_SERIAL_MSG("Podatci: ");
        PRINT_SERIAL_ARRAY(data, lenght, DEC)
        PRINT_SERIAL_MSG("\n")
        if(bytes_read != lenght)
            return -1;
    }

    return lenght;  
}

void NarfWirelessProtocolServer::respondeToMsg(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t response_code, uint8_t data[])
{
    // send header
    uint8_t packet[NARF_PROT_HEADER_SIZE + 3 + lenght] = {0xAC, 0x46, 0x72, pack_num, 0x41, 0x6E, 0x4A, NARF_PROT_VER_MAX, NARF_PROT_VER_MIN, 0xDC};
    
    // lenght and response code
    packet[NARF_PROT_HEADER_SIZE] = (lenght & 0xFF00) >> 8;
    packet[NARF_PROT_HEADER_SIZE + 1] = (lenght & 0x00FF);
    packet[NARF_PROT_HEADER_SIZE + 2] = response_code;

    // data
    for(int i = 0; i < lenght; i++)
        packet[NARF_PROT_HEADER_SIZE + 3 + i] = data[i];
    
    // send response
    client.write(packet, sizeof(packet)); 
}

void NarfWirelessProtocolServer::executeRequest(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t cmd, uint8_t data[])
{
    // check for errors, and close connection channel
    if(lenght < 0)
    {
        switch (lenght)
        {
            case -1:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_TIMEOUT_C, NULL);
                PRINT_SERIAL_MSG("Poslan error: ")
                PRINT_SERIAL_DATA(NARF_RES_ERROR_TIMEOUT_C, HEX)
                PRINT_SERIAL_MSG("\n")
                break;
            case -2:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_MSG_SIZE_C, NULL);
                PRINT_SERIAL_MSG("Poslan error: ")
                PRINT_SERIAL_DATA(NARF_RES_ERROR_MSG_SIZE_C, HEX)
                PRINT_SERIAL_MSG("\n")
                break;
            case -4:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_COMMUNICATION_C, NULL);
                PRINT_SERIAL_MSG("Poslan error: ")
                PRINT_SERIAL_DATA(NARF_RES_ERROR_COMMUNICATION_C, HEX)
                PRINT_SERIAL_MSG("\n")
                break;
            default:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_UNKNOWN_C, NULL);
                PRINT_SERIAL_MSG("Poslan error: ")
                PRINT_SERIAL_DATA(NARF_RES_ERROR_UNKNOWN_C, HEX)
                PRINT_SERIAL_MSG("\n")
                break;
        }
        
        // close connection
        delay(200);
        this->closeConnection(client);
        PRINT_SERIAL_MSG("Gasim konekciju - server!")
        PRINT_SERIAL_MSG("\n")
        return;
    }

    // call requested method
    switch (cmd)
    {
        case NARF_CMD_READ_PINS_D:
            this->reqReadPinsD(client, pack_num, lenght, data);
            break;
        case NARF_CMD_WRITE_PINS_D:
            this->reqWritePinsD(client, pack_num, lenght, data);
            break;
        default:
            this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_CMD_UNKNOWN, NULL);
            break;
    }
}

void NarfWirelessProtocolServer::closeConnection(WiFiClient &client)
{
    client.stop();
    if(client.available())
        client.flush();
}

void NarfWirelessProtocolServer::reqReadPinsD(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t data[])
{
    // if request dosen't have data
    if(lenght == 0)
    {
        this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
        return;
    }

    // out data    
    uint8_t data_out[lenght] = {0};

    // read digital PINs
    for(int i = 0; i < lenght; i++)
    {   
        // does PIN exists
        if(data[i] < (uint8_t)NARF_PROT_PIN_MIN_NUM || data[i] > (uint8_t)NARF_PROT_PIN_MAX_NUM)
        {
            this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
            return;
        }

        data_out[i] = digitalRead(data[i]);
    }

    // send data to client
    this->respondeToMsg(client, pack_num, lenght, NARF_RES_OK, data_out);
}

void NarfWirelessProtocolServer::reqWritePinsD(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t data[])
{
    // if request data dosen't have even number of bytes
    if(lenght % 2 || lenght == 0)
    {
        this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
        return;
    }

    // write digital PINs
    for(int i = 0; i < lenght; i += 2)
    {   
        // does PIN exists
        if(data[i] < (uint8_t)NARF_PROT_PIN_MIN_NUM || data[i] > (uint8_t)NARF_PROT_PIN_MAX_NUM || data[i + 1] > (uint8_t)1)
        {
            this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
            return;
        }

        digitalWrite(data[i], data[i + 1]);
    }

    // send data to client
    this->respondeToMsg(client, pack_num, 0, NARF_RES_OK, NULL);
}