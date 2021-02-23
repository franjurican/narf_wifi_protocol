#include <arduino_dev/wireless_protocol_fj.h>

using namespace wireless;

InterfacesManipulation::InterfacesManipulation()
{    
    // network manager d-bus connection
    this->client = nm_client_new(NULL, NULL);

    if(this->client == NULL)
        throw std::string("Error: opening D-Bus connection to Network Manager!");
}

InterfacesManipulation::~InterfacesManipulation()
{}

interfaces_info InterfacesManipulation::getAllNetworkInterfaces(int domain)
{
    NMDevice *dev;
    NMActiveConnection *active_conn;
    std::string interface_name;
    interfaces_info out;
    const GPtrArray *devices;

    // get devices
    devices = nm_client_get_devices(client);

    // get info from devices
    for(int i = 0; i < devices->len; i++)
    {    
        dev = (NMDevice *)g_ptr_array_index(devices, i);
        interface_name = nm_device_get_iface(dev);
        active_conn = nm_device_get_active_connection(dev);

        // interface general info
        out[interface_name].interfaceType = nm_device_get_device_type(dev);
        out[interface_name].connection = nm_device_get_state(dev);
        out[interface_name].softwareDevice = nm_device_is_software(dev);
        out[interface_name].interfaceMAC = nm_device_get_hw_address(dev);  
        out[interface_name].interfaceAdress = out[interface_name].softwareDevice ? "" : this->getIPAddress(active_conn, domain);     

        // device specific info
        if(out[interface_name].interfaceType == NM_DEVICE_TYPE_WIFI)
        {
            out[interface_name].interfaceSpeed = round(nm_device_wifi_get_bitrate((NMDeviceWifi *)dev) / 1000.0);
            out[interface_name].ap.reset(this->getAPInfo((NMDeviceWifi *)dev, active_conn));
        }
        else if(out[interface_name].interfaceType == NM_DEVICE_TYPE_ETHERNET)
        {
            out[interface_name].interfaceSpeed = nm_device_ethernet_get_speed((NMDeviceEthernet *)dev);
        }
    }
    return out;
}

void InterfacesManipulation::printNetworkInterfacesInfo(interfaces_info info)
{
    int table1_lenght = 125 + 7;
    int table2_lenght = 124 + 8;

    // interfaces general info - header
    std::cout << std::string(table1_lenght, '=') << std::endl;
    std::cout << "||" << center("Interfaces - general info", table1_lenght - 4) << "||" << std::endl;
    std::cout << std::string(table1_lenght, '=') << std::endl;

    // interfaces general info - content
    std::cout << "|" << center("Name", 15) << "|";
    std::cout << center("Interface type", 23) << "|";
    std::cout << center("MAC", 21) << "|";
    std::cout << center("Connection status", 21) << "|";
    std::cout << center("IP adress", 31) << "|" ;
    std::cout << center("Speed [Mb/s]", 14) << "|" << std::endl; 
    std::cout << std::string(table1_lenght, '=') << std::endl;

    // interfaces general info - data
    for(auto i = info.cbegin(); i != info.cend(); i++)
    {
        std::cout << "|" << center(i->first, 15) << "|";
        std::cout << center(this->interfaceTypeToString(i->second.interfaceType) + (i->second.softwareDevice ? "/software" : "/hardware"), 23) << "|";
        std::cout << center(i->second.interfaceMAC, 21) << "|";
        std::cout << center(this->connectionToString(i->second.connection), 21) << "|";
        std::cout << (i->second.interfaceAdress.empty() ? center("-", 31) : center(i->second.interfaceAdress, 31)) << "|";
        std::cout << (i->second.interfaceSpeed == 0 ? center("-", 14): center(std::to_string(i->second.interfaceSpeed), 14)) << "|" << std::endl;
        std::cout << std::string(table1_lenght, '-') << std::endl;
    }

    // wireless interfaces AP info - header
    std::cout << std::endl;
    std::cout << std::string(table2_lenght, '=') << std::endl;
    std::cout << "||" << center("Wireless interfaces - connection/AP info", table2_lenght - 4) << "||" << std::endl;
    std::cout << std::string(table2_lenght, '=') << std::endl;

    // wireless interfaces AP info - content
    std::cout << "|" << center("Name", 15) << "|";
    std::cout << center("Device mode", 16) << "|";
    std::cout << center("SSID", 23) << "|";
    std::cout << center("BSSID", 21) << "|";
    std::cout << center("Signal strenght [%]", 21) << "|";
    std::cout << center("Frequency [GHz]", 17) << "|";
    std::cout << center("Security", 11) << "|" << std::endl;
    std::cout << std::string(table2_lenght, '=') << std::endl;

    // wireless interfaces AP info - data
    for(auto i = info.cbegin(); i != info.cend(); i++)
    {   
        if(i->second.interfaceType == NM_DEVICE_TYPE_WIFI)
        {
            std::cout << "|" << center(i->first, 15) << "|";
            std::cout << center(this->deviceModeToString(i->second.ap->deviceMode), 16) << "|";
            std::cout << (i->second.ap->ssid.empty() ? center("-", 23) : center(i->second.ap->ssid, 23)) << "|";
            std::cout << (i->second.ap->bssid.empty() ? center("-", 21) : center(i->second.ap->bssid, 21)) << "|";
            std::cout << (i->second.ap->signalStrenght == 0 ? center("-", 21) : center(std::to_string(i->second.ap->signalStrenght), 21)) << "|";
            std::cout << (i->second.ap->frequency == 0 ? center("-", 17) : center(std::to_string(i->second.ap->frequency), 17)) << "|";
            std::cout << (i->second.ap->securityProtocol.empty() ? center("-", 11) : center(i->second.ap->securityProtocol, 11)) << "|" << std::endl;
            std::cout << std::string(table2_lenght, '-') << std::endl;
        }
    }
}

