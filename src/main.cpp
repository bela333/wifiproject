#include <tins/tins.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

using namespace Tins;

struct Arguments{
    std::string interface;
    std::string switch_mac;
    int socket;
};


void *receiver_thread(void * data){
    Arguments* arguments = ((Arguments *)data);
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("wlan addr2 " + arguments -> switch_mac);
    std::cout << "wlan addr2 " + arguments -> switch_mac << std::endl;
    config.set_rfmon(true);
    Sniffer sniffer(arguments->interface, config);
    for (auto &packet : sniffer){
        auto radiotap = packet.pdu()->find_pdu<RadioTap>();
        auto data = radiotap->serialize();
        sockaddr_in localSock = {};
        localSock.sin_family = AF_INET;
        localSock.sin_port = htons(1420);
        localSock.sin_addr.s_addr = inet_addr("224.0.39.69");
        sendto(arguments -> socket, data.data(), data.size(), NULL, (sockaddr*)&localSock, sizeof(localSock));
        std::cout << "Asd" << std::endl;
    }
}

void *sender_thread(void * data){
    Arguments* arguments = ((Arguments *)data);
    PacketSender sender(arguments->interface);
    

    std::array<uint8_t, 1024> arr;
    arr.fill(0);
    while (1)
    {
        sockaddr_in localSock = {};
        localSock.sin_family = AF_INET;
        localSock.sin_port = htons(1420);
        localSock.sin_addr.s_addr = INADDR_ANY;
        socklen_t len = sizeof(localSock);
        auto c = recvfrom(arguments -> socket, arr.data(), arr.size(), NULL, (sockaddr*)&localSock, &len);
        auto packet = RadioTap(arr.data(), c);
        sender.send(packet);
    }
    
    
}
 
//224.0.39.69
//1420

int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <monitor interface> <switch mac address> <interface address>" << std::endl;
        return 1;
    }
    Arguments arguments;
    arguments.interface = argv[1];
    arguments.switch_mac = argv[2];
    auto interface_address = inet_addr(argv[3]);

    auto sock = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse_addr = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0)
        throw "Error!";
    
    sockaddr_in localSock = {};
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(1420);
    localSock.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (sockaddr*)&localSock, sizeof(localSock));

    ip_mreq group = {};
    group.imr_multiaddr.s_addr = inet_addr("224.0.39.69");
    group.imr_interface.s_addr = htons(0);
    if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0)
        throw "Error!";
    
    if(setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &interface_address, sizeof(interface_address)) < 0)
        throw "Error!";

    arguments.socket = sock;

    pthread_t sender_t;
    pthread_t receiver_t;
    std::cout << "Creating threads" << std::endl;
    pthread_create(&sender_t, NULL, sender_thread, &arguments);
    pthread_create(&receiver_t, NULL, receiver_thread, &arguments);
    pthread_join(sender_t, NULL);
    pthread_join(receiver_t, NULL);
    return 0;
}