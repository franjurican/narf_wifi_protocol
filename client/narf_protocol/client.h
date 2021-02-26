#ifndef NARF_WIRELESS_PROTOCOL_CLIENT_H
#define NARF_WIRELESS_PROTOCOL_CLIENT_H

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

#define ERROR(msg) {std::cout << msg << std::endl; std::cout << "ERRNO: " << errno << std::endl; exit(24);}

class NarfWirelessProtocolClient
{
    public:
        /* Constructor */
        NarfWirelessProtocolClient(std::string serverIPv4, uint16_t server_port = NARF_PROT_PORT_NUM); 

        /* Destructor */
        ~NarfWirelessProtocolClient();

        /* 
            Connect to server. This method throws possible connection errors. 
            \param timeout_ms maximal wait time for server bytes
            \param tcp_delay enable Nagles buffering(write delay up to 200ms)
        */
        void connectToServer(uint32_t timeout_ms = NARF_PROT_BYTE_WAIT_TIMEOUT, bool tcp_delay = false);

        /* 
            Send protocol msg and get response from server.
            \param cmd request command
            \param lenght request data lenght
            \param req_data data with request
            \param res_data server response data 
        */
        uint8_t sendProtocolMsg(uint8_t cmd, short int lenght, uint8_t req_data[], short int &res_lenght, uint8_t res_data[]);

    private:
        /* 
            Checks for header "inside" header situations!
            \param pack_num packet number   
            \return true if header is finded
        */
        bool checkHeaderBody(uint8_t pack_num);

        /* 
            Detects message header - start of message packet.
            \param pack_num packet number
            \return true if header is finded
        */
        bool detectMsgHeader(uint8_t pack_num);

        /* 
            Read response msg.
            \param pack_num packet number
            \param res_lenght response data lenght
            \param res_data server response data
            \return response code
        */
        uint8_t getResponseMsg(uint8_t pack_num, short int &res_lenght, uint8_t res_data[]);

        /*
            Generate packet number.
            \retunr packet number
        */
        uint8_t generatePacketNumber();

        // private variables
        /* file descriptor */
        int sock_fd;

        /* server IPv4 address*/
        std::string serverIP;

        /* server port number */
        uint_fast16_t server_port;

        /* count recursive function calls - checkHeaderBody() */
        int head_rec_count;

        /* msg packet number */
        uint8_t packet_num;
};

#endif //NARF_WIRELESS_PROTOCOL_CLIENT_H