scan_results InterfacesManipulation::scanForAP(std::string wirelessInterfaceName)
{   
    scan_info ap_info;
    scan_results out;
    NMDevice *dev;
    NMAccessPoint *ap;
    gsize len_ssid;
    GBytes *ssid;
    const GPtrArray *accessPoints;
    const char *bssid;

    // get wireless device
    dev = nm_client_get_device_by_iface(this->client, wirelessInterfaceName.c_str());
    if(dev == NULL)
        return out;
    
    // check if interface is wireless
    if(nm_device_get_device_type(dev) != NM_DEVICE_TYPE_WIFI)
        return out;
    
    // get scan results
    accessPoints = nm_device_wifi_get_access_points((NMDeviceWifi *)dev);
    if(accessPoints == NULL)
        return out;
    
    // get info for each AP
    for(int i = 0; i < accessPoints->len; i++)
    {
        ap = (NMAccessPoint *)g_ptr_array_index(accessPoints, i);

        // ssid
        ssid = nm_access_point_get_ssid(ap);
        if(ssid == NULL)
            ap_info.ssid = "";
        else
        {
            len_ssid = g_bytes_get_size(ssid);
            ap_info.ssid = (len_ssid > 0) ? (char *)g_bytes_get_data(ssid, &len_ssid) : "";
        }

        // other info
        bssid = nm_access_point_get_bssid(ap);
        ap_info.bssid = (bssid == NULL) ? "" : bssid;
        ap_info.signalStrenght = nm_access_point_get_strength(ap);
        ap_info.frequency = nm_access_point_get_frequency(ap) / 1000.0;
        ap_info.maxSpeed = round(nm_access_point_get_max_bitrate(ap) / 1000.0); 
        ap_info.accessPointMode = nm_access_point_get_mode(ap);

        // security
        ap_info.securityProtocol.clear();
        this->accessPointSecurityToString(nm_access_point_get_wpa_flags(ap), ap_info.securityProtocol, "WPA");
        this->accessPointSecurityToString(nm_access_point_get_rsn_flags(ap), ap_info.securityProtocol, "WPA2");
        out.push_back(ap_info);
    }

    return out;
}

