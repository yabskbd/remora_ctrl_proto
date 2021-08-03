import boto3
import logging
import gzip
import os
import psycopg2
import sys

from cantools.logreader import Parser
from cantools.database import load_file
from collections import namedtuple
from pathlib import Path

DecodedMessage = namedtuple('DecodedMessage', ['timestamp', 'signal_values'])

class DatabaseWriter:
    def __init__(self):
        self.conn = psycopg2.connect(f"dbname='defaultdb' user='doadmin' password='{os.environ['DATABASE_PASSWORD']}' host='private-canpostgres-do-user-9606770-0.b.db.ondigitalocean.com' port='25060'")
        self.cursor = self.conn.cursor()

    # TODO: Bulk insert or otherwise make more performant
    def write_row(self, timestamp, metric, value):
        self.cursor.execute('insert into can_data(time, metric, value) values (%s, %s, %s)', (timestamp, metric, value))
        self.conn.commit()

# TODO: Consolidate with canparse.py script
def decode_row(row, parser):
    parsed_row = parser.parse(row)
    message = db.get_message_by_frame_id(parsed_row.frame_id)
    return DecodedMessage(timestamp=parsed_row.timestamp, signal_values=message.decode(parsed_row.data))


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    try:
        db = load_file(os.path.join(Path(__file__).parent.resolve(), 'remora.dbc'), frame_id_mask=0x00ffff00)
    except FileNotFoundError:
        sys.exit('ERROR: Cannot find remora.dbc file. It should exist in the same directory as this script')

    can_parser = Parser()
    db_writer = DatabaseWriter()
    do_client = boto3.resource('s3',
                               region_name='nyc3',
                               endpoint_url='https://nyc3.digitaloceanspaces.com',
                               aws_access_key_id=os.environ['DIGITAL_OCEAN_ACCESS_KEY'],
                               aws_secret_access_key=os.environ['DIGITAL_OCEAN_SECRET'])
    bucket = do_client.Bucket('testingcandump')
    for compressed_file in bucket.objects.filter(Prefix='dumps/'):
        logging.info(f'Starting to process file: {compressed_file.key}')
        for can_dump_row in gzip.decompress(compressed_file.get()['Body'].read()).splitlines():
            try:
                try:
                    decoded_message = decode_row(can_dump_row.decode('ascii'), can_parser)
                except KeyError:
                    continue

                for signal, value in decoded_message.signal_values.items():
                    db_writer.write_row(decoded_message.timestamp, signal, value)

            except Exception as ex:
                logging.warning(f'Cannot handle row: {can_dump_row}', exc_info=True)
        logging.info(f'Done processing file: {compressed_file.key}')
