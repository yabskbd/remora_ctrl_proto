# RDU

This contains all the configuration and scripting for the Remora Data Unit

# Getting Started

The RDU will run on a BeagleBone Black Wireless. First, download the latest IoT Debian image for BeagleBone from [here](https://debian.beagleboard.org/images/bone-debian-10.3-iot-armhf-2020-04-06-4gb.img.xz) (tested with the 2020-04-06 10.3 release). Then, download [Balena Etcher](https://www.balena.io/etcher/). Plug in your SD card to your computer, and open Balena Etcher. Flash the SD card with the .img.xz Debian image file. Then, plug the SD card into the BeagleBone, and connect the BeagleBone to your computer over microUSB.

The BeagleBone automatically creates a local network with your computer over USB. Once the BeagleBone has booted, you should be able to run `scp beaglebone_initial_setup.sh debian@beaglebone.local:~ && scp replay/canparse.py debian@beaglebone.local:~ && scp replay/remora.dbc debian@beaglebone.local:~ && scp replay/beaglebone_can_upload.sh debian@beaglebone.local:~` via [git bash](https://git-scm.com/downloads). This will copy the initialization script, and CAN parsing code over to the BeagleBone. You will have to enter the default password repeatedly (sorry!) - the password is `temppwd`. Once the files have all copied, you can run `ssh debian@beaglebone.local` and use default password `temppwd` to login to the device. Run `find . -maxdepth 1 -type f -print0 | xargs -0 sed -i 's/\r//g'` to convert any Windows-style line endings to unix.

Now, we need to manually create a DigitalOcean Spaces access key and secret. Open the DO dashboard, and click on API:

![image](https://user-images.githubusercontent.com/9583514/128417590-194f1d20-006c-4039-9f56-dbb82fcb7137.png)

Then, click Generate New Key under the Spaces area:

![image](https://user-images.githubusercontent.com/9583514/128417673-3efb17f8-e98e-4b28-8c87-e5f036139605.png)

For now, name the key `temp`:

![image](https://user-images.githubusercontent.com/9583514/128417780-0baf82d4-525e-46c2-ad5c-cc5adc78aaf6.png)

Then, the key and secret will appear. **Copy these values into notepad!** You will need them to run the script.

Back on the BeagleBone, run `ECHENEIDAE_WIFI_PASSWORD=OfficeWifiPassword DIGITAL_OCEAN_ACCESS_KEY=xxxx DIGITAL_OCEAN_SECRET=yyy ./beaglebone_initial_setup.sh` (replace with appropriate passwords. You will need to provide the default debian password `temppwd` once more). You may see `Re-reading the partition table failed.: Device or resource busy` - this is expected and not a critical error. 

Once the script has completed, it should print something like this:

```
************************************************
            RECORD THESE VALUES
************************************************
host name: newhost1234
debian user password: verysecret
beaglebone wifi password: morecret
************************************************
```

Now, let's go back to DO and assign the secret to this host. Click *More* and then *Rename*. Then rename the key to the host name:

![image](https://user-images.githubusercontent.com/9583514/128423252-6c04eb07-6488-4be6-b905-5974cebf2d68.png)

Now that the script has finished, run `rm beaglebone_initial_setup.sh` and unplug the device from your computer. Then, plug it into a 5V power source, and after a few minutes, the device should advertise a WiFi hotspot of the form `RemoraBone<device_id>`. If you connect to this hotspot, the device will be available over ssh: `ssh debian@192.168.8.1` - remember the password is now the `beaglebone wifi password` that was printed under RECORD THESE VALUES.

SSH may give you a host key error when connecting to the Bone - you can remove the key from `/c/Users/<username>/.ssh/known_hosts` in order to fix the error. Also, the device is now configured to connect to any `Echeneidae` WiFi hotspot. If the BeagleBone is connected to WiFi, and your laptop is connected to the BeagleBone's hotspot, you will continue to have internet access through the BeagleBone.

# Helpful Commands

To view the current CAN logs:

```
tail -f /var/log/candump/candump.log
```

To force a log rotation of the CAN log file:

```
sudo logrotate -vf /etc/logrotate.d/canlog
```

To view the next time a timer will trigger (either the CAN log rotation, or the CAN upload process):

```
sudo systemctl list-timers
```
 
To view the CAN uploader logs:

```
sudo journalctl -u canupload.*
```

To parse the current CAN logs into a CSV:

```
tail -f /var/log/candump/candump.log | canparse > ~/parseddump.csv &
```

This will print the pid of the process back to you. Something like this:

```
[1] 2678
```

Since we are spwaning subprocesses, the best way to kill the parsing is like this:

```
pkill -P 2678
```

This will run the parsing in the background. If you then want to watch the parsed data flow in, you can run:

```
tail -f ~/parseddump.csv
```


Finally, if you want to copy the parsed dump back to your laptop, you can run this via Git Bash on your laptop:

```
scp debian@192.168.8.1:~/parseddump.csv ~/Downloads
```
