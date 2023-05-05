# eelreless
ESP32 firmware to read power consumption and provide HTTP API access

Configure, build, flash:
```bash
$ idf.py menuconfig
$ idf.py build flash monitor
```

To test, using `curl`:
```
$ curl <ip>/current
```

