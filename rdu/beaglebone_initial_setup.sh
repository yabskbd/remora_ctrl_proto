#!/bin/bash
for ENV_VAR in BEAGLEBONE_WIFI_PASSWORD DEBIAN_USER_PASSWORD ECHENEIDAE_WIFI_PASSWORD; do
        if [[ -z "${!ENV_VAR}" ]]; then
                echo "Must set environment variable $ENV_VAR";
		exit 1;
        fi
done

echo "remorabone$(</proc/sys/kernel/random/uuid)" | sudo tee /etc/hostname > /dev/null
HOST_NAME=$(cat /etc/hostname) 
sudo sed -i "s/beaglebone/$HOST_NAME/g" /etc/hosts

FIRST_PART_OF_HOST_NAME=$(echo $HOST_NAME | sed -e 's/remorabone\(.*\)-\(.*\)-\(.*\)-\(.*\)-\(.*\)$/\1/')
sudo sed -i 's/USE_PERSONAL_PASSWORD="BeagleBone"/USE_PERSONAL_PASSWORD="'$BEAGLEBONE_WIFI_PASSWORD'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_PERSONAL_SSID="BeagleBone"/USE_PERSONAL_SSID="RemoraBone'$FIRST_PART_OF_HOST_NAME'"/g' /etc/default/bb-wl18xx
sudo sed -i 's/USE_APPENDED_SSID=yes/USE_APPENDED_SSID=no/g' /etc/default/bb-wl18xx

echo "[global]
Name = Echeneidae
Description = Remora network services

[service_wifi_echeneidae]
Type = wifi
Name = Echeneidae
Passphrase = $ECHENEIDAE_WIFI_PASSWORD" | sudo tee /var/lib/connman/echeneidae.config > /dev/null

echo "#!/bin/bash
sudo config-pin p9.24 can
sudo config-pin p9.26 can
sudo config-pin p9.19 can
sudo config-pin p9.20 can
sudo ip link set can1 up type can bitrate 250000 listen-only on restart-ms 250
sudo ip link set can0 up type can bitrate 250000 listen-only on restart-ms 250
sudo ifconfig can1 up
sudo ifconfig can0 up" | sudo tee /usr/bin/beaglebone_can_setup.sh > /dev/null

echo "[Unit]
Description=Configure CAN buses on system startup

[Service]
Type=simple
ExecStart=/usr/bin/beaglebone_can_setup.sh

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/bonecan.service > /dev/null

sudo ln -s /lib/systemd/system/bonecan.service /etc/systemd/system/

sudo systemctl enable bonecan

python3 -m pip install wheel
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools
sudo mv generate_csv.py /usr/local/bin
sudo mv remora.dbc /usr/local/bin

echo "debian:$DEBIAN_USER_PASSWORD" | sudo chpasswd
echo "Successfully setup $HOST_NAME"

