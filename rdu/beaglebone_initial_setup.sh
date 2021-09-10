#!/bin/bash
# Ensure password environment variables are set
# ECHENEIDAE_WIFI_PASSWORD: The password of the Encheneidae SSID, which this BeagleBone will connect to if available
# DIGITAL_OCEAN_ACCESS_KEY: DigitalOcean Spaces Key
# DIGITAL_OCEAN_SECRET: DigitalOcean Spaces Secret
set -e

for ENV_VAR in ECHENEIDAE_WIFI_PASSWORD DIGITAL_OCEAN_ACCESS_KEY DIGITAL_OCEAN_SECRET; do
        if [[ -z "${!ENV_VAR}" ]]; then
            echo "Must set environment variable $ENV_VAR";
            exit 1;
        fi
done

# Ensure CAN script has been copied, along with dbc database file
for file_name in canparse.py remora.dbc beaglebone_can_upload.sh BB-I2C1-RTC-DS3231.dts; do
        if [[ ! -e $file_name ]]; then
            echo "Must copy $file_name file over to host";
            exit 1;
        fi
done

# Generate a random name for the host, and update relevant files
echo "Updating host name..."
echo "remorabone$(</proc/sys/kernel/random/uuid)" | sudo tee /etc/hostname > /dev/null
host_name=$(cat /etc/hostname) 
sudo sed -i "s/beaglebone/$host_name/g" /etc/hosts
sudo sed -i "s/remorabone.\{36\}/$host_name/g" /etc/hosts

# Update the SoftAP settings, so the BeagleBone advertises a unique SSID, accessible with the generated 
echo "Updating SoftAP settings..."
first_part_of_host_name=$(echo $host_name | sed -e 's/remorabone\(.*\)-\(.*\)-\(.*\)-\(.*\)-\(.*\)$/\1/')
beaglebone_wifi_password=$(< /dev/urandom tr -dc A-Z-a-z-0-9 | head -c${1:-32})
sudo sed -i 's/USE_PERSONAL_PASSWORD=".*"/USE_PERSONAL_PASSWORD="'$beaglebone_wifi_password'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_PERSONAL_SSID=".*"/USE_PERSONAL_SSID="RemoraBone'$first_part_of_host_name'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_APPENDED_SSID=yes/USE_APPENDED_SSID=no/g' /etc/default/bb-wl18xx

# Add connmanctl settings, so we connect to WiFi automatically
echo "Updating connmanctl..."
echo "[global]
Name = Echeneidae
Description = Remora network services

[service_wifi_echeneidae]
Type = wifi
Name = Echeneidae
Passphrase = $ECHENEIDAE_WIFI_PASSWORD" | sudo tee /var/lib/connman/echeneidae.config > /dev/null

# Create CAN setup script
echo "Adding CAN setup service..."
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
Description=Remora CAN Configuration

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_can_setup.sh
After=syslog.target

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/remoracanconfigure.service > /dev/null

sudo ln -sf /lib/systemd/system/remoracanconfigure.service /etc/systemd/system/

# Create CAN logging script
echo "Adding CAN logging service..."
echo "#!/bin/bash
mkdir -p /var/log/candump
FILTERS_FILE=/usr/local/bin/can_filters.txt
if [[ -f $FILTERS_FILE ]]; then
        FILTERS=,$(cat /usr/local/bin/can_filters.txt)
else
        $FILTERS=''
fi

candump -tA -L any$FILTERS | tee -a /var/log/candump/candump.log > /dev/null" | sudo tee /usr/bin/beaglebone_can_listen.sh > /dev/null

sudo chmod +x /usr/bin/beaglebone_can_listen.sh

# Create systemd unit file to run CAN logging script on startup, and restart on failure
echo "[Unit]
Description=Remora CAN Logger
Requires=remoracanconfigure.service
After=remoracanconfigure.service syslog.target

StartLimitBurst=10
StartLimitIntervalSec=3min

[Service]
Type=simple
Restart=on-failure
RestartSec=10s
ExecStart=/usr/bin/beaglebone_can_listen.sh

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/remoracanlog.service > /dev/null

sudo ln -sf /lib/systemd/system/remoracanlog.service /etc/systemd/system/

# Configure candump log rotation. Once the file is > log_file_size_mb MB, it will be copied to another file, truncated, and the new file will be compressed
echo "Configuring CAN log rotation..."
log_file_size_mb=10
num_log_files_available=$((`df --block-size=M | grep '/dev/mmcb' | awk '{ print $4 }' | sed 's/M//g'` / $log_file_size_mb))

echo "/var/log/candump/candump.log {
    rotate $num_log_files_available
    size ${log_file_size_mb}M
    compress
    copytruncate
    dateext
    dateformat %Y%m%d.%s
    extension .log
}" | sudo tee /etc/logrotate.d/canlog > /dev/null

# Update logrotate systemd timer to run every 5 minutes
sudo sed -i 's/OnCalendar=.*/OnCalendar=*:0\/5/g' /lib/systemd/system/logrotate.timer

sudo systemctl enable logrotate.timer
sudo systemctl enable remoracanconfigure.service
sudo systemctl enable remoracanlog.service

