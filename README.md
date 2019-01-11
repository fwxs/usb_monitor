# usb_monitor
Monitor USB devices connections and disconnections on the current machine using UDEV rules, then send the information regarding the USB device to a remote server.

## Usage
```
$ usb_monitor <server ip> <port>

[*] Waiting for usb devices.
[*] Reporting device connection.
[*] Device disconnected.


Server raw data (Serialized JSON data)
{"Action":"add","dev_manufacturer":"<data>","dev_path":"<sysfs path>","max_pkt_size":"<data>","max_power":"<power mA>","product":"<product name>","product_id":"<id>","serial":"<device serial>","speed":"<speed in MB>","vendor_id":"<vendor id>"}

{"Action":"remove","dev_manufacturer":"<data>","dev_path":"<sysfs path>","max_pkt_size":"<data>","max_power":"<power mA>","product":"<product name>","product_id":"<id>","serial":"<device serial>","speed":"<speed in MB>","vendor_id":"<vendor id>"}
```

Windows version and storage server in progress...
