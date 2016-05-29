#!/bin/bash
#
# Sets up a composite HID + ECM USB device.
#
# Expects to be run on Debian Jessie base image
# provided by Inverse Path.
#
# The MAC address will be set to the same one
# that the Debian Jessie base image uses, namely
# 1a:55:89:a2:69:41. The host will see the device
# as having the MAC address 1a:55:89:a2:69:42.
#
# Collin Mulliner <collin AT mulliner.org>
# Quentin Young <qlyoung@qlyoung.net>

# load libcomposite
modprobe libcomposite

# remove all USB Ethernet drivers
modprobe -r g_ether usb_f_ecm u_ether

# insert modules for HID and ECM Ethernet
modprobe usb_f_hid
modprobe usb_f_ecm

# make new usb gadget in configfs
cd /sys/kernel/config/
mkdir usb_gadget/g1
cd usb_gadget/g1
mkdir configs/c.1

# enable hid and ecm functions
mkdir functions/hid.usb0
mkdir functions/ecm.usb0

# setup hid parameters for the keyboard function
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 8 > functions/hid.usb0/report_length
echo -ne "\x05\x01\x09\x06\xA1\x01\x05\x07\x19\xE0\x29\xE7\x15\x00\x25\x01\x75\x01\x95\x08\x81\x02\x95\x01\x75\x08\x81\x03\x95\x05\x75\x01\x05\x08\x19\x01\x29\x05\x91\x02\x95\x01\x75\x03\x91\x03\x95\x06\x75\x08\x15\x00\x25\x65\x05\x07\x19\x00\x29\x65\x81\x00\xC0" > functions/hid.usb0/report_desc

# set the host and device MAC addresses to be the same as the armory's
# default Debian Jessie MAC address as set in /etc/modprobe/usbarmory.conf
echo 1a:55:89:a2:69:41 > functions/ecm.usb0/dev_addr
echo 1a:55:89:a2:69:42 > functions/ecm.usb0/host_addr

# configure general properties for the USB device
mkdir strings/0x409
mkdir configs/c.1/strings/0x409
echo 0x1d6b > idVendor                  # Linux Foundation
echo 0x0104 > idProduct                 # Multifunction Composite Gadget
echo 0x0100 > bcdDevice                 # v1.0.0
echo 0x0200 > bcdUSB                    # USB2
echo "deadbeef9876543210" > strings/0x409/serialnumber
echo "USBArmory" > strings/0x409/manufacturer
echo "USBArmory Network + Keyboard" > strings/0x409/product
echo 120 > configs/c.1/MaxPower

# name this configuration
echo "Conf1" > configs/c.1/strings/0x409/configuration
echo 120 > configs/c.1/MaxPower

# link and enable functionfs functions
ln -s functions/hid.usb0 configs/c.1/
ln -s functions/ecm.usb0 configs/c.1/

# bind usbg driver
echo ci_hdrc.0 > UDC
