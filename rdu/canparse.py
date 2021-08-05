import os
import sys
import logging

from argparse import ArgumentParser
from cantools.logreader import Parser
from cantools.database import load_file
from cantools.database import Message, Signal
from collections import namedtuple
from datetime import datetime, timedelta
from pathlib import Path

DecodedMessage = namedtuple('DecodedMessage', ['timestamp', 'signal_values'])

def get_pgn(frame_id):
    result = int((frame_id & 0x00ffff00) >> 8)
    return result

def get_all_signals(can_database, pgns_to_include):
    signal_name_to_idx = {} 
    signal_names = []
    idx = 0
    for message in can_database.messages:
        if message.protocol == 'j1939' and pgns_to_include and get_pgn(message.frame_id) not in pgns_to_include:
            continue
        for signal in message.signals:
            signal_name_to_idx[signal.name] = idx
            idx += 1
            signal_names.append(signal.name)

    return signal_name_to_idx, signal_names

def get_notset_signal_values(can_database, decode_choices):
    signal_to_notset_value = {}
    for message in can_database.messages:
        decoded_signals = message.decode(bytearray.fromhex('ff' * message.length), decode_choices=decode_choices)
        for signal_name, notset_value in decoded_signals.items():
            signal_to_notset_value[signal_name] = notset_value

    return signal_to_notset_value

def decode_row(row, parser, can_db, decode_choices):
    parsed_row = parser.parse(row)
    message = can_db.get_message_by_frame_id(parsed_row.frame_id)
    return DecodedMessage(timestamp=parsed_row.timestamp, signal_values=message.decode(parsed_row.data, decode_choices=decode_choices))

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


def print_data_row(bucket_time, signal_name_to_idx, latest_values_by_signal_name, device_id):
    row = ['' for idx in range(len(signal_name_to_idx))]
    row.insert(0, bucket_time.strftime('%Y-%m-%d %H:%M:%S.%f'))
    if device_id:
        row.insert(1, str(device_id))
    for signal_name, idx in signal_name_to_idx.items():
        row[idx + (1 if not device_id else 2)] = str(latest_values_by_signal_name.get(signal_name, ''))

    print(','.join(row))

def get_can_db():
    try:
        can_db = load_file(os.path.join(Path(__file__).parent.resolve(), 'remora.dbc'), frame_id_mask=0x00ffff00)
    except FileNotFoundError:
        sys.exit('ERROR: Cannot find remora.dbc file. It should exist in the same directory as this script')

    return can_db

def generate_csv(args):
    time_bucket = timedelta(milliseconds=args.time_bucket)
    pgns_to_include = set([int(p) for p in args.pgns.split(',')]) if args.pgns else None

    can_db = get_can_db()

    can_parser = Parser()
    latest_values_by_signal_name = {}
    latest_print_timestamp = None

    signal_name_to_idx, signal_names = get_all_signals(can_db, pgns_to_include)
    signal_names.insert(0, 'Signal_Time_UTC')
    if args.device_id:
        signal_names.insert(1, 'Device_ID')
    print(','.join(signal_names))

    signal_name_to_notset_value = get_notset_signal_values(can_db, args.decode_choices)

    first_row = True  # Used for initial time bucketing logic

    # While you can simply cat a file and pipe it to this script, pdb sometimes works better if you specify the inputfile in advance
    if not args.inputfile:
        rows = sys.stdin
    else:
        rows = open(args.inputfile, 'r').readlines()

    for row in rows:
        row = row.strip()
        try:
            try:
                decoded_row = decode_row(row, can_parser, can_db, args.decode_choices)
            except KeyError:
                continue

            if first_row:
                latest_print_timestamp = get_prior_time_bucket(decoded_row.timestamp, time_bucket)
                first_row = False

            if should_print_csv_row(decoded_row.timestamp, time_bucket, latest_print_timestamp):
                bucket_time = get_prior_time_bucket(decoded_row.timestamp, time_bucket)
                latest_print_timestamp = decoded_row.timestamp
                print_data_row(bucket_time, signal_name_to_idx, latest_values_by_signal_name, args.device_id)

            for signal_name, signal_value in decoded_row.signal_values.items():
                if signal_name_to_notset_value[signal_name] == signal_value:
                    continue

                latest_values_by_signal_name[signal_name] = signal_value
        except Exception:
            logging.warning(f'Error parsing row: {row}', exc_info=True)

