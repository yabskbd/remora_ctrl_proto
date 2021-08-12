#!/bin/bash
# NOTE: PGPASSWORD env variable must be set to the postgres password

# Errors should go onto stderr
echoerr() { echo "$@" 1>&2; }

# This is simple way to detect error early in a pipeline
set -o pipefail
while true; do
    file_to_parse=$(head -n 1 < <(rclone ls digitalocean:remoracandump/toload/ ) |  awk '{ print $2 }')
    if [[ $? -ne 0 ]]; then
        echoerr "Error listing files on DO. Trying again..."
        sleep 10s
        continue
    fi
    if [[ -z $file_to_parse ]]; then
        echo "No files found to parse. Waiting..."
        sleep 15s
        continue
    fi

    echo "Processing dump $file_to_parse"
    rclone copy digitalocean:remoracandump/toload/$file_to_parse /tmp/
    if [[ $? -ne 0 ]]; then
        echoerr "Detected error in rclone copy"
        sleep 10s
        continue
    fi
    echo $ret_code
    echo "Copied digitalocean:remoracandump/toload/$file_to_parse locally"
    gzip --force -d /tmp/$file_to_parse
    if [[ $? -ne 0 ]]; then
        echoerr "Cannot unzip $file_to_parse. Moving to failure folder"
        rclone moveto digitalocean:remoracandump/toload/$file_to_parse digitalocean:remoracandump/failures/$file_to_parse 
        continue
    fi

    unzipped_file_to_parse=$(echo $file_to_parse | sed "s/.gz//g")
    device_id=$(echo $unzipped_file_to_parse | sed -E 's/.*_(.*).log/\1/')

    attempt_count=0
    max_attempts=5
    while (test "$attempt_count" -le 5); do
	    cat /tmp/$unzipped_file_to_parse | python3 $(dirname "$0")/canparse.py --device-id $device_id | psql --host=private-canpostgres-do-user-9606770-0.b.db.ondigitalocean.com --port=25060 --dbname=candump --username=canloader -c "INSERT INTO device (device_id) VALUES ('$device_id') ON CONFLICT DO NOTHING; COPY can_data FROM STDIN DELIMITER ',' CSV HEADER;"
        if [[ $? -ne 0 ]]; then
            echoerr "Error parsing $file_to_parse"
        else
            echo "Successfully parsed $file_to_parse"
            break
        fi
        sleep 1s
        attempt_count=$((attempt_count + 1))
    done

    if [[ $attempt_count -eq $(($max_attempts + 1)) ]]; then
        echoerr "CRITICAL ERROR HANDLING $file_to_parse. Moving to failure folder."
        rclone moveto digitalocean:remoracandump/toload/$file_to_parse digitalocean:remoracandump/failures/$file_to_parse 
    else
        echo "Successfully handled $file_to_parse. Moving to success folder."
        rclone moveto digitalocean:remoracandump/toload/$file_to_parse digitalocean:remoracandump/successes/$file_to_parse 
    fi    
done
