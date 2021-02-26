#include "narf_protocol/server.h"

NarfWirelessProtocolServer::NarfWirelessProtocolServer(int port) : module_init(false), server(port), head_rec_count(0)
{}

void NarfWirelessProtocolServer::checkForProtocolMsg(int timeout)
{
    int lenght;
    uint8_t packet_number, cmd = 0, data[NARF_PROT_MAX_MSG_DATA_SIZE] = {0};

    // check if module is initialized
    if(!this->module_init)
        return;

    // get connection with client and set timeout(between bytes!)
    this->client = this->server.available();
    this->client.setTimeout(timeout);

    // if client have sent data read protocol msg
    if(this->client) 
    {
        packet_number = this->detectMsgHeader(this->client);

        Serial.print("Header packet number: ");
        Serial.println(packet_number);
        Serial.println();
            
        if(packet_number)
        {
            lenght = this->getRawMsgBody(this->client, &cmd, data);

            Serial.println("Primljena poruka: ");
            Serial.print("Duljina poruke: ");
            Serial.println(lenght);
            Serial.print("Primljena naredba: ");
            Serial.println(cmd);
            Serial.print("Primljeni podatci: ");
            Serial.println((int)data[0]);
            Serial.println();

            this->executeRequest(client, packet_number, lenght, cmd, data);
        }
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

uint8_t NarfWirelessProtocolServer::checkHeaderBody(WiFiClient &client)
{
    uint8_t buff, pack_num;
    int bytes_read;

    // for MAX recursive function calls!!
    if(this->head_rec_count == NARF_PROT_REC_HEAD_MAX)
        return 0;
    else
        this->head_rec_count++;

    // protocol check bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0x46)
        return 0;

    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0x72)
        return 0;

    // packet number
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else
        pack_num = buff;
    
    // protocol check bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0x41)
        return 0;
    
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0x6E)
        return 0;

    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0x4A)
        return 0;
    
    // version max - CAN'T BE EQUAL TO START BITS!!!!
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != NARF_PROT_VER_MAX)
        return 0;

    // version min - CAN'T BE EQUAL TO START BITS!!!!
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != NARF_PROT_VER_MIN)
        return 0;

    // header end bytes
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return 0;
    else if(buff == 0xAC)
        return this->checkHeaderBody(client);
    else if(buff != 0xDC)
        return 0;

    return pack_num;
}

uint8_t NarfWirelessProtocolServer::detectMsgHeader(WiFiClient &client)
{
    uint8_t buff, dummy;
    int bytes_read;

    // read first byte
    bytes_read = client.readBytes(&buff, sizeof(uint8_t));

    // detect header
    if(bytes_read != sizeof(uint8_t) || buff != 0xAC)
        return 0;
    else
    {
        dummy = this->checkHeaderBody(client);
        this->head_rec_count = 0;
        return dummy;
    }
}

int NarfWirelessProtocolServer::getRawMsgBody(WiFiClient &client, uint8_t *cmd, uint8_t data[])
{
    int lenght, bytes_read;

    // read msg body lenght
    bytes_read = client.readBytes((uint8_t *) &lenght, sizeof(int));
    if(bytes_read != sizeof(int))
        return -1;
    else if(lenght > NARF_PROT_MAX_MSG_DATA_SIZE)
        return -2;
    
    // read cmd
    bytes_read = client.readBytes(cmd, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return -1;

    // read data only if there is data in msg
    if(lenght > 0 && data != NULL)
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
    const uint8_t header[15] = {0xAC, 0x46, 0x72, pack_num, 0x41, 0x6E, 0x4A, NARF_PROT_VER_MAX, NARF_PROT_VER_MIN, 0xDC};
    client.write(header, sizeof(header));

    // send lenght
    client.write((const uint8_t*) &lenght, sizeof(lenght));

    // send response code
    client.write(response_code);

    // send response data
    if(data != NULL && lenght > 0)
        client.write((const uint8_t*)data, lenght);
}

void NarfWirelessProtocolServer::executeRequest(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t cmd, uint8_t data[])
{
    // check for errors
    if(lenght == -1)
    {
        this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_TIMEOUT, NULL);
        return;
    }
    else if(lenght == -2)
    {
        this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_MSG_SIZE, NULL);
        return;
    }
    else if(lenght < 0)
    {
        this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_UNKNOWN, NULL);
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
        this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
        return;
    }

    // out data    
    uint8_t data_out[lenght] = {0};

    // read digital PINs
    for(int i = 0; i < lenght; i++)
    {   
        // does PIN exists
        if(data[i] < NARF_PROT_PIN_MIN_NUM || data[i] > NARF_PROT_PIN_MAX_NUM)
        {
            this->respondeToMsg(client, pack_num, 0, NARF_RES_ERROR_INVALID_DATA, NULL);
            return;
        }

        data_out[i] = digitalRead(data[i]);
    }

    // send data to client
    this->respondeToMsg(client, pack_num, lenght, NARF_RES_OK, data_out);
}

void NarfWirelessProtocolServer::reqWritePinsD(WiFiClient &client, uint8_t pack_num, int lenght, uint8_t data[])
{

}