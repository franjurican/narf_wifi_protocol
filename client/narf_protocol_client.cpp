#include <narf_protocol/client.h>

NarfWirelessProtocolClient::NarfWirelessProtocolClient(std::string serverIPv4, uint16_t server_port) : serverIP(serverIPv4), 
                                                                server_port(server_port), packet_num(0)
{}

NarfWirelessProtocolClient::~NarfWirelessProtocolClient()
{   
    usleep(0.24e6);
    close(this->sock_fd);
}

void NarfWirelessProtocolClient::connectToServer(uint32_t timeout_ms, bool tcp_delay)
{
    int no_delay = !tcp_delay;
    struct sockaddr_in adresa;
    struct timeval tv;
    
    // open socket
    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
        ERROR("Problem kod otvaranja socketa!")

    // Nagle buffering
    if(setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &no_delay, sizeof(int)) < 0)
        ERROR("Problem kod ukljucivanja TCP_NODELAY!")

    // read timeout
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv)) < 0)
        ERROR("Problem kod postavljanja timeout-a!")

    // server address
    memset(&adresa, 0, sizeof(adresa));
    adresa.sin_family = AF_INET;
    adresa.sin_port = htons(this->server_port);
    if(!inet_aton(this->serverIP.c_str(), &(adresa.sin_addr)))
        ERROR("Problem kod zadavanja IP adrese Arduina")

    // connect to server(arduino)
    if(connect(sock_fd, (struct sockaddr *)&adresa, sizeof(adresa)) < 0)
        ERROR("Problem prilikom spajanja na arduino!");

    std::cout << "Uspjesno spojen na server: " << this->serverIP << ":" << this->server_port << ", socket: " << this->sock_fd << std::endl;
}

uint8_t NarfWirelessProtocolClient::sendProtocolMsg(uint8_t cmd, short int lenght, uint8_t req_data[], short int &res_lenght, uint8_t res_data[])
{   
    uint8_t pack_num = this->generatePacketNumber();
    uint8_t packet[NARF_PROT_HEADER_SIZE + 3 + lenght] = {0xAC, 0x46, 0x72, pack_num, 0x41, 0x6E, 0x4A, NARF_PROT_VER_MAX, NARF_PROT_VER_MIN, 0xDC};

    // lenght and cmd
    packet[NARF_PROT_HEADER_SIZE] = (lenght & 0xFF00) >> 8;
    packet[NARF_PROT_HEADER_SIZE + 1] = (lenght & 0x00FF);
    packet[NARF_PROT_HEADER_SIZE + 2] = cmd;

    // data
    for(int i = 0; i < lenght; i++)
        packet[NARF_PROT_HEADER_SIZE + 3 + i] = req_data[i];

    // send request
    write(this->sock_fd, packet, sizeof(packet));

    return this->getResponseMsg(pack_num, res_lenght, res_data);
}

void  NarfWirelessProtocolClient::reconnectToServer(uint32_t timeout_ms, bool tcp_delay)
{
    if(close(this->sock_fd) < 0)
         ERROR("Problem kod zatvaranja socket-a!")

    this->connectToServer(timeout_ms, tcp_delay);    
}

uint8_t NarfWirelessProtocolClient::getResponseMsg(uint8_t pack_num, short int &res_lenght, uint8_t res_data[])
{
    uint8_t buff, cmd, up, down;
    int bytes_read;

    ////////////
    // header //
    ////////////
    // header start byte
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0xAC)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    // protocol check bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0x46)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0x72)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    // packet number
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != packet_num && buff != 0xAC)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;
    
    // protocol check bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0x41)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;
    
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0x6E)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0x4A)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;
    
    // version max
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != NARF_PROT_VER_MAX)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    // version min
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != NARF_PROT_VER_MIN)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    // header end bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;
    else if(buff != 0xDC)
        return NARF_CLIENT_ERROR_COMMUNICATION_C;

    //////////
    // data //
    //////////
    // read lenght bytes
    bytes_read = read(this->sock_fd, &up, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;

    bytes_read = read(this->sock_fd, &down, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;

    // get lenght
    res_lenght = (short int)up << 8 | (short int)down;

    if(res_lenght > NARF_PROT_MAX_MSG_DATA_SIZE || res_lenght < 0)
        return NARF_CLIENT_ERROR_MSG_SIZE_C;
    
    // read cmd
    bytes_read = read(this->sock_fd, &cmd, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return NARF_CLIENT_ERROR_TIMEOUT_C;

    // read data only if there is data in msg
    if(res_lenght > 0)
    {
        bytes_read = read(this->sock_fd, res_data, res_lenght);
        if(bytes_read != res_lenght)
            return NARF_CLIENT_ERROR_TIMEOUT_C;
    }

    return cmd;  
}

uint8_t NarfWirelessProtocolClient::generatePacketNumber()
{
    // new packet
    this->packet_num++;

    // packet number goes from 1-255/{0xAC}!!
    if(this->packet_num == 0 || this->packet_num == 0xAC)
        this->packet_num++;
    
    return this->packet_num;
}