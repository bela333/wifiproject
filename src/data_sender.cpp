#include <tins/tins.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace Tins;

struct Arguments{
    std::string interface;
    std::string switch_mac;
    int socket;
};


void *receiver_thread(void * data){
    Arguments* arguments = ((Arguments *)data);
    Dot11Beacon beacon;
    beacon.addr1(Dot11::BROADCAST);
    beacon.addr2("69:42:00:13:37:39");
    beacon.addr3(beacon.addr2());
    beacon.ssid("NANI");
    beacon.ds_parameter_set(1);
    beacon.supported_rates({1.0f, 5.5f, 11.0f});
    //beacon.rsn_information(RSNInformation::wpa2_psk());
    PacketSender sender(arguments->interface);
    RadioTap radio = RadioTap() / beacon;
    while (1){
        auto data = radio.serialize();
        sockaddr_in localSock = {};
        localSock.sin_family = AF_INET;
        localSock.sin_port = htons(1420);
        localSock.sin_addr.s_addr = inet_addr("192.168.193.138");
        sendto(arguments -> socket, data.data(), data.size(), NULL, (sockaddr*)&localSock, sizeof(localSock));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //std::cout << "Received data!" << std::endl;
    }
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
    int loop = 1;
    if(setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) <0)
        throw "Error!";

    arguments.socket = sock;

    pthread_t receiver_t;
    std::cout << "Creating threads" << std::endl;
    pthread_create(&receiver_t, NULL, receiver_thread, &arguments);
    pthread_join(receiver_t, NULL);
    return 0;
}