#!/bin/bash
#For DigitalOcean droplet with Debian 10
sudo apt-get update
sudo apt install postgresql-client-11
sudo apt-get install python3-pip
sudo apt install git-all
python3 -m pip install git+https://github.com/eerimoq/cantools.git@3bbc98bd2a9fc2d62979fca0bbf9a6c9b8e84df1#egg=cantools
sudo apt install libpq-dev
python3 -m pip install psycopg2
