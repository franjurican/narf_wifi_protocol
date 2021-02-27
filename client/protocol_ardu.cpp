#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <narf_protocol/client.h>
#include <narf_protocol/definitions.h>

void printResponseMsg(uint8_t cmd, short int lenght, uint8_t data[])
{
    if(cmd == NARF_RES_OK)
    {
        std::cout << "Poruka uspjesno poslana!" << std::endl;
        std::cout << "Response data lenght: " << lenght << std::endl;
        std::cout << "Response cmd: " << (unsigned int)cmd << std::endl;
        
        for(int i = 0; i < lenght; i++)
            std::cout << "Response data[" << i << "]: " << (unsigned int)data[i] << std::endl;   
    }
    else
    {
        std::cout << "Poruka NIJE uspjesno poslana!" << std::endl;
        std::cout << "Response error code: " << (unsigned int)cmd << std::endl;
    }

    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    NarfWirelessProtocolClient client("192.168.1.24");

    client.connectToServer();

    uint8_t res_cmd;
    uint8_t data[] = {8, 1}, res_data[NARF_PROT_MAX_MSG_DATA_SIZE];
    short int lenght = sizeof(data), res_lenght;

    res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
    printResponseMsg(res_cmd, res_lenght, res_data);
    usleep(2e6);

    data[1] = 0;
    res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
    printResponseMsg(res_cmd, res_lenght, res_data);

    return 0;
}