def get_choices_table(signal):
    sql = f'CREATE TABLE IF NOT EXISTS {signal.name}_choices (choice_id smallint PRIMARY KEY  NOT NULL, choice_value varchar(50) NOT NULL);\n'
    sql += f'CREATE TEMPORARY TABLE {signal.name}_choices_temp (choice_id smallint PRIMARY KEY NOT NULL, choice_value varchar(50) NOT NULL) ON COMMIT DROP;\n'
    choice_values = ',\n'.join([f'\t({choice_id}, \'{choice_value}\')' for choice_id, choice_value in signal.choices.items()])
    sql += f'INSERT INTO {signal.name}_choices_temp (choice_id, choice_value) VALUES\n {choice_values};\n'
    sql += f'INSERT INTO {signal.name}_choices (choice_id, choice_value)\nSELECT choice_id, choice_value\n'
    sql += f'FROM {signal.name}_choices_temp\nON CONFLICT (choice_id) DO UPDATE SET choice_value = EXCLUDED.choice_value;\n'
    return sql

def generate_sql():
    can_db = get_can_db()
    message: Message
    TABLE_NAME = 'can_data'
    sql = 'CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;\n'
    sql += f'CREATE TABLE IF NOT EXISTS customer (customer_id SMALLSERIAL PRIMARY KEY NOT NULL, customer_name VARCHAR(200) NOT NULL);\n'
    sql += f'CREATE TABLE IF NOT EXISTS device (device_id UUID PRIMARY KEY NOT NULL, customer_id SMALLINT NULL REFERENCES customer (customer_id));\n'
    sql += f'CREATE TABLE IF NOT EXISTS {TABLE_NAME} (signal_time_utc TIMESTAMP WITHOUT TIME ZONE NOT NULL, device_id UUID NOT NULL REFERENCES device (device_id));\n'
    sql += f'SELECT create_hypertable(\'{TABLE_NAME}\', \'signal_time_utc\', if_not_exists => TRUE);\n'
    for message in can_db.messages:
        signal: Signal
        for signal in message.signals:
            if signal.choices:
                sql_type = 'smallint'
                sql += get_choices_table(signal)
                foreign_key = f'REFERENCES {signal.name}_choices (choice_id)'
            else:
                sql_type = 'double PRECISION'
                foreign_key = ''

            sql += f'ALTER TABLE {TABLE_NAME} ADD COLUMN IF NOT EXISTS {signal.name} {sql_type} {foreign_key} NULL;\n'
            comment = ''
            if signal.unit:
                comment += f'Units: {signal.unit}. '
            if signal.spn:
                comment += f'SPN: {signal.spn}. '
            if signal.comment:
                comment += signal.comment.strip()
            if comment:
                sql += f'COMMENT ON COLUMN {TABLE_NAME}.{signal.name} IS \'{comment}\';\n'
    print(sql)

if __name__ == '__main__':
    parser = ArgumentParser(description='Parse CAN dump file into a csv')
    parser.add_argument('action', nargs='?', default='csv', choices=['csv', 'sqlgen'],
                        help='csv: Generate a CSV with parsed data\n' +
                        'sqlgen: Generate a postgres table to store signals defined in the remora.dbc file')
    parser.add_argument('--bucket-size', help='How often to log the latest CAN values, in milliseconds',
                        dest='time_bucket', type=int, default=1000)
    parser.add_argument('--pgns', help='Which PGNs to include (comma-separated)',
                        dest='pgns', type=str, default=None)
    parser.add_argument('--input-file', help='A CAN dump file to use as input',
                        dest='inputfile', type=str, default=None)
    parser.add_argument('--decode-choices', dest='decode_choices', action='store_true',
                        help='Include to represent enum-like values as strings. Otherwise, they will log as integers')
    parser.add_argument('--device-id', dest='device_id', type=str, default=None,
                        help='The device id this data is coming from')
    parser.set_defaults(decode_choices=False)

    args = parser.parse_args()
    if args.action == 'csv':
        generate_csv(args)
    elif args.action == 'sqlgen':
        generate_sql()
    else:
        sys.exit(f'Unhandled action {args.action}')
