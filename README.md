# smartwatch
A smartwatch and it's Android application, built for Raspberry Pi Pico microcontroller.

For the android application see [Smartwatch App](https://github.com/umutsevdi/smartwatch-app).

## Installation
* Clone this repository
```bash
$ git clone https://github.com/umutsevdi/smartwatch.git
$ cd pico-sdk
$ git submodule  update --init
$ cd ..
```
* Install the Waveshare Touchscreen driver for Raspberry Pi Pico.
```bash
$ wget https://files.waveshare.com/upload/3/3e/1.28inch_Touch_LCD_Pico.zip
$ unzip 1.28inch_Touch_LCD_Pico.zip
$ mv 1.28inch_Touch_LCD_Pico/c/ src/waveshare
$ rm 1.28inch_Touch_LCD_Pico* -rf
```
* Start the compiling container.
```bash
$ docker-compose up -d && docker exec -it picobox bash
```
* Compile the program. In the container
```bash
$ mkdir -p /app/build
$ cd /app/build
$ cmake ..
$ make
$ mv /app/build/src/smartwatch.uf2 /path/to/pico-driver # (mount it to the docker-compose if you didn't)
```
