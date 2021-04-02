#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
 
using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

using namespace Tins;
 
class BeaconSniffer {
public:
    void run(const string& iface);
private:
    typedef Dot11::address_type address_type;
    typedef set<address_type> ssids_type;
 
     
    ssids_type ssids;
};
 
void BeaconSniffer::run(const std::string& iface) {
    std::cout << iface << std::endl;
    PacketSender sender(iface);
    while (1)
    {
        Dot11Beacon beacon;
        // Make this a broadcast frame. Note that Dot11::BROADCAST
        // is just the same as "ff:ff:ff:ff:ff:ff"
        beacon.addr1(Dot11::BROADCAST);
        // We'll set the source address to some arbitrary address
        beacon.addr2("00:01:02:03:04:05");
        // Set the bssid, to the same one as above
        beacon.addr3(beacon.addr2());

        // Let's add an ssid option
        beacon.ssid("libtins");
        // Our current channel is 8
        beacon.ds_parameter_set(8);
        // This is our list of supported rates:
        beacon.supported_rates({ 1.0f, 5.5f, 11.0f });

        // Encryption: we'll say we use WPA2-psk encryption
        beacon.rsn_information(RSNInformation::wpa2_psk());



        RadioTap radio = RadioTap() / beacon;
        sender.send(radio);
    }
    
}
 
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " <<* argv << " <interface>" << endl;
        return 1;
    }
    string interface = argv[1];
    BeaconSniffer sniffer;
    sniffer.run(interface);
}