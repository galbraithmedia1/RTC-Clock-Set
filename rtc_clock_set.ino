#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <Timezone.h>

const char* ssid = "";
const char* password = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0); // UTC offset set in the Timezone object later
RTC_DS3231 rtc;

// US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};  // UTC -6 hours
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};   // UTC -7 hours
Timezone usMT(usMDT, usMST);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Start the NTP client
  timeClient.begin();

  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Update the NTP client and set the RTC
  timeClient.update();
  time_t utc = timeClient.getEpochTime();
  time_t local = usMT.toLocal(utc);
  // rtc.adjust(DateTime(local));
}
void loop() {
  timeClient.update();
  time_t utc = timeClient.getEpochTime();
  time_t local = usMT.toLocal(utc);

  DateTime now = rtc.now();
  int hour12 = hour(local) % 12;
  hour12 = hour12 ? hour12 : 12; // convert hour '0' to '12' for readability

  Serial.print("Local time: ");
  Serial.print(hour12);  // Use hour12 instead of hour(local)
  Serial.print(':');
  Serial.print(minute(local));
  Serial.print(':');
  Serial.print(second(local));
  Serial.print(hour(local) < 12 ? " AM - " : " PM - ");  // AM/PM indicator
  Serial.print(day(local));
  Serial.print('/');
  Serial.print(month(local));
  Serial.print('/');
  Serial.println(year(local));

  delay(1000);
}
