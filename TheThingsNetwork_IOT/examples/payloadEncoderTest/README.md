# Testing with payload decoder

For testing with a payload decoder of a different project group, some settings need to be changed. 
## Step 1
In the thingsnetwork cloud, go to payload formatters -> uplink. (https://eu1.cloud.thethings.network/console/applications/lorahan/payload-formatters/uplink). Select “Custom Javascript formatter”. In formatter code, paste in the decoder code of a different group, in our case the “decoder_CayenneLPP_extreme.js” file from GitHub - HAN-IoT-LAB/TTN-Decoder: TheThingsNetwork decoder which is linked to the refactored CayenneLPP encoder. Save your changes with the blue button.
!! Delete the very last line of this file, which is: “module.exports = { SensorTypes, decodeUplink };”. 

## Step 2
Go to End devices -> (device) -> payload formatters. In uplink, select Custom Javascript formatter, scroll down and click ‘paste application formatter’. This will copy the same file as above to the formatter code. 
!! Check again if the very last line is not being displayed, otherwise go back to step 1. 

To test the other example codes, reset all the settings to what is was before:
In the thingsnetwork cloud, go to payload formatters -> uplink. Select “none”. Save changes
In the thingsnetwork cloud, go to End devices -> (device) -> payload formatters. In uplink, select “CayenneLPP”. Save changes. 



