# eelreless
ESP32 firmware to read power consumption and provide HTTP API access

>To add local name resolution ([mDNS](https://en.wikipedia.org/wiki/Multicast_DNS)), enable at `menuconfig` and run: 
>
>```$ idf.py add-dependency espressif/mdns```
>
>This will add the [mDNS component](https://github.com/espressif/esp-protocols/tree/master/components/mdns) to your project. The default name is **eelreless.local**.

Configure, build, flash:
```bash
$ idf.py menuconfig
$ idf.py build flash monitor
```

To test, using `curl`:
```bash
$ curl <ip>/current
```
Or if mDNS enabled (with name `eelreless`):
```bash
$ curl eelreless.local/current
```