void InterfacesManipulation::printScanResults(scan_results scan)
{
    int table_lenght = 117 + 7;
    int height;

    // wireless interfaces AP info - header
    std::cout << std::string(table_lenght, '=') << std::endl;
    std::cout << "||" << center("Scan results - AP info", table_lenght - 4) << "||" << std::endl;
    std::cout << std::string(table_lenght, '=') << std::endl;

    // wireless interfaces AP info - content
    std::cout << "|" << center("SSID", 23) << "|";
    std::cout << center("AP mode", 16) << "|";
    std::cout << center("BSSID", 21) << "|";
    std::cout << center("Signal strenght [%]", 21) << "|";
    std::cout << center("Frequency [GHz]", 17) << "|";
    std::cout << center("Security", 19) << "|" << std::endl;
    std::cout << std::string(table_lenght, '=') << std::endl;

    // wireless interfaces AP info - data
    for(auto i = scan.cbegin(); i != scan.cend(); i++)
    {   
        height = i->securityProtocol.size();

        if(height == 0)
        {
            std::cout << "|" << (i->ssid.empty() ? center("-", 23) : center(i->ssid, 23)) << "|";
            std::cout << center(this->deviceModeToString(i->accessPointMode), 16) << "|";
            std::cout << (i->bssid.empty() ? center("-", 21) : center(i->bssid, 21)) << "|";
            std::cout << (i->signalStrenght == 0 ? center("-", 21) : center(std::to_string(i->signalStrenght), 21)) << "|";
            std::cout << (i->frequency == 0 ? center("-", 17) : center(std::to_string(i->frequency), 17)) << "|";
            std::cout << center("-", 19) << "|" << std::endl;
        }
        else
        {
            for(int k = 0; k < height; k++)
            {
                if(k == 0)
                {
                    std::cout << "|" << (i->ssid.empty() ? center("-", 23) : center(i->ssid, 23)) << "|";
                    std::cout << center(this->deviceModeToString(i->accessPointMode), 16) << "|";
                    std::cout << (i->bssid.empty() ? center("-", 21) : center(i->bssid, 21)) << "|";
                    std::cout << (i->signalStrenght == 0 ? center("-", 21) : center(std::to_string(i->signalStrenght), 21)) << "|";
                    std::cout << (i->frequency == 0 ? center("-", 17) : center(std::to_string(i->frequency), 17)) << "|";
                    std::cout << center(i->securityProtocol[k], 19) << "|" << std::endl;
                }
                else
                {
                    std::cout << "|" << center("", 23) << "|";
                    std::cout << center("", 16) << "|";
                    std::cout << center("", 21) << "|";
                    std::cout << center("", 21) << "|";
                    std::cout << center("", 17) << "|";
                    std::cout << center(i->securityProtocol[k], 19) << "|" << std::endl;  
                }
            }
        }
        std::cout << std::string(table_lenght, '-') << std::endl;
    }
}

bool InterfacesManipulation::connectDeviceToNetwork()
{

}

bool InterfacesManipulation::disconnectDeviceFromAP(std::string wirelessInterfaceName)
{
    NMDevice *dev = nm_client_get_device_by_iface(this->client, wirelessInterfaceName.c_str());
    
    if(dev == NULL)
        return false;
    else
        return nm_device_disconnect(dev, NULL, NULL);
}

std::string InterfacesManipulation::center(std::string input, int width)
{
    int str_half = input.length() / 2, width_half = width / 2;

    if(str_half > width_half)
        return input;
    else
        return std::string(width_half - str_half, ' ') + input + std::string(width - width_half + str_half - input.length(), ' ');
}

