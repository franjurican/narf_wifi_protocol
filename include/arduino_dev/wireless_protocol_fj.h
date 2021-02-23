#ifndef WIRELESS_PROTOCOL_FJ
#define WIRELESS_PROTOCOL_FJ

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <map> 
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include <NetworkManager.h>

namespace wireless 
{   
    // structs
    typedef struct struct_w {  
        std::string ssid;
        std::string bssid;
        int signalStrenght;
        float frequency;
        NM80211Mode deviceMode;
        std::string securityProtocol;
    } wireless_struct;

    typedef struct struct_info {
        NMDeviceType interfaceType;
        NMDeviceState connection;
        bool softwareDevice;
        std::string interfaceMAC;
        std::string interfaceAdress;
        int interfaceSpeed;
        std::shared_ptr<wireless_struct> ap;
    } interface_info;

    typedef struct struct_scan {
        std::string ssid;
        std::string bssid;
        int signalStrenght;
        float frequency;
        int maxSpeed;
        NM80211Mode accessPointMode;
        std::vector<std::string> securityProtocol;
    } scan_info;

    // containers
    typedef std::map<std::string, struct struct_info> interfaces_info;
    typedef std::vector<struct struct_scan> scan_results;
        
    // class for interfaces
    class InterfacesManipulation
    {
        public:
            /* constructor */
            InterfacesManipulation();

            /* destructor */ 
            ~InterfacesManipulation();

            /* get network ineterfaces info */
            interfaces_info getAllNetworkInterfaces(int domain = AF_INET);

            /* print network interfaces informations */
            void printNetworkInterfacesInfo(interfaces_info info);

            /* scan for AP */
            scan_results scanForAP(std::string wirelessInterfaceName); 

            /* print scan results */
            void printScanResults(scan_results scan);

            /* connect device to WLAN */
            bool connectDeviceToNetwork();

            /* disconnect */
            bool disconnectDeviceFromAP(std::string wirelessInterfaceName);

        private: 
            NMClient *client;

            /* private methods, fill request with ONLY interface name */
            std::string center(std::string input, int width);
            std::string getIPAddress(NMActiveConnection *active_conn, int domain);
            wireless_struct* getAPInfo(NMDeviceWifi *dev, NMActiveConnection *activ_conn);
            std::string interfaceTypeToString(NMDeviceType type);
            std::string connectionToString(NMDeviceState state);
            std::string deviceModeToString(NM80211Mode mode);   
            void accessPointSecurityToString(NM80211ApSecurityFlags mode, std::vector<std::string> &out, std::string name = "WPA");  
    }; // class InterfacesManipulation
} // namespace wireless

#endif // WIRELESS_PROTOCOL_FJ