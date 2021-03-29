#include <tins/tins.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

using namespace Tins;

struct Arguments{
    std::string interface;

};


void *receiver_thread(void * data){
    Arguments* arguments = ((Arguments *)data);
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("wlan addr2 dc:68:eb:b2:cd:03");
    config.set_rfmon(true);
    Sniffer sniffer(arguments->interface, config);
    for (auto &packet : sniffer){
        auto radiotap = packet.pdu()->find_pdu<RadioTap>();
        auto data = radiotap->serialize();
        data.data();
        std::cout << "Data!" << std::endl;
    }
}

void *sender_thread(void * data){
    //std::cout << "Doing it\n";
    Arguments* arguments = ((Arguments *)data);
    /*Dot11Beacon beacon;
    beacon.addr1(Dot11::BROADCAST);
    beacon.addr2("69:42:00:13:37:39");
    beacon.addr3(beacon.addr2());
    beacon.ssid("NANI");
    beacon.ds_parameter_set(1);
    beacon.supported_rates({1.0f, 5.5f, 11.0f});
    //beacon.rsn_information(RSNInformation::wpa2_psk());
    PacketSender sender(arguments->interface);
    RadioTap radio = RadioTap() / beacon;
    while (1)
    {
        sender.send(radio);
    }*/
    
}
 
//224.0.39.69
//1420

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <monitor interface>" << std::endl;
        return 1;
    }
    Arguments arguments;
    arguments.interface = argv[1];
    pthread_t sender_t;
    pthread_t receiver_t;
    std::cout << "Creating threads" << std::endl;
    pthread_create(&sender_t, NULL, sender_thread, &arguments);
    pthread_create(&receiver_t, NULL, receiver_thread, &arguments);
    pthread_join(sender_t, NULL);
    pthread_join(receiver_t, NULL);
    return 0;
}