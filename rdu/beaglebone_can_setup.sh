# First, download the latest image from here https://beagleboard.org/latest-images (non-TIDL)
# Plug in SD card to computer, and upload the .img.xz file to it, using Balena Etcher
# Plug SD card into board, then plug board into computer. Put the BeagleBone Comms Shield onto the device, and properly plug into CAN_High and CAN_Low. Once Windows recognizes the device, you can run 'ssh debian@beaglebone.local`, with password "temppwd". Then, on the BeagleBone, run the below, and if you are connected to an active CAN bus, you should see data print out
sudo config-pin p9.24 can
sudo config-pin p9.26 can
sudo ip link set can1 up type can bitrate 500000
sudo ifconfig can1 up
candump can1
