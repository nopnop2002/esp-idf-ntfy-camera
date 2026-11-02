# esp-idf-ntfy-camera
Take a picture and publish it via ntfy.sh.   
ntfy (pronounced notify) is a simple HTTP-based pub-sub notification service.   
It allows you to send notifications to your phone or desktop via scripts from any computer, and/or using a REST API.   
The ESP32 acts as a REST client.   
You can download this app from [here](https://ntfy.sh/).   

This project use [this](https://components.espressif.com/components/espressif/esp32-camera) Camera Driver.   
<img width="1280" height="720" alt="Image" src="https://github.com/user-attachments/assets/d06fd414-6fe8-45b6-9854-5efc2fb7c25c" />
<img width="1280" height="720" alt="Image" src="https://github.com/user-attachments/assets/9bc03acc-cbed-466d-b11d-877dd4281b6f" />
<img width="1280" height="720" alt="Image" src="https://github.com/user-attachments/assets/0c628d33-f29e-4ba2-8d0a-4ddebf3d7014" />

# Hardware requirements
ESP32 development board with OV2640 camera.   
If you use other camera, edit sdkconfig.default.   
From the left:   
- Aithinker ESP32-CAM   
- Freenove ESP32-WROVER CAM   
- UICPAL ESPS3 CAM   
- Freenove ESP32S3-WROVER CAM (Clone)   

![es32-camera](https://github.com/nopnop2002/esp-idf-websocket-camera/assets/6020549/38dbef9a-ed85-4df2-8d22-499b2b497278)

# Software requirements
- ESP-IDF V5.0 or later.   
	ESP-IDF V4.4 release branch reached EOL in July 2024.   

# Start ntfy.sh
Install ntfy.sh on your smartphone and register the topics you wish to subscribe to.   
Click the plus mark to create a topic.   
By sending messages or pictures via PUT or POST, you enable this device to receive notifications.   
Topics are not password-protected, so please choose a name that is difficult to guess.   
<img width="864" height="576" alt="Image" src="https://github.com/user-attachments/assets/f7245770-1067-4ac3-a724-6a323040afd8" />
<img width="864" height="576" alt="Image" src="https://github.com/user-attachments/assets/e2fdf4e2-9dfb-4ca7-922d-07d937d6a2f1" />

# Installation
For AiThinker ESP32-CAM, you need to use a USB-TTL converter and connect GPIO0 to GND.   

|ESP-32|USB-TTL|
|:-:|:-:|
|U0TXD|RXD|
|U0RXD|TXD|
|GPIO0|GND|
|5V|5V|
|GND|GND|


```
git clone https://github.com/nopnop2002/esp-idf-ntfy-camera
cd esp-idf-ntfy-camera
idf.py set-target {esp32/esp32s3}
idf.py menuconfig
idf.py flash monitor
```

# Start firmware
For AiThinker ESP32-CAM, Change GPIO0 to open and press the RESET button.

# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/cdddeea9-1e5f-4c51-ac61-58335a68f610" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/6b4acb63-6f4d-41fd-a2c1-cd13bb5c4547" />

### Wifi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/f516b542-318e-4db7-b222-f4532e7ad65a" />   
You can connect using the mDNS hostname instead of the IP address.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/6ea94e7b-d640-4a33-be07-057ed7e3c9cb" />   
You can use static IP.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/599594ab-c8dd-4152-9513-861dfb13be15" />   

### NTFY Server Setting
The ESP32 sends pictures to the ntfy.sh via HTTP.   
Specify the topics to subscribe to.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/f98e0c25-4dd1-406a-b1d5-042623e2f9af" />

### Select Board
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/76afd9ef-803c-4d4f-92e9-86a2747effa3" />

### Select Shutter
ESP32 acts as a HTTP server and listens for requests from HTTP clients.   
You can use this command as shutter.   
`curl -X POST http://{ESP32's IP Address}:8080/post`   
If your ESP32's IP address is `192.168.10.157`, it will look like this.   
`curl -X POST http://192.168.10.157:8080/post`   
You can use an mDNS host name instead of an IP address.   
`curl -X POST http://esp32-camera.local:8080/post`

In addition to this, you can select the following triggers:   

- Shutter is the Enter key on the keyboard   
	For operation check.   
	When using the USB port provided by the USB Serial/JTAG Controller Console, you need to enable the following line in sdkconfig.
	```
	CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
	```
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/7ce8d2c4-819f-42a6-838c-a9b9d4d92fee" />


- Shutter is a GPIO toggle   

	- Initial Sate is PULLDOWN   
	The shutter is prepared when it is turned from OFF to ON, and a picture is taken when it is turned from ON to OFF.   

	- Initial Sate is PULLUP   
	The shutter is prepared when it is turned from ON to OFF, and a picture is taken when it is turned from OFF to ON.   

	I confirmed that the following GPIO can be used.   

	|GPIO|PullDown|PullUp|
	|:-:|:-:|:-:|
	|GPIO12|OK|NG|
	|GPIO13|OK|OK|
	|GPIO14|OK|OK|
	|GPIO15|OK|OK|
	|GPIO16|NG|NG|

	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/36eb033f-31ec-43c0-a7de-797312b48240" />

- Shutter is TCP Socket   
	ESP32 acts as a TCP server and listens for requests from TCP clients.   
	You can use tcp_send.py as shutter.   
	`python3 ./tcp_send.py`
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/9060a1dc-e2a2-4c8b-b093-8b16d3a42396" />

- Shutter is UDP Socket   
	ESP32 acts as a UDP listener and listens for requests from UDP clients.   
	You can use this command as shutter.   
	`echo -n "take" | socat - UDP-DATAGRAM:255.255.255.255:49876,broadcast`   
	You can use udp_send.py as shutter.   
	Requires netifaces.   
	`python3 ./udp_send.py`   
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/48cd585c-428e-4f30-bc49-b9da980e0411" />   
	You can use these devices as shutters.   
	![Image](https://github.com/user-attachments/assets/cc97da4e-6c06-4604-8362-f81c6fb6eb58)   
	Click [here](https://github.com/nopnop2002/esp-idf-selfie-trigger) for details.   

- Shutter is MQTT Publish   
	ESP32 acts as an MQTT subscriber and listens to requests from MQTT publishes.   
	You can use mosquitto_pub as shutter.   
	`mosquitto_pub -h your_broker -p 1883 -t "/take/picture" -m ""`   
	You can use publish.py as shutter.   
	`python3 ./publish.py --host your_broker`
	Many MQTT clients that can be used on smartphones are available.   
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/c1740dd6-51eb-45e5-93ca-712e1f2290b4" />

### Flash Light   
ESP32-CAM by AI-Thinker have flash light on GPIO4.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/a495a9fb-a785-4ed3-99e0-fdc354469d85" />

### PSRAM   
When you use ESP32S3-WROVER CAM, you need to change the PSRAM type.   
![config-psram](https://github.com/nopnop2002/esp-idf-websocket-camera/assets/6020549/ba04f088-c628-46ac-bc5b-2968032753e0)

# View picture using Built-in WEB Server
ESP32 works as a web server.   
You can view the pictures taken using the built-in WEB server.   
Enter the ESP32's IP address and port number in the address bar of your browser.   
You can connect using mDNS hostname instead of IP address.   

![browser](https://user-images.githubusercontent.com/6020549/124227364-837a7880-db45-11eb-9d8b-fa15c676adac.jpg)

# View picture using ntfy.sh application
When a picture is taken, you will receive a notification on your smartphone.   
<img width="864" height="576" alt="Image" src="https://github.com/user-attachments/assets/458e9ebc-8a64-4a47-b12d-c60e9f20e03c" />
<img width="864" height="576" alt="Image" src="https://github.com/user-attachments/assets/1cc37748-71aa-46ea-906e-24a7d107e67c" />
