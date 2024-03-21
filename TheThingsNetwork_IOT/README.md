# Library Starterguide
## Introduction
This starter guide will help you set up your device and get it connected to the network
## Prerequisites
Before you begin, make sure you have the following:
- RN2483 module
- Arduino IDE
- The Things Network account
## Hardware Setup­
Follow these steps to set up the hardware:
- Connect the RN2483 module to your pc via the usb input
- Power on the RN2483 module.
## Software Setup
Follow these steps to set up the software:
- Download and install the Arduino IDE from the Arduino website
- Open the Arduino IDE
- Copy the ‘TheThingsNetwork_IOT’ library in your /Arduino/libraries folder
- Reopen Arduino and open one of the example sketches via File -> Examples -> TheThingsNetwork_IOT
- Replace placeholders such as AppEUI, AppKey and DevEUI., with your credentials obtained from your LoRaWAN network provider
- Upload the sketch to the RN2483
## Testing
Once your hardware and software are set up, it's time to test your device:
- Open the Serial Monitor in the Arduino IDE
- Verify that the device successfully joins the network by checking the output in the Serial Monitor
- Check if the expected data is retrieved at thethingsindustries.com with your TheThingsNetwork account.
