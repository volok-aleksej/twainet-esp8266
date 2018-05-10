# Library dependency
1. console
2. std
3. multitask
4. signal
5. protobuf
6. communication
7. twainet
8. json
9. utils

# Install
- checkout this repository in path ~/Arduino/libraries
- execute prebuild.sh
- compile your sketch in ArduinoIde
- flash board

# Set parameters after flashing
- in console set parameters(wifi.ssid, wifi.pass, server.user, server.pass, server.ip, server.port)<br>
    sample:
    config set wifi.ssid myssid
- reset board

# Usage example:
``` cpp
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <twainet.h>

void mainloop() {
    //TODO(): your code
}

void usersetup() {
    //TODO(): your code
}

```
