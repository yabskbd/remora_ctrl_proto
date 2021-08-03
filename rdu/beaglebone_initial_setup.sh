#!/bin/bash
# Ensure password environment variables are set
# BEAGLEBONE_WIFI_PASSWORD: The password to login to the WiFi network advertised by this BeagleBone
# DEBIAN_USER_PASSWORD: The password to update the debian user to. Change from 'temppwd' to this
# ECHENEIDAE_WIFI_PASSWORD: The password of the Encheneidae SSID, which this BeagleBone will connect to if available
for ENV_VAR in BEAGLEBONE_WIFI_PASSWORD DEBIAN_USER_PASSWORD ECHENEIDAE_WIFI_PASSWORD DIGITAL_OCEAN_ACCESS_KEY DIGITAL_OCEAN_SECRET; do
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
if [[ ! -e beaglebone_can_upload.sh ]]; then
    echo "Must copy beaglebone_can_upload.dbc file over to host"
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
mkdir -p /var/log/candump
candump -tA -L any | tee -a /var/log/candump/candump.log > /dev/null" | sudo tee /usr/bin/beaglebone_can_listen.sh > /dev/null

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

# Configure candump log rotation. Once the file is > LOG_FILE_SIZE_MB MB, it will be copied to another file, truncated, and the new file will be compressed
LOG_FILE_SIZE_MB=100
NUM_LOG_FILES_AVAILABLE=$((`df --block-size=M | grep '/dev/mmcb' | awk '{ print $4 }' | sed 's/M//g'` / $LOG_FILE_SIZE_MB - 2))

echo "/var/log/candump/candump.log {
    rotate $NUM_LOG_FILES_AVAILABLE
    size ${LOG_FILE_SIZE_MB}M
    compress
    copytruncate
    dateext
    dateformat %Y%m%d.%s
    extension .log
}" | sudo tee /etc/logrotate.d/canlog > /dev/null

# Update logrotate systemd timer to run every 10 minutes
sudo sed -i 's/OnCalendar=.*/OnCalendar=*:0\/10/g' /lib/systemd/system/logrotate.timer

sudo systemctl enable logrotate.timer
sudo systemctl enable canconfigure
sudo systemctl enable canlog

# Install rclone
sudo apt-get update
sudo apt-get install p7zip-full
curl https://rclone.org/install.sh | sudo bash

sudo mkdir -p /root/.config/rclone/

# Configure rclone
echo "[digitalocean]
type = s3
provider = DigitalOcean
access_key_id = ${DIGITAL_OCEAN_ACCESS_KEY}
secret_access_key = ${DIGITAL_OCEAN_SECRET}
endpoint = nyc3.digitaloceanspaces.com" | sudo tee /root/.config/rclone/rclone.conf > /dev/null

# Add rclone upload script
sudo mv beaglebone_can_upload.sh /usr/bin
sudo chmod +x /usr/bin/beaglebone_can_upload.sh

# Setup systemd timer for upload script
echo "[Unit]
Description=Upload CAN data to the cloud

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_can_upload.sh

[Install]
WantedBy=multi-user.target" |  sudo tee /lib/systemd/system/canupload.service > /dev/null

sudo ln -s /lib/systemd/system/canupload.service /etc/systemd/system/

echo "[Unit]
Description=Pushes CAN logs to the cloud
Requires=canupload.service

[Timer]
Unit=canupload.service
OnCalendar=*:0/2
AccuracySec=15s

[Install]
WantedBy=timers.target" | sudo tee /lib/systemd/system/canupload.timer > /dev/null

sudo ln -s /lib/systemd/system/canupload.timer /etc/systemd/system/

sudo systemctl enable canupload.timer

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
