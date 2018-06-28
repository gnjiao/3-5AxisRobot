#! /bin/sh
DATABASE_PATH="/opt/Qt/UpdateApps"

# Set the QWS_MOUSE_PROTO for touchscreen if it exists
if [ -e /dev/input/touchscreen0 ]
then
    #load modules
    InstallModules.sh /home/root
    ln -s /dev/ttyO1 /dev/ttySZHC0
    #export environment 
    export QWS_MOUSE_PROTO=Tslib:/dev/input/touchscreen0
    export set QWS_KEYBOARD="linuxinput:/dev/input/event1:repeat-rate=200:repeat-delay=500:keymap=/home/root/3a8.qmap"
    export set QWS_DISPLAY="LinuxFB:/dev/fb0"
    export set QWS_DISPLAY="LinuxFB:mmWidth=192:mmHeight=144:0"
    export set QWS_SIZE=="640x480"
    export set QT_QWS_FONTDIR=/opt/Qt/fonts
fi

if [ -d "/proc/scsi/usb-storage/" ];then
    sleep 3
fi

if [ -f "/mnt/udisk/SelectSystemBootWay" ];then
    cd /mnt/udisk/
	if [ -f "/mnt/udisk/Reinstall/canupdate_ml" ];then
    	./SelectSystemBootWay -qws
	fi
else
    echo "normal boot system(/usr/bin/QtAutorun.sh)"
fi

if [ -f /home/szhc/recal ] ; then
	ts_calibrate
	cp /etc/pointercal /home/szhc/pointercal.bak
	rm /home/szhc/recal
	cd $QTDIR/bin
else
	cp /home/szhc/pointercal.bak /etc/pointercal
	cd $QTDIR/bin
fi



RunApp.sh &
