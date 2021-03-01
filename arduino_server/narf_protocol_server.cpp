#include "narf_protocol/server.h"

NarfWirelessProtocolServer::NarfWirelessProtocolServer(int port) : module_init(false), server(port)
{}

void NarfWirelessProtocolServer::checkForProtocolMsg(int timeout)
{
    // check if module is initialized
    if(!this->module_init)
        return;
    
    // local vars
    int lenght;
    uint8_t packet_number = 255, cmd = 255, data[NARF_PROT_MAX_MSG_DATA_SIZE] = {0};

    // get connection with client and set timeout(between bytes!)
    this->client = this->server.available();
    this->client.setTimeout(timeout);

    // if client have sent data read protocol msg
    if(this->client) 
    {
        // get packet
        unsigned long int t1 = millis(), t2;
        lenght = this->getMsgPacketSecure(this->client, packet_number, cmd, data);
        this->executeRequest(client, packet_number, lenght, cmd, data);
        t2 = millis() - t1;

        Serial.print("Vrijeme izvrsavanja zahtjeva: ");
        Serial.println(t2);
        Serial.println();

        Serial.print("Header packet number: ");
        Serial.println(packet_number);
        Serial.println();

        Serial.println("Primljena poruka: ");
        Serial.print("Duljina poruke: ");
        Serial.println(lenght);
        Serial.print("Primljena naredba: ");
        Serial.println(cmd);
        Serial.print("Primljeni podatci: ");
        Serial.println((int)data[0]);
        Serial.println();
    }
}

void NarfWirelessProtocolServer::initializeWiFiModuleAP(IPAddress arduinoIP)
{
    // detect wifi module
    if(WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Ne mogu pristupiti WiFi modulu.");
        return;
    }

    // create AP
    if(WiFi.beginAP(NARF_AP_SSID, NARF_AP_PASSWORD) != WL_AP_LISTENING)
    {
        Serial.println("Nisam uspio kreirati AP.");
        return; 
    }

    // module init successfully
    this->module_init = true;
    
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
}

int NarfWirelessProtocolServer::getMsgPacketSecure(WiFiClient &client, uint8_t &pack_num, uint8_t &cmd, uint8_t data[])
{
    uint8_t buff;
    int bytes_read, lenght;

    ////////////
    // header //
    ////////////

    // header start byte
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0xAC)
        return -4;

    // protocol check bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0x46)
        return -4;

    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0x72)
        return -4;

    // packet number
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else
        pack_num = buff;
    
    // protocol check bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0x41)
        return -4;
    
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0x6E)
        return -4;

    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0x4A)
        return -4;
    
    // version max
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != NARF_PROT_VER_MAX)
        return -4;

    // version min
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != NARF_PROT_VER_MIN)
        return -4;

    // header end bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;
    else if(buff != 0xDC)
        return -4;

    //////////
    // data //
    //////////

    // read msg body lenght
    bytes_read = client.readBytes((uint8_t *) &lenght, sizeof(int));
    if(bytes_read != sizeof(int))
        return -1;
    else if(lenght > NARF_PROT_MAX_MSG_DATA_SIZE || lenght < 0)
        return -2;
    
    // read cmd
    bytes_read = client.readBytes(&cmd, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;

    // read data only if there is data in msg
    if(lenght > 0)
    {
        bytes_read = client.readBytes(data, lenght);
        if(bytes_read != lenght)
            return -1;
    }

    return lenght;  
}

void NarfWirelessProtocolServer::respondeToMsg(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t response_code, uint8_t data[])
{
    // send header
    uint8_t header[] = {0xAC, 0x46, 0x72, pack_num, 0x41, 0x6E, 0x4A, NARF_PROT_VER_MAX, NARF_PROT_VER_MIN, 0xDC};
    client.write(header, sizeof(header));

    // send lenght
    client.write((const uint8_t*) &lenght, sizeof(lenght));

    // send response code
    client.write(response_code);

    // send response data
    if(lenght > 0)
        client.write((const uint8_t*)data, lenght);
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
                break;
            case -2:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_MSG_SIZE_C, NULL);
                break;
            case -4:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_COMMUNICATION_C, NULL);
                break;
            default:
                this->respondeToMsg(client, 0xAC, 0, NARF_RES_ERROR_UNKNOWN_C, NULL);
                break;
        }
        
        // close connection
        client.stop();
        client.flush();
        return;
    }

    // call request method
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