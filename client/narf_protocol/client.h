#ifndef NARF_WIRELESS_PROTOCOL_CLIENT_H
#define NARF_WIRELESS_PROTOCOL_CLIENT_H

#include <iostream>
#include <stdio.h>

class NarfWirelessProtocolClient
{
    public:
        /* Constructor */
        NarfWirelessProtocolClient(); 

        /* 
            Send protocol msg and get response ffrom server.
            \param cmd request command
            \lenght request data lenght
            \req_data data with request
            \res_data server response data 
        */
        uint8_t sendProtocolMsg(uint8_t cmd, short int lenght, uint8_t req_data[], uint8_t res_data[]);
};

#endif //NARF_WIRELESS_PROTOCOL_CLIENT_H