
# install venv
sudo apt-get update
sudo apt-get install python3-venv
# create python service user
sudo useradd --system rduserviceuser
# create data folder and assign to service user
sudo mkdir /var/lib/rdu
sudo chown rduserviceuser /var/lib/rdu


