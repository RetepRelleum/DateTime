#ifndef DATETIME_H
#define DATETIME_H

#include <WiFiUdp.h>

namespace dt
{

    class DateTimes
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
        String adr[4]={"ch.pool.ntp.org","ntp11.metas.ch","ntp12.metas.ch","ntp13.metas.ch"};
    public:
        void begin();
        int getYear();
        int getMonth();
        int getDay();
        int getHour();
        int getMin();
        int getSec();
        String getUTCTime();
        unsigned long getTimestamp();
        String getTimeString(unsigned long);
        DateTimes(/* args */);
        ~DateTimes();
    };
} // namespace dt
extern dt::DateTimes DateTime;
using namespace dt;

#endif