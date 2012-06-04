#!/bin/sh
# `cat autosuspend` returns 2 on a fresh install. I assume -1 disables
# autosuspend. This change is required to get the power-hungry WiFi adapter
# working with Linux.
sudo sh -c "echo -1 > /sys/module/usbcore/parameters/autosuspend"
