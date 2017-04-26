# Library dependency
1. console
2. std
3. multitask
4. signal
5. protobuf
6. communication
7. twainet

# Install
- checkout this repository in path ~/Arduino/libraries
- execute thirdparty.sh
- compile your sketch in ArduinoIde

# Usage example:
``` cpp
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <twainet.h>

char* ssid = "<wifi name>";
char* pass = "<wifi password>";

void mainloop() {
    //TODO(): your code

    if(WiFi.status() == WL_CONNECTED && !status) {
      GetTwainetClient()->SetUserName("<user name>");
      GetTwainetClient()->SetPassword("<user password>");
      GetTwainetClient()->Connect("<server ip>", 0/*<server port>*/);
      status = true;
    }
    delay(150);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, pass);
}

```