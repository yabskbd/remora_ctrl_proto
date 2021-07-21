
# Takes a candump file (recorded using candump -l <interface>), and replays
# it over a virtual CAN interface (for now, hardcoded to assume dump was taken
# on can1 interface, and will replay on vcan1 interface)

sudo modprobe vcan
sudo ifconfig vcan1 down
sudo ip link delete vcan1
sudo ip link add dev vcan1 type vcan
sudo ifconfig vcan1 up
canplayer vcan1=can1 -I candump-2021-07-21_220402.log

