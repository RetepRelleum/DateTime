

#include <DateTime.h>

String url = "";
unsigned long timeOut = 0;
unsigned long sTime = 0;
long timeSendData = -60000;

WiFiUDP Udp;

DateTime::DateTime(/* args */)
{
}

DateTime::~DateTime()
{
}

void DateTime::begin()
{
  ntpTime = getTime();
  timestamp = millis();
  Serial.println(unixtToString(ntpTime));
}

String DateTime::unixtToString(uint32_t t)
{
  t = isSommer(t) ? t + 2 * 3600 : t + 3600;
  int wochentag = ((t + 345600) % (604800)) / 86400;

  data[5] = t % 60;
  t = t / 60;
  data[4] = t % 60;
  t = t / 60;
  data[3] = t % 24;
  int h = t % 24;
  t = t / 24;

  unsigned yr = t / 365 + 1970;
  t = t % 365; // Year
  unsigned ly;
  for (ly = 1972; ly < yr; ly += 4)
  {
    if (!(ly % 100) && (ly % 400))
      continue;
    --t;
  }
  if (t < 0)
    t += 365, ++yr;
  static uint8_t const dm[2][12] = {
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
      {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
  int day = t;
  int month = 0;
  ly = (yr == ly) ? 1 : 0;
  while (day >= dm[ly][month])
    day -= dm[ly][month++];
  data[2] = day + 1;
  data[1] = month + 1;
  data[0] = yr;
  return String(data[0]) + "-" + (data[1] <= 9 ? "0" + String(data[1]) : String(data[1])) + "-" + (data[2] <= 9 ? "0" + String(data[2]) : String(data[2])) + "T" + (data[3] <= 9 ? "0" + String(data[3]) : String(data[3])) + ":" + (data[4] <= 9 ? "0" + String(data[4]) : String(data[4])) + ":" + (data[5] <= 9 ? "0" + String(data[5]) : String(data[5]));
}

bool DateTime::isSommer(uint32_t t)
{
  int wochentag = ((t + 345600) % (604800)) / 86400;
  t = t / 3600;
  int h = t % 24;
  t = t / 24;
  unsigned yr = t / 365 + 1970;
  t = t % 365; // Year
  unsigned ly;
  for (ly = 1972; ly < yr; ly += 4)
  {
    if (!(ly % 100) && (ly % 400))
      continue;
    --t;
  }
  if (t < 0)
    t += 365, ++yr;
  static uint8_t const dm[2][12] = {
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
      {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
  int day = t;
  int month = 0;
  ly = (yr == ly) ? 1 : 0;
  while (day >= dm[ly][month])
    day -= dm[ly][month++];

  bool sommer = false;
  if (month > 2 && month < 9)
    sommer = true;
  if ((month == 2) && (31 - day + wochentag < 7) && (wochentag > 0))
    sommer = true;
  if ((month == 2) && (31 - day + wochentag < 7) && (wochentag == 0) && (h > 0))
    sommer = true;
  if ((month == 9) && (31 - day + wochentag > 7))
    sommer = true;
  if ((month == 9) && (31 - day + wochentag < 7) && (wochentag == 0) && (h < 1))
    sommer = true;
  return sommer;
}

unsigned long DateTime::getTime()
{
  const int NTP_PACKET_SIZE = 48;
  char packetBuffer[50];
  unsigned int localPort = 2390; // local port to listen for UDP packets

  Udp.begin(localPort);

  sendNTPpacket(packetBuffer, NTP_PACKET_SIZE); // send an NTP packet to a time server
  delay(500);
  if (Udp.parsePacket())
  {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    return (epoch);
  }
  return 0;
}

void DateTime::sendNTPpacket(char *packetBuffer, const int NTP_PACKET_SIZE)
{
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  char adr[] = "ch.pool.ntp.org";
  Udp.beginPacket(adr, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write((uint8_t *)packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}

int DateTime::getYear()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[0];
}

int DateTime::getMonth()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[1];
};
int DateTime::getDay()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[2];
};
int DateTime::getHour()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[3];
};
int DateTime::getMin()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[4];
};
int DateTime::getSec()
{
  unixtToString(ntpTime + timestamp / 1000);
  return data[5];
};
