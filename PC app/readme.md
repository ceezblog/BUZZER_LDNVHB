LED PANEL CONTROLLER App for BUZZER LD NVHB

Visual Studio 2022 project:
- Require Visual Studio 2022 Community Edition
- Target build x64 if running on Intel/AMD laptop or arm64 if intend to run on ARM powered laptop
- Project originally created using VS2013 with traces of MFC although not using MFC
- Ported to VS2022 (tool set v143) with some modification in resouce.rc in order to get rid of MFC completely

Setup Wifi Accesspoint (wifi router)
- The router must have larger than 128MB of ram, usually available with expensive routers, to handle request from 11 clients. Cheap routers usually have 16/32MB of RAM
- Wifi 802.11abgn, SSID: BUZZER, Password: 12344321, Encryption: WPA/WPA2
- Free routers from Optus/Teltra... usually can only handle 4 to 6 clients. The speed is not important but the RAM of the router is.
- Set LAN IP of the router to 10.10.10.253, DHCP range: 10.10.10.1 to 10.10.10.50
- Tell people not to try to connect to the wifi BUZZER as it might crash the wifi
- This router can be connected to the internet (via WAN port) but there is no point to it, except to sastisfy some laptops





![the app](https://github.com/ceezblog/BUZZER_LDNVHB/blob/master/PC%20app/shot.png)

Before starting the app:
- PC to control LED panels must be connected to BUZZER wifi or plug the Ethernet cable to the wifi router
- Make sure the wifi BUZZER is connected, if wifi asks for action, simply ignore it for some router will pop up the message saying you don't have internet connection
- Some laptop will disconnect the wifi without internet connection automatically, so the only option here is connect ethernet cable and turn off wifi

Running the app:
- You will see the app similar to the screenshot above
- Check your IP address: 10.10.10.x is good, if else, check wifi and restart the app
- File session.txt is the data the app using, any change in the app will be saved in this file
- The app will load session.txt in the same folder at start
- Copy session.txt to different name to archive the results if needed
- Load the old session.txt to see old result
- Name/text to display should be less than 10 chars due to limitation of the LCD panel
- Keep data in the same folder with the app as the app loads session file in the same folder when it starts
