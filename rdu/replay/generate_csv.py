import os
import sys

from argparse import ArgumentParser
from cantools.logreader import Parser
from cantools.database import load_file
from collections import namedtuple
from datetime import datetime, timedelta
from pathlib import Path

DecodedMessage = namedtuple('DecodedMessage', ['timestamp', 'signal_values'])

def get_pgn(frame_id):
    result = int((frame_id & 0x00ffff00) >> 8)
    return result

def get_all_signals(can_database, pgns_to_include):
    signal_name_to_idx = {} 
    csv_headers = ['Time']
    idx = 0
    for message in can_database.messages:
        if message.protocol == 'j1939' and pgns_to_include and get_pgn(message.frame_id) not in pgns_to_include:
            continue
        for signal in message.signals:
            signal_name_to_idx[signal.name] = idx
            idx += 1
            header = signal.name
            if signal.unit:
                header += f' ({signal.unit})'
            csv_headers.append(header)

    return signal_name_to_idx, csv_headers

def decode_row(row, parser):
    parsed_row = parser.parse(row)
    message = db.get_message_by_frame_id(parsed_row.frame_id)
    return DecodedMessage(timestamp=parsed_row.timestamp, signal_values=message.decode(parsed_row.data))

def update_latest_values(decoded_row, latest_values_by_signal_name):
    for signal_name, signal_value in decoded_row.signal_values.items():
        latest_values_by_signal_name[signal_name] = signal_value

def should_print_csv_row(current_message_timestamp, time_bucket, latest_print_timestamp):
    if (current_message_timestamp - latest_print_timestamp) > time_bucket:
        return True
    else:
        return False

UNIX_TIME_0 = datetime.utcfromtimestamp(0)
SECONDS_TO_MICROSECONDS = 1E6

def get_prior_time_bucket(current_message_timestamp, time_bucket):
    if type(current_message_timestamp) != datetime:
        raise ValueError(f'Timestamp must be datetime. Given {type(current_message_timestamp)}')

    current_timestamp_utc_micros = int((current_message_timestamp - UNIX_TIME_0).total_seconds() * SECONDS_TO_MICROSECONDS)
    bucket_size_micros = int(time_bucket.total_seconds() * SECONDS_TO_MICROSECONDS)
    return datetime.utcfromtimestamp((current_timestamp_utc_micros - (current_timestamp_utc_micros % bucket_size_micros)) / SECONDS_TO_MICROSECONDS)


def print_data_row(bucket_time, signal_name_to_idx, latest_values_by_signal_name):
    row = ['' for idx in range(len(signal_name_to_idx))]
    row.insert(0, bucket_time.strftime('%Y-%m-%d %H:%M:%S.%f'))
    for signal_name, idx in signal_name_to_idx.items():
        row[idx + 1] = str(latest_values_by_signal_name.get(signal_name, ''))

    print(','.join(row))

if __name__ == '__main__':
    parser = ArgumentParser(description='Parse CAN dump file into a csv')
    parser.add_argument('--bucket-size', help='How often to log the latest CAN values, in milliseconds',
                        dest='time_bucket', type=int, default=1000)
    parser.add_argument('--pgns', help='Which PGNs to include (comma-separated)',
                        dest='pgns', type=str, default=None)
    args = parser.parse_args()
    time_bucket = timedelta(milliseconds=args.time_bucket)
    pgns_to_include = set([int(p) for p in args.pgns.split(',')]) if args.pgns else None

    try:
        db = load_file(os.path.join(Path(__file__).parent.resolve(), 'remora.dbc'), frame_id_mask=0x00ffff00)
    except FileNotFoundError:
        sys.exit('ERROR: Cannot find remora.dbc file. It should exist in the same directory as this script')

    can_parser = Parser()
    latest_values_by_signal_name = {}
    latest_print_timestamp = None

    signal_name_to_idx, csv_headers = get_all_signals(db, pgns_to_include)
    print(','.join(csv_headers))

    first_row = True  # Used for initial time bucketing logic

    for row in sys.stdin:
        try:
            try:
                decoded_row = decode_row(row, can_parser)
            except KeyError:
                continue

            if first_row:
                latest_print_timestamp = get_prior_time_bucket(decoded_row.timestamp, time_bucket)
                first_row = False

            if should_print_csv_row(decoded_row.timestamp, time_bucket, latest_print_timestamp):
                bucket_time = get_prior_time_bucket(decoded_row.timestamp, time_bucket)
                latest_print_timestamp = decoded_row.timestamp
                print_data_row(bucket_time, signal_name_to_idx, latest_values_by_signal_name)

            for signal_name, signal_value in decoded_row.signal_values.items():
                latest_values_by_signal_name[signal_name] = signal_value
        except Exception as ex:
            print(ex)
            print('Error handling row: ' + row)

