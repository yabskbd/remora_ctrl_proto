#!/bin/bash
HOST_NAME=$(cat /etc/hostname);
HOST_ID=$(echo $HOST_NAME | sed 's/remorabone//g');
# This absurd pipeline takes files of the form candump<date>.log.gz and uploads them to the cloud in the form candump<date>_<host_id>.log.gz
find /var/log/candump -type f -name 'candump*.gz' -printf '%f\n' | xargs -I % sh -c "DEST_FILE_NAME=\$(echo % | sed -E \"s/candump(.*).log.gz/candump\1_$HOST_ID.log.gz/g\"); echo \"Uploading %\"; rclone moveto /var/log/candump/% digitalocean:remoracandump/toload/\$DEST_FILE_NAME"
