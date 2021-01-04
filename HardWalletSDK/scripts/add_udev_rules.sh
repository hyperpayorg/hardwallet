#!/bin/bash
echo "SUBSYSTEMS==\"usb\", ATTRS{idVendor}==\"096e\", ATTRS{idProduct}==\"0891\", MODE=\"0660\", GROUP=\"plugdev\"" >/etc/udev/rules.d/20-Jupiter1.rules

udevadm trigger
udevadm control --reload-rules