std::string InterfacesManipulation::getIPAddress(NMActiveConnection *active_conn, int domain)
{
    NMIPConfig *ip_config;
    NMIPAddress *ip_adress;
    const char *ip_string;
    GPtrArray *adress_array;

    if(active_conn == NULL)
        return "";

    if(domain == AF_INET)
        ip_config = nm_active_connection_get_ip4_config(active_conn);
    else if(domain == AF_INET6)
        ip_config = nm_active_connection_get_ip6_config(active_conn);
    else
        return "";
            
    if(ip_config == NULL)
        return "";

    adress_array = nm_ip_config_get_addresses(ip_config);
    if(adress_array == NULL )
        return "";

    ip_adress = (NMIPAddress *)g_ptr_array_index(adress_array, 0);
    if(ip_adress == NULL)
        return "";

    ip_string = nm_ip_address_get_address(ip_adress);
    if(ip_string == NULL)
        return "";
    else
        return ip_string;
}

wireless_struct* InterfacesManipulation::getAPInfo(NMDeviceWifi *dev, NMActiveConnection *active_conn)
{
    NMAccessPoint *ap;
    NMConnection *conn;
    NMSettingWirelessSecurity *wifi_sec;
    wireless_struct *ws;
    GBytes *ssid;
    gsize len_ssid;
    const char *bssid, *key_mgmt;
    std::string dummy;

    // connection
    conn = (NMConnection *)nm_active_connection_get_connection(active_conn);

    // AP info
    ap = nm_device_wifi_get_active_access_point(dev);
    ws = new wireless_struct;
    if(ap != NULL)
    {
        // ssid
        ssid = nm_access_point_get_ssid(ap);
        if(ssid == NULL)
            ws->ssid = "";
        else
        {
            len_ssid = g_bytes_get_size(ssid);
            ws->ssid = (len_ssid > 0) ? (char *)g_bytes_get_data(ssid, &len_ssid) : "";
        }

        //other info
        bssid = nm_access_point_get_bssid(ap);
        ws->bssid = (bssid == NULL) ? "" : bssid;
        ws->signalStrenght = nm_access_point_get_strength(ap);
        ws->frequency = nm_access_point_get_frequency(ap) / 1000.0; 
        ws->deviceMode = nm_device_wifi_get_mode(dev);

        // wifi security
        wifi_sec = nm_connection_get_setting_wireless_security(conn);
        key_mgmt = nm_setting_wireless_security_get_key_mgmt(wifi_sec);
        if(key_mgmt != NULL)
        {
            dummy = key_mgmt;
            if(dummy == "none" || dummy == "ieee8021x")
                ws->securityProtocol = "WEP";
            else
                ws->securityProtocol = dummy;
        }
        else
            ws->securityProtocol = "open";     

    }
    else
    {
        ws->ssid = "";
        ws->bssid = "";
        ws->signalStrenght = 0;
        ws->frequency = 0;
        ws->deviceMode = NM_802_11_MODE_UNKNOWN;
        ws->securityProtocol = "";
    }

    return ws;
}

std::string InterfacesManipulation::interfaceTypeToString(NMDeviceType type)
{
    switch (type)
    {
        case NM_DEVICE_TYPE_UNKNOWN:
            return "unknown";
        case NM_DEVICE_TYPE_ETHERNET:
            return "ethernet";
        case NM_DEVICE_TYPE_WIFI:
            return "wi-fi";
        case NM_DEVICE_TYPE_UNUSED1:
            return "unused";
        case NM_DEVICE_TYPE_UNUSED2:
            return "unused";
        case NM_DEVICE_TYPE_BT:
            return "bluetooth";
        case NM_DEVICE_TYPE_OLPC_MESH:
            return "olpc mesh";
        case NM_DEVICE_TYPE_WIMAX:
            return "wimax";
        case NM_DEVICE_TYPE_MODEM:
            return "modem";
        case NM_DEVICE_TYPE_INFINIBAND:
            return "infiniband";
        case NM_DEVICE_TYPE_BOND:
            return "bond";
        case NM_DEVICE_TYPE_VLAN:
            return "vlan";
        case NM_DEVICE_TYPE_ADSL:
            return "adsl";
        case NM_DEVICE_TYPE_BRIDGE:
            return "bridge";
        case NM_DEVICE_TYPE_GENERIC:
            return "generic";
        case NM_DEVICE_TYPE_TEAM:
            return "team";
        case NM_DEVICE_TYPE_TUN:
            return "tun";
        case NM_DEVICE_TYPE_IP_TUNNEL:
            return "ip tunnel";
        case NM_DEVICE_TYPE_MACVLAN:
            return "macvlan";
        case NM_DEVICE_TYPE_VXLAN:
            return "vxlan";
        case NM_DEVICE_TYPE_VETH:
            return "veth";
        case NM_DEVICE_TYPE_MACSEC:
            return "macsec";
        case NM_DEVICE_TYPE_DUMMY:
            return "dummy";
        case NM_DEVICE_TYPE_PPP:
            return "ppp";
        case NM_DEVICE_TYPE_OVS_INTERFACE:
            return "ovs iface";
        case NM_DEVICE_TYPE_OVS_PORT:
            return "ovs port";
        case NM_DEVICE_TYPE_OVS_BRIDGE:
            return "ovs bridge";    
        default:
            return "";
    }
}

