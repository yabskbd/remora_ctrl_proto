import can
import typing

from can.interface import Bus
from datetime import datetime
from numbers import Number
from os import path
from pathlib import Path



class BusReader:
    """
    Utilizes socketCAN to read all CAN data from the
    specified channel
    """
    def __init__(self, can_ids_to_include=None, can_channel:str='can1') -> None:
        """
        can_ids_to_include:
            If None, all CAN messages will be returned. Else, only the ids in this list will be returned.
            Note this filtering is done at the hardware/kernel level, so should be more
            performant than filtering in Python

        can_channel:
            The network interface to accept messages on. This must be pre-configured
            from the command line, using 'ip link'. When running 'ip link' we should
            also configure the bitrate of the interface
        """
        self.can_ids_to_include = can_ids_to_include
        self.can_channel = can_channel
        self.bus = None

    def __enter__(self):
        self.initialize()
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.shutdown()

    def __iter__(self):
        return self.bus.__iter__()

    def initialize(self) -> None:
        self.bus = Bus(bustype='socketcan', channel=self.can_channel)
        bus_filter_list = []
        if self.can_ids_to_include:
            for can_id in self.can_ids_to_include:
                bus_filter_list.append({'can_id': can_id, 'can_mask': 0xFF})
        self.bus.set_filters(bus_filter_list)

    def shutdown(self) -> None:
        if self.bus:
            self.bus.shutdown()


class DataFileWriter:
    """
    Handles opening and writing data to a data file
    """
    DEFAULT_FILE_PATH = '/var/lib/rdu/data/candata.txt'

    def __init__(self, data_file_path:str=None) -> None:
        self.data_file_path = data_file_path or self.DEFAULT_FILE_PATH
        if not self.data_file_path:
            raise ValueError('Must specify data_file_path')
        self.data_file:typing.TextIO = None
    
    def __enter__(self):
        self.initialize()
        self.data_file = open(self.data_file_path, 'w')
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.data_file.flush()
        self.data_file.close()

    def initialize(self) -> None:
        Path(path.dirname(self.data_file_path)).mkdir(parents=True, exist_ok=True)

    def write_data_row(self, data_row) -> None:
        time_str = datetime.utcnow().isoformat()
        new_row = time_str + '; ' + '; '.join([f'{k}={v}' for k, v in data_row.items()]) + '\n'
        self.data_file.write(new_row)
        self.data_file.flush()

def main():
    with DataFileWriter() as d:
        with BusReader(can_ids_to_include=[0x100]) as br:
            for msg in br:
                temp_celsius = (msg.data[2] * 16**2 + msg.data[3]) / 100.0
                d.write_data_row({'ambient_temperature': temp_celsius})

if __name__ == '__main__':
    main()

