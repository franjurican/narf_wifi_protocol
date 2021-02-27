#include <narf_protocol/client.h>

NarfWirelessProtocolClient::NarfWirelessProtocolClient(std::string serverIPv4, uint16_t server_port) : serverIP(serverIPv4), 
                                                                server_port(server_port), head_rec_count(0), packet_num(0)
{}

NarfWirelessProtocolClient::~NarfWirelessProtocolClient()
{
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
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
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
}

uint8_t NarfWirelessProtocolClient::sendProtocolMsg(uint8_t cmd, short int lenght, uint8_t req_data[], short int &res_lenght, uint8_t res_data[])
{
    uint8_t pack_num = this->generatePacketNumber();
    uint8_t header[] = {0xAC, 0x46, 0x72, pack_num, 0x41, 0x6E, 0x4A, NARF_PROT_VER_MAX, NARF_PROT_VER_MIN, 0xDC};

    // send header, lenght and cmd
    write(this->sock_fd, header, sizeof(header));
    write(this->sock_fd, &lenght, sizeof(lenght));
    write(this->sock_fd, &cmd, sizeof(cmd));

    // send req data
    if(req_data != NULL && lenght > 0)
        write(this->sock_fd, req_data, lenght);

    return this->getResponseMsg(pack_num, res_lenght, res_data);
}

bool NarfWirelessProtocolClient::checkHeaderBody(uint8_t pack_num)
{
    uint8_t buff;
    int bytes_read;
    
    // for MAX recursive function calls!!
    if(this->head_rec_count == NARF_PROT_REC_HEAD_MAX)
        return false;
    else
        this->head_rec_count++;

    // protocol check bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0x46)
        return this->detectMsgHeader(pack_num);

    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0x72)
        return this->detectMsgHeader(pack_num);

    // packet number - CAN'T BE EQUAL TO START BITS!!!!
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != pack_num)
        return this->detectMsgHeader(pack_num);
    
    // protocol check bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0x41)
        return this->detectMsgHeader(pack_num);
    
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0x6E)
        return this->detectMsgHeader(pack_num);

    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0x4A)
        return this->detectMsgHeader(pack_num);
    
    // version max - CAN'T BE EQUAL TO START BITS!!!!
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != NARF_PROT_VER_MAX)
        return this->detectMsgHeader(pack_num);

    // version min - CAN'T BE EQUAL TO START BITS!!!!
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != NARF_PROT_VER_MIN)
        return this->detectMsgHeader(pack_num);

    // header end bytes
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));
    if(bytes_read != sizeof(uint8_t))
        return false;
    else if(buff == 0xAC)
        return this->checkHeaderBody(pack_num);
    else if(buff != 0xDC)
        return this->detectMsgHeader(pack_num);

    return true;
}

bool NarfWirelessProtocolClient::detectMsgHeader(uint8_t pack_num)
{
    uint8_t buff;
    int bytes_read;
    bool dummy;

    // read first byte
    bytes_read = read(this->sock_fd, &buff, sizeof(uint8_t));

    // detect header
    if(bytes_read <= 0 || this->head_rec_count == NARF_PROT_REC_HEAD_MAX)
    {
        this->head_rec_count = 0;
        return false;
    }
    else if(buff != 0xAC)
    {   
        this->head_rec_count++;
        return this->detectMsgHeader(pack_num);
    }
    else
    {
        dummy = this->checkHeaderBody(pack_num);
        this->head_rec_count = 0;
        return dummy;
    }
}

uint8_t NarfWirelessProtocolClient::getResponseMsg(uint8_t pack_num, short int &res_lenght, uint8_t res_data[])
{
    int read_bytes;
    uint8_t cmd;

    if(this->detectMsgHeader(pack_num))
    {
        // res data lenght
        read_bytes = read(sock_fd, &res_lenght, sizeof(res_lenght));
        if(read_bytes != sizeof(res_lenght))
            return NARF_RES_ERROR_TIMEOUT; 

        // res command
        read_bytes = read(sock_fd, &cmd, sizeof(cmd));
        if(read_bytes != sizeof(cmd))
            return NARF_RES_ERROR_TIMEOUT;

        // res data
        if(res_lenght > 0 && res_lenght < NARF_PROT_MAX_MSG_DATA_SIZE)
        {
            read_bytes = read(sock_fd, res_data, res_lenght);
            if(read_bytes != res_lenght)
                return NARF_RES_ERROR_TIMEOUT;
        }
        return cmd;
    }

    return NARF_RES_ERROR_HEADER;
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