# Install rclone
echo "Installing rclone..."
if ! [[ -x $(command -v rclone) ]]; then
    sudo apt-get update
    sudo apt-get -y install p7zip-full
    curl https://rclone.org/install.sh | sudo bash
fi

sudo mkdir -p /root/.config/rclone/

# Configure rclone
echo "[digitalocean]
type = s3
provider = DigitalOcean
access_key_id = ${DIGITAL_OCEAN_ACCESS_KEY}
secret_access_key = ${DIGITAL_OCEAN_SECRET}
endpoint = nyc3.digitaloceanspaces.com" | sudo tee /root/.config/rclone/rclone.conf > /dev/null

# Add rclone upload script
echo "Adding CAN upload service..."
sudo mv beaglebone_can_upload.sh /usr/bin
sudo chmod +x /usr/bin/beaglebone_can_upload.sh

# Setup systemd timer for upload script
echo "[Unit]
Description=Upload CAN data to the cloud
After=syslog.target

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_can_upload.sh

[Install]
WantedBy=multi-user.target" |  sudo tee /lib/systemd/system/remoracanupload.service > /dev/null

sudo ln -sf /lib/systemd/system/remoracanupload.service /etc/systemd/system/

echo "[Unit]
Description=Remora CAN Log Cloud Uploader
Requires=remoracanupload.service
After=syslog.target

[Timer]
Unit=remoracanupload.service
OnCalendar=*:0/2
AccuracySec=15s

[Install]
WantedBy=timers.target" | sudo tee /lib/systemd/system/remoracanupload.timer > /dev/null

sudo ln -sf /lib/systemd/system/remoracanupload.timer /etc/systemd/system/

sudo systemctl enable remoracanupload.timer

# Install python dependencies globally
echo "Installing Python requirements..."
python3 -m pip install wheel
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools

# Install canparse script
sudo mv canparse.py /usr/local/bin
sudo mv remora.dbc /usr/local/bin

echo "#!/bin/bash
python3 -u /usr/local/bin/canparse.py \"\$@\"" | sudo tee /usr/local/bin/canparse > /dev/null

sudo chmod +x /usr/local/bin/canparse

echo "Adding CAN DBC download service..."
touch /usr/local/bin/can_filters.txt
sudo chmod +666 /usr/local/bin/can_filters.txt
echo "#!/bin/bash
cd /usr/local/bin
curl -O https://raw.githubusercontent.com/yabskbd/remora_ctrl_proto/main/rdu/remora.dbc
if [[ $? -eq 0 ]]; then
    python3 canparse.py filters
    sudo systemctl restart remoracanlog.service
fi
cd -" | sudo tee /usr/bin/beaglebone_dbc_download.sh > /dev/null

# Setup systemd timer for DBC download script
echo "[Unit]
Description=Download DBC file from GitHub
After=syslog.target

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_dbc_download.sh

[Install]
WantedBy=multi-user.target" |  sudo tee /lib/systemd/system/remoradbcdownload.service > /dev/null

sudo ln -sf /lib/systemd/system/remoradbcdownload.service /etc/systemd/system/

echo "[Unit]
Description=Remora DBC Downloader
Requires=remoradbcdownload.service
After=syslog.target

[Timer]
Unit=remoradbcdownload.service
OnCalendar=*:0/2
AccuracySec=15s

[Install]
WantedBy=timers.target" | sudo tee /lib/systemd/system/remoradbcdownload.timer > /dev/null

sudo ln -sf /lib/systemd/system/remoradbcdownload.timer /etc/systemd/system/

sudo systemctl enable remoradbcdownload.timer

# Use handy BeagleBone script to partition disk to full size of SD card
sudo /opt/scripts/tools/grow_partition.sh

# Disable bluetooth
echo "Disabling bluetooth..."
sudo systemctl stop bb-wl18xx-bluetooth
sudo systemctl disable bb-wl18xx-bluetooth
sudo systemctl stop bluetooth
sudo systemctl disable bluetooth

# Install DS3231 RTC on I2C1 interface
echo "Setting up RTC..."
set +e
cat /boot/uEnv.txt | grep DS3231 > /dev/null
rtc_already_exists=$?
set -e
if [[ rtc_already_exists -ne 0 ]]; then
    git clone https://github.com/beagleboard/bb.org-overlays.git
    cd bb.org-overlays
    cp ../BB-I2C1-RTC-DS3231.dts src/arm
    ./install.sh
    cd ..
    rm -rf bb.org-overlays
    echo -e "\ndtb_overlay=/lib/firmware/BB-I2C1-RTC-DS3231.dtbo\n" | sudo tee -a  /boot/uEnv.txt > /dev/null
fi

# Update password from default 'temppwd' to provided DEBIAN_USER_PASSWORD
echo "Updating debian password..."
debian_password=$(< /dev/urandom tr -dc A-Z-a-z-0-9 | head -c${1:-32})
echo "debian:$debian_password" | sudo chpasswd
echo "Successfully setup $host_name"
echo "************************************************"
echo "            RECORD THESE VALUES                 "
echo "************************************************"
echo "host name: $host_name"
echo "debian user password: $debian_password"
echo "beaglebone wifi password: $beaglebone_wifi_password"
echo "************************************************"
