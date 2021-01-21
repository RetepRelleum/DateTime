#ifndef DATETIME_H
#define DATETIME_H

#include <WiFiUdp.h>

class DateTime
{
private:
    WiFiUDP Udp;
    void sendNTPpacket(char *packetBuffer, const int NTP_PACKET_SIZE);
    bool isSommer(uint32_t t);
    String unixtToString(uint32_t t);
    unsigned long getTime();
    unsigned long timestamp;
    unsigned long ntpTime;
      int data[6] = {0, 0, 0, 0, 0, 0};

public:
    void begin();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMin();
    int getSec();

    DateTime(/* args */);
    ~DateTime();
};

#endif