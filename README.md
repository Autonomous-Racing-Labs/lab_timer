# lab_timer

## Dot Matrix Display (DMD)
This display is used to display the current lap time.
The display type is called HUB12 because it has only white leds.
### Useful links
- [HUB explanation](https://olympianled.com/led-hub-pinout-diagrams/)
- [Currently used matrix panel driver lib](https://github.com/Qudor-Engineer/DMD32/tree/main) KERNEL-VERSION NEEDS TO be @ 2.0.2
- [Alternative driver](https://github.com/adafruit/Adafruit_Protomatter/tree/master)
- [Pinout of the used DMD](https://cdn.shopify.com/s/files/1/0045/8932/files/DMDCON_DMDConnector.pdf?100730)
- ["User Manual"](https://cdn-reichelt.de/documents/datenblatt/A300/AD026_EN.pdf)
# micro agent
start the micro agent as docker container

sudo docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:iron serial --dev /dev/ttyUSB0 -v6
