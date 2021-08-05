#!/bin/bash
#For DigitalOcean droplet with Debian 10
sudo apt-get update
sudo apt install postgresql-client-11
sudo apt-get install python3-pip
sudo apt install git-all
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools
sudo apt install libpq-dev
python3 -m pip install psycopg2
sudo apt install curl
curl https://rclone.org/install.sh | sudo bash

for ENV_VAR in DIGITAL_OCEAN_ACCESS_KEY DIGITAL_OCEAN_SECRET; do
        if [[ -z "${!ENV_VAR}" ]]; then
            echo "Must set environment variable $ENV_VAR";
            exit 1;
        fi
done

# Configure rclone
echo "[digitalocean]
type = s3
provider = DigitalOcean
access_key_id = ${DIGITAL_OCEAN_ACCESS_KEY}
secret_access_key = ${DIGITAL_OCEAN_SECRET}
endpoint = nyc3.digitaloceanspaces.com" | sudo tee /root/.config/rclone/rclone.conf > /dev/null
