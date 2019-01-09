# usb_monitor
Monitor USB devices connections and disconnections on the current machine using UDEV rules, then send the information regarding the USB device to a remote server.

## Usage
```
$ usb_monitor <server ip> <port>
[*] Waiting for usb devices.
Sending 115 bytes of data.

Server raw data (Serialized JSON data)
{"Action":"[add|remove]","dev_name":"<syspath name>","dev_path":"<udev device path>","dev_type":"usb_device"}

```

Windows version and storage server in progress...
