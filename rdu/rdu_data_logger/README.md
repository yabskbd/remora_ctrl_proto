# RDU Data Logger

This is a Python module for the Remora Data Unit's logging

# Getting Started

This package is designed to run on the BeagleBone Black, and assumes you have
configured the board to communicate over CAN. See `beaglebone_can_setup.sh` and
`beaglebone_python_setup.sh` for help with system setup

Once you have cloned the code, create a virtual environment using `python3 -m venv .env`.
Then run `pip install -r requirements.txt`. Finally you can run the module as the service user
like so: `sudo su - rduserviceuser -s /bin/bash -c "/home/debian/remora_ctrl_proto/rdu/rdu_data_logger/.data_logger_env/bin/python canlogger.py` (See bash scripts for service user setup. Service user needs access to data folder)
