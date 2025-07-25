# Sistem Absensi
Sistem absensi berbasis ESP32

## using arduino-cli

### compile

```
sudo arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=no_ota tugasakhir
```

### upload

```
sudo arduino-cli upload -p <PORT> --fqbn esp32:esp32:esp32:PartitionScheme=no_ota tugasakhir
```

## SPIFFS

create spiffs.bin via mkspiffs

```
./mkspiffs -c data/ -b 4096 -p 256 -s 0x180000 spiffs.bin
```

flash via esptool

```
sudo esptool --chip esp32 --port <PORT> write-flash 0x1E0000 spiffs.bin
```

verify

```
esptool --port <PORT> write-flash 0x1000 bootloader.bin 0x10000 app.bin 0x1E0000 spiffs.bin

```
