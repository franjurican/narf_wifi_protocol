#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <narf_protocol/client.h>

#define ERROR(msg) {std::cout << msg << std::endl; std::cout << "ERRNO: " << errno << std::endl; exit(24);}

std::string interface = "wlp5s0";
std::string ip_arduino = "192.168.1.24";
std::string ip_pc = "192.168.1.25";
uint16_t port1_arduino = 12;

int main(int argc, char *argv[])
{
    int sock_fd, read_bytes, write_bytes, no_delay = 1;
    struct sockaddr_in adresa;
    
    // otvori socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd < 0)
        ERROR("Problem kod otvaranja socketa!")

    // iskljuci Nagle-ovo bufiranje
    if(setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &no_delay, sizeof(int)) < 0)
        ERROR("Problem kod ukljucivanja TCP_NODELAY!")

    // adresa arduina
    memset(&adresa, 0, sizeof(adresa));
    adresa.sin_family = AF_INET;
    adresa.sin_port = htons(port1_arduino);

    if(!inet_aton(ip_arduino.c_str(), &(adresa.sin_addr)))
        ERROR("Problem kod zadavanja IP adrese Arduina")

    // spoji se na server(arduino)
    if(connect(sock_fd, (struct sockaddr *)&adresa, sizeof(adresa)) < 0)
        ERROR("Problem prilikom spajanja na arduino!");

    std::cout << "Uspjesno spojen na arduino!" << std::endl;

    // salji header
    uint8_t header1[] = {0xAC, 0x46, 0x72, 0x41, 0x6E, 0x01, 0x00, 0xDC};
    uint8_t header2[8] = {0};

    write_bytes = write(sock_fd, header1, sizeof(header1));
    std::cout << "poslano bytova: " << write_bytes << std::endl;

    ////////////////////
    // posalji poruku //
    ////////////////////
    uint8_t cmd = 0x08;
    // std::string salji = "Fran ti si car";
    uint8_t data[1] = {8};
    short int lenght = sizeof(data);//salji.length() + 1;

    // salji duljinu podataka
    write_bytes = write(sock_fd, &lenght, sizeof(lenght));
    std::cout << "poslano bytova: " << write_bytes << std::endl;

    // salji cmd
    write_bytes = write(sock_fd, &cmd, sizeof(cmd));
    std::cout << "poslano bytova: " << write_bytes << std::endl;

    // salji podatke
    write_bytes = write(sock_fd, data, lenght);
    std::cout << "poslano bytova: " << write_bytes << std::endl;

    ///////////////////////
    // procitaj response //
    ///////////////////////
    read_bytes = read(sock_fd, header2, sizeof(header2));
    std::cout << "velicina header-a: " << read_bytes << ", verzija: " << (int)header2[5] << "." << (int)header2[6] << std::endl;

    read_bytes = read(sock_fd, &lenght, sizeof(lenght));
    std::cout << "velicina lenghta: " << read_bytes << ", lenght: " << lenght << std::endl;

    read_bytes = read(sock_fd, &cmd, sizeof(cmd));
    std::cout << "velicina cmd-a: " << read_bytes << ", cmd: " << (int)cmd << std::endl;

    read_bytes = read(sock_fd, data, sizeof(data));
    std::cout << "velicina data: " << read_bytes << ", PIN: " << (int)data[0] << std::endl;

    close(sock_fd);    
    return 0;
}