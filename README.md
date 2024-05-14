# lab_timer

# micro agent
start the micro agent as docker container

sudo docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:iron serial --dev /dev/ttyUSB0 -v6