std::string InterfacesManipulation::connectionToString(NMDeviceState state)
{
    switch (state)
    {
        case NM_DEVICE_STATE_UNKNOWN:
            return "unknown";
        case NM_DEVICE_STATE_UNMANAGED:
            return "unmanaged";
        case NM_DEVICE_STATE_UNAVAILABLE:
            return "unavailable";
        case NM_DEVICE_STATE_DISCONNECTED:
            return "disconnected";
        case NM_DEVICE_STATE_PREPARE:
            return "preparing";
        case NM_DEVICE_STATE_CONFIG:
            return "configuring";
        case NM_DEVICE_STATE_NEED_AUTH:
            return "need auth";
        case NM_DEVICE_STATE_IP_CONFIG:
            return "ip configuring";
        case NM_DEVICE_STATE_IP_CHECK:
            return "ip check";
        case NM_DEVICE_STATE_SECONDARIES:
            return "secondaries";
        case NM_DEVICE_STATE_ACTIVATED:
            return "connected";
        case NM_DEVICE_STATE_DEACTIVATING:
            return "disconnecting";
        case NM_DEVICE_STATE_FAILED:
            return "failed";
        default:
            return "";
    }
}

std::string InterfacesManipulation::deviceModeToString(NM80211Mode mode)
{
    switch(mode)
    {
        case NM_802_11_MODE_UNKNOWN:
            return "-";
        case NM_802_11_MODE_ADHOC:
            return "ad-hoc";
        case NM_802_11_MODE_INFRA:
            return "infrastructure";
        case NM_802_11_MODE_AP:
            return "access point";
        default:
            return "-";
    }
}

void InterfacesManipulation::accessPointSecurityToString(NM80211ApSecurityFlags mode, std::vector<std::string> &out, std::string name)
{   
    std::string key_mgmt = "";
    std::string chiper = "";

    // key managment
    if(mode & NM_802_11_AP_SEC_KEY_MGMT_PSK)
        key_mgmt = "-PSK";
    else if(mode & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
        key_mgmt = "-IEEE802.1x";
    
    // chiper
    if(mode & NM_802_11_AP_SEC_GROUP_CCMP || mode & NM_802_11_AP_SEC_PAIR_CCMP)
        out.push_back(name + "(AES)" + key_mgmt);
    
    if(mode & NM_802_11_AP_SEC_GROUP_TKIP || mode & NM_802_11_AP_SEC_PAIR_TKIP)
       out.push_back(name + "(TKIP)" + key_mgmt);
    
    if(mode & NM_802_11_AP_SEC_GROUP_WEP40 || mode & NM_802_11_AP_SEC_PAIR_WEP40)
        out.push_back(name + "(WEP40)" + key_mgmt);

    if(mode & NM_802_11_AP_SEC_GROUP_WEP104 || mode & NM_802_11_AP_SEC_PAIR_WEP104)
        out.push_back(name + "(WEP104)" + key_mgmt);
    
    // open connection
    if(mode & NM_802_11_AP_SEC_NONE)
        out.push_back("open");
}
