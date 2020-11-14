# mqttNEO
Arduino Ethernet with 5x8 NEOPixel Shield.
Brightness and color are sent via mqtt topics. The Brightness is defined by how many of the 40 Pixels are lit.
Randomly one Pixel is turned on while another is turned off, except the brightness changes then Pixels only get randomly turned on or off.

PubSubCLient fixed to this [commit][pubsubclient]. Place in `/usr/share/arduino/libraries/PubSubCLient`.

![animated demo gif](/demo.gif?raw=true)


[pubsubclient]: https://github.com/knolleary/pubsubclient/commit/bb101c58e8c1bd970f3b6cde58e31d77a8c0d56a
