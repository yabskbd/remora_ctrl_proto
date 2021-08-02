#!/bin/bash
# Ensure password environment variables are set
# BEAGLEBONE_WIFI_PASSWORD: The password to login to the WiFi network advertised by this BeagleBone
# DEBIAN_USER_PASSWORD: The password to update the debian user to. Change from 'temppwd' to this
# ECHENEIDAE_WIFI_PASSWORD: The password of the Encheneidae SSID, which this BeagleBone will connect to if available
for ENV_VAR in BEAGLEBONE_WIFI_PASSWORD DEBIAN_USER_PASSWORD ECHENEIDAE_WIFI_PASSWORD; do
        if [[ -z "${!ENV_VAR}" ]]; then
                echo "Must set environment variable $ENV_VAR";
		exit 1;
        fi
done

# Ensure CAN script has been copied, along with dbc database file
if [[ ! -e canparse.py ]]; then
	echo "Must copy canparse.py script over to host"
	exit 1;
fi
if [[ ! -e remora.dbc ]]; then
	echo "Must copy remora.dbc file over to host"
	exit 1;
fi


# Generate a random name for the host, and update relevant files
echo "remorabone$(</proc/sys/kernel/random/uuid)" | sudo tee /etc/hostname > /dev/null
HOST_NAME=$(cat /etc/hostname) 
sudo sed -i "s/beaglebone/$HOST_NAME/g" /etc/hosts

# Update the SoftAP settings, so the BeagleBone advertises a unique SSID, accessible with the provided BEAGLEBONE_WIFI_PASSWORD
FIRST_PART_OF_HOST_NAME=$(echo $HOST_NAME | sed -e 's/remorabone\(.*\)-\(.*\)-\(.*\)-\(.*\)-\(.*\)$/\1/')
sudo sed -i 's/USE_PERSONAL_PASSWORD="BeagleBone"/USE_PERSONAL_PASSWORD="'$BEAGLEBONE_WIFI_PASSWORD'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_PERSONAL_SSID="BeagleBone"/USE_PERSONAL_SSID="RemoraBone'$FIRST_PART_OF_HOST_NAME'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_APPENDED_SSID=yes/USE_APPENDED_SSID=no/g' /etc/default/bb-wl18xx

# Add connmanctl settings, so we connect to WiFi automatically
echo "[global]
Name = Echeneidae
Description = Remora network services

[service_wifi_echeneidae]
Type = wifi
Name = Echeneidae
Passphrase = $ECHENEIDAE_WIFI_PASSWORD" | sudo tee /var/lib/connman/echeneidae.config > /dev/null

# Create CAN setup script
echo "#!/bin/bash
sudo config-pin p9.24 can
sudo config-pin p9.26 can
sudo config-pin p9.19 can
sudo config-pin p9.20 can
sudo ip link set can1 up type can bitrate 250000 listen-only on restart-ms 250
sudo ip link set can0 up type can bitrate 250000 listen-only on restart-ms 250
sudo ifconfig can1 up
sudo ifconfig can0 up" | sudo tee /usr/bin/beaglebone_can_setup.sh > /dev/null

sudo chmod +x /usr/bin/beaglebone_can_setup.sh

# Create systemd unit file to run CAN setup script on startup
echo "[Unit]
Description=Configure CAN buses on system startup

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_can_setup.sh

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/canconfigure.service > /dev/null

sudo ln -s /lib/systemd/system/canconfigure.service /etc/systemd/system/

# Create CAN logging script
echo "#!/bin/bash
candump -tA -L any >> /var/log/candump.log" | sudo tee /usr/bin/beaglebone_can_listen.sh > /dev/null

sudo chmod +x /usr/bin/beaglebone_can_listen.sh

# Create systemd unit file to run CAN logging script on startup, and restart on failure
echo "[Unit]
Description=Listens to all CAN buses and logs to a file
Requires=canconfigure.service
After=canconfigure.service

StartLimitBurst=10
StartLimitIntervalSec=3min

[Service]
Type=simple
Restart=on-failure
RestartSec=10s
ExecStart=/usr/bin/beaglebone_can_listen.sh

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/canlog.service > /dev/null

sudo ln -s /lib/systemd/system/canlog.service /etc/systemd/system/

sudo systemctl enable canconfigure
sudo systemctl enable canlog

# Install python dependencies globally
python3 -m pip install wheel
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools

# Install canparse script
sudo mv canparse.py /usr/local/bin
sudo mv remora.dbc /usr/local/bin

echo "#!/bin/bash
python3 /usr/local/bin/canparse.py \"\$@\"" | sudo tee /usr/local/bin/canparse > /dev/null

sudo chmod +x /usr/local/bin/canparse

# Use handy BeagleBone script to partition disk to full size of SD card
sudo /opt/scripts/tools/grow_partition.sh

# Update password from default 'temppwd' to provided DEBIAN_USER_PASSWORD
echo "debian:$DEBIAN_USER_PASSWORD" | sudo chpasswd
echo "Successfully setup $HOST_NAME"
