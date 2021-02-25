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
            \return true if header is finded
        */
        bool checkHeaderBody();

        /* 
            Detects message header - start of message packet.
            \return true if header is finded
        */
        bool detectMsgHeader();

        /* 
            Read response msg.
            \param res_data server response data
            \return response code
        */
        uint8_t getResponseMsg(short int &res_lenght, uint8_t res_data[]);

        // private variables
        /* file descriptor */
        int sock_fd;

        /* server IPv4 address*/
        std::string serverIP;

        /* server port number */
        uint_fast16_t server_port;

        /* count recursive function calls - checkHeaderBody() */
        int head_rec_count;
};

#endif //NARF_WIRELESS_PROTOCOL_CLIENT_H