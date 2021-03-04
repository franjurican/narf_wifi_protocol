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

void simpleCLI(NarfWirelessProtocolClient &client)
{
    char c;
    uint8_t res_cmd;
    uint8_t data[] = {8, 1}, res_data[NARF_PROT_MAX_MSG_DATA_SIZE];
    short int lenght = sizeof(data), res_lenght;

    std::cout << "\nKontrole za lampicu: " << std::endl;
    std::cout << "1 - upali lampicu" << std::endl;
    std::cout << "0 - ugasi lampicu" << std::endl;
    std::cout << "q - izlazak" << std::endl;
    std::cout << "Unesite naredbu: ";
    std::cin >> c;

    if(c == '1')
    {
        data[1] = 1;
        res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
        if(res_cmd != NARF_RES_OK)
        {
            std::cout << "Naredba nije uspijela:" << (int)res_cmd << "! Ponovno se spajam na server!" << std::endl;
            client.reconnectToServer(); 
        } 
    }
    else if(c == '0')
    {
        data[1] = 0;
        res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
        if(res_cmd != NARF_RES_OK)
        {
            std::cout << "Naredba nije uspijela:" << (int)res_cmd << "! Ponovno se spajam na server!" << std::endl;
            client.reconnectToServer();
        }
    }
    else if(c == 'q')
        return;

    res_cmd = client.sendProtocolMsg(NARF_CMD_READ_PINS_D, 1, data, res_lenght, res_data);
    if(res_cmd != NARF_RES_OK)
    {
        std::cout << "Status lampice nije ucitan:" << (int)res_cmd << "! Ponovno se spajam na server!" << std::endl;
        client.reconnectToServer(); 
    } 
    else
        std::cout << "Status lampice: " << (res_data[0] == 1 ? "ukljucena" : "iskljucena") << "!" << std::endl;
    
    simpleCLI(client);
}

//////////
// main //
//////////
int main(int argc, char *argv[])
{
    uint8_t res_cmd;
    uint8_t data[] = {8, 1}, res_data[NARF_PROT_MAX_MSG_DATA_SIZE];
    short int lenght = sizeof(data), res_lenght;
    long unsigned int k = 0, e = 0, packet_num = 10000;
    double wait_time = 0.05e6;
    NarfWirelessProtocolClient client("192.168.1.24");

    client.connectToServer();
    std::cout << "Krece slanje " << packet_num << " paketa, brzinom od 20Hz" << std::endl;
    while(k < packet_num)
    {
        std::cout << "Poslanih paketa:" << k << ", errora: " << e << std::endl;
        data[1] = 1; 
        k++;
        res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
        printResponseMsg(res_cmd, res_lenght, res_data);

        if(res_cmd != NARF_RES_OK)
        {   
            e++;
            client.reconnectToServer(); 
        }

        //usleep(wait_time); 

        data[1] = 0;
        k++;
        res_cmd = client.sendProtocolMsg(NARF_CMD_WRITE_PINS_D, lenght, data, res_lenght, res_data);
        printResponseMsg(res_cmd, res_lenght, res_data);

        if(res_cmd != NARF_RES_OK)
        {   
            e++;
            client.reconnectToServer(); 
        }
        //usleep(wait_time);
    } 

    std::cout << "Report: \n" << "Poslano paketa: " << k << "\nPogresaka: " << e << "\nUspjesnost: " << ((float)(k - e)) / k * 100 << std::endl;

    //simpleCLI(client);
    return 0;
}