#!/bin/bash
#For DigitalOcean droplet with Debian 10
set -e
for ENV_VAR in PGPASSWORD DIGITAL_OCEAN_ACCESS_KEY DIGITAL_OCEAN_SECRET; do
        if [[ -z "${!ENV_VAR}" ]]; then
            echo "Must set environment variable $ENV_VAR";
            exit 1;
        fi
done

if [[ ! -e canload.sh ]]; then
	echo "Must copy canload.sh script to host"
	exit 1;
fi

if [[ ! -e canparse.py ]]; then
	echo "Must copy canparse.py script to host"
	exit 1;
fi

if [[ ! -e remora.dbc ]]; then
	echo "Must copy remora.dbc file to host"
	exit 1;
fi

sudo apt-get update
sudo apt-get install -y postgresql-client-11
sudo apt-get install -y python3-pip
sudo apt-get install -y git-all
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools
sudo apt-get install -y libpq-dev
python3 -m pip install psycopg2
sudo apt-get install -y curl
if ! [[ -x $(command -v rclone) ]]; then
	curl https://rclone.org/install.sh | sudo bash
fi

# Configure rclone
echo "[digitalocean]
type = s3
provider = DigitalOcean
access_key_id = ${DIGITAL_OCEAN_ACCESS_KEY}
secret_access_key = ${DIGITAL_OCEAN_SECRET}
endpoint = nyc3.digitaloceanspaces.com" | sudo tee /root/.config/rclone/rclone.conf > /dev/null

sudo cp canparse.py /usr/bin
sudo cp remora.dbc /usr/bin
sudo cp canload.sh /usr/bin
sudo chmod +x /usr/bin/canload.sh

echo "[Unit]
Description=CAN Database Loader
After=syslog.target
StartLimitBurst=10
StartLimitIntervalSec=3min

[Service]
Type=simple
Restart=on-failure
RestartSec=10s
ExecStart=/usr/bin/canload.sh
Environment=PGPASSWORD=$PGPASSWORD

[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/remoracanload.service > /dev/null

sudo ln -sf /lib/systemd/system/remoracanload.service /etc/systemd/system/

sudo systemctl enable remoracanload.service
sudo systemctl start remoracanload.service

