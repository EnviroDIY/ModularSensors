# Setting AWS Certificates<!--! {#extra_aws_certificates} -->

This program loads the required certificates for communication with AWS IoT Core onto the EnviroDIY LTE Bee modem.

> [!WARNING]
> Even among modems that have SSL support, not all firmwares have support for the mutual authentication required by IOT core.
> You may need to update your firmware.
> Updating firmware requires obtaining the firmware update files - which can be difficult to find - and may require soldering to access the programming pins of your module.
> The firmware the EnviroDIY LTE Bee's ship with from the factory is *too old* to work correctly with AWS's mutual authentication!
> You should be running firmware version 1951B**17**SIM7080 or later.
> The modems provided with your Stonefly should already have been upgraded to this version.

_______

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Setting AWS Certificates](#setting-aws-certificates)
  - [Advanced Work on AWS](#advanced-work-on-aws)
  - [Customizing the Example Sketch](#customizing-the-example-sketch)
    - [Setup AWS IOT Config](#setup-aws-iot-config)
      - [Set your AWS IoT Core Endpoint](#set-your-aws-iot-core-endpoint)
      - [Set your Thing Name](#set-your-thing-name)
      - [Set your AWS IoT Core Client Certificate](#set-your-aws-iot-core-client-certificate)
      - [Set your AWS IoT Core Client Private Key](#set-your-aws-iot-core-client-private-key)
    - [Customize the Set Certificates program](#customize-the-set-certificates-program)
      - [Select your Modem](#select-your-modem)
      - [Set your cellular APN or Wifi credentials](#set-your-cellular-apn-or-wifi-credentials)
  - [Upload to your Board](#upload-to-your-board)
  - [Monitor the sketch to confirm that you are correctly connected](#monitor-the-sketch-to-confirm-that-you-are-correctly-connected)
  - [Troubleshooting](#troubleshooting)
    - [Compiling or uploading fails](#compiling-or-uploading-fails)
    - [The Arduino fails to communicate with the modem](#the-arduino-fails-to-communicate-with-the-modem)
    - [The certificates fail to load on the modem](#the-certificates-fail-to-load-on-the-modem)
    - [Modem fails to connect to the network](#modem-fails-to-connect-to-the-network)
    - [Modem fails to get LTE data connection](#modem-fails-to-get-lte-data-connection)
    - [SSL (Client) or MQTT connection fails](#ssl-client-or-mqtt-connection-fails)
    - [Publishing fails or the message is not received by the broker](#publishing-fails-or-the-message-is-not-received-by-the-broker)

<!--! @endif -->

_______

## Advanced Work on AWS

Someone needs to generate the certificates and provide them to you, along with the Thing Name = Client ID = Logger ID.

## Customizing the Example Sketch

This "sketch" consists of *TWO* files that you need to modify.
You must edit both files and have both files in the AWS_IoT_SetCertificates folder!

### Setup AWS IOT Config

This file is the configuration file which contains the actual text of the certificates.
Everything in this file is unique to your logger and AWS instance.
**DO NOT SHARE THIS FILE!!**
Keep this file and the items you need below private.
The certificates are like passwords allowing direct access to your AWS IoT Core instance.
Protect this information like any other important password.

Before you start, have these things ready:

- The AWS endpoint (URL) that you will be connecting to
- The thing name given to you for your own Stonefly
  - These will be unique to each user
- The AWS client certificate and private key files that are linked to your thing name
  - These files must be the ones corresponding to your specific thing name!
  - These will be unique for each user.
  - These files will probably be provide in a single zip file that contains your personal certificate, public key, private key, and two Amazon root CA certificates.
    - Unzip the files if necessary - make sure you know where they end up on your computer after unzipping!

Unfortunately, the Stonefly cannot read the certificates from your computer, so you need to copy and paste them here.

#### Set your AWS IoT Core Endpoint

In line 11, find and replace the text `YOUR_ENDPOINT-ats.iot.YOUR_REGION.amazonaws.com` with your real endpoint.
Make sure there are quotation marks around the endpoint string, as there are in the example.

#### Set your Thing Name

In line 13, find and replace the text `YOUR_THING_NAME` with your assigned thing name.
Make sure there are quotation marks around the name string, as there are in the example.

#### Set your AWS IoT Core Client Certificate

From the required files mentioned above, find and open the file that *ends with* "certificate.pem.crt" in a text editor like Notepad.
By default, your computer may want to open the certificate in a web browser.

On Windows:

- Instead of double clicking to open the file, right click and select "Open with."
- Find Notepad in the list of programs to open the file with.
- If Notepad is not in the open list, select "Choose another app" and search for notepad.
- You will get a security warning about opening the file - it's a secure password file.
- Open the file anyway.

Once you have the file open, it should look like a bunch of random characters sandwiched between the lines `-----BEGIN CERTIFICATE-----` and `-----END CERTIFICATE-----`.
Find and replace the text `paste the certificate here` in approximately line 47 of with the text of your certificate.
Make sure that the text begins and ends with the lines `-----BEGIN CERTIFICATE-----` and `-----END CERTIFICATE-----` as it does in the example and in your certificate text.

#### Set your AWS IoT Core Client Private Key

From the required files mentioned above, find and open the file that *ends with* "private.pem.key" in a text editor like Notepad.
As before, you need to see the file in a text editor and you may get a security warning when you open it.

Once you have the file open, it should look like a bunch of random characters sandwiched between the lines `-----BEGIN RSA PRIVATE KEY-----` and `-----END RSA PRIVATE KEY-----`.
Find and replace the text `paste the private key here` in approximately line 71 of with the text of your certificate.
Make sure that the text begins and ends with the lines `-----BEGIN RSA PRIVATE KEY-----` and `-----END RSA PRIVATE KEY-----` as it does in the example and in your certificate text.

> [!NOTE]
> The line number where the private key starts may change based on the length of the certificate pasted above it.

### Customize the Set Certificates program

This is the program the Stonefly will run to feed the certificates onto the modem.
Since all of the private information went into the config file modified above, only the cellular APN may need to be modified.

#### Select your Modem

In lines 27-31 remove the slashes (`//`) before the modem that you want to use and add slashes to all of the others.

#### Set your cellular APN or Wifi credentials

If you are using a cellular modem, in line 78, find and replace the text `hologram` with the APN for your SIM card.
Make sure there are quotation marks around the APN string, as there are in the example.
If you are using a Hologram SIM, you don't need to change this.

If you are using a WiFi modem, in lines 83-84 find and replace `YourWiFiSSID` and `YourWiFiPassword` with your real SSID and password.

## Upload to your Board

After correctly modifying the configuration and set certificates files, upload the sketch to your Arduino board.
Instructions for uploading sketches can be found on the [Arduino support pages](https://support.arduino.cc/hc/en-us/articles/4733418441116-Upload-a-sketch-in-Arduino-IDE).

## Monitor the sketch to confirm that you are correctly connected

After uploading your program, open the serial port monitor to confirm the certificates load and you succeed in connecting to AWS IoT Core's MQTT client.
Instructions for opening the Arduino IDE's serial monitor are on [their support site](https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-serial-monitor/).
These are the messages you should see on the serial monitor as your program runs:

- First there will be a several second delay and wait for the modem to wake.
- After warm up, the modem will initialize and you should see the message `Initializing modem` &hellip; &hellip; `success`.
- Next you will see a print out of the modem info; `Modem Info:` followed by the serial number and other info about your modem. This is just for your information.
- The Stonefly will then load the CA certificate, client certificate, and key onto the modem, convert them to the modem's internal file format, and print back the loaded text so you can confirm it matches later if there are problems.
- After the certificates are loaded, the modem waits for a network connection.  If the connection succeeds, you'll see the message `Waiting for network` &hellip; &hellip; `success`

> [!WARNING]
> The first time you connect to the a new cellular tower with a new SIM card and new modem, the connection may take a **long** time - up to several minutes.

- Now the modem waits gets a LTE internet connection.  If the connection succeeds, you'll see the message `Connecting to hologram` &hellip; `success`
- Once fully connected, the modem will sync its timestamp with NTP. This is required for later secured connections.
- You'll see a message `=== MQTT NOT CONNECTED ===` because you haven't connected yet.
- The modem will connect to MQTT, giving the message `Connecting to YOUR_ENDPOINT with client ID YOUR_THING_NAME` &hellip; `success`
- Finally, the modem will publish a message, telling you: `Publishing a message to YOUR_THING_NAME/init`.
- If all is well, someone watching your AWS IoT Core's MQTT broker will see the message {"YOUR_THING_NAME":"connected"} from your device on the topic `YOUR_THING_NAME/init`.

## Troubleshooting

### Compiling or uploading fails

If the program fails to compile, look at all of the places you pasted information and make sure that the strings are surrounded by quotation marks and each line ends with a semicolon.
Make sure that the `BEGIN` and `END` tags are in place for each certificate.
After the end of the closing tag, the next line will be a `)EOF`.

If the program fails to upload, confirm that your USB cable is tightly connected and you have the right serial port number selected.
If needed, confirm that you've correctly set up the IDE for a your board.

If you've previously programmed your SAMD board (like the Stonefly) with any program (like a logging program) that might tell it to go to a low-power sleep mode, you may need to wake the board in order to program it.
There are some [tips in the Read Me](https://github.com/EnviroDIY/USGS_NGWOS/?tab=readme-ov-file#reprogramming-a-sleeping-logger) for dealing with the sleeping problem.

### The Arduino fails to communicate with the modem

If you see the message `failed to initialize modem`, there's a communication problem between the modem and the Arduino.
If after the failure message and a delay you do see your modem serial number or firmware version after the message `Modem Info:`, you can ignore this error: it was a baud rate problem and the Arduino adjusted.
If you don't get the modem info, there's something wrong.
If your SIM card requires a user name and password to unlock (uncommon), enter those in lines 79 and 80 of the ino file and recompile and re-upload.
Confirm that your wires between the Arduino and your modem are correct and you've set the correct port for `SerialAT` in line 45.
Confirm that your modem has power and that any expected LED's are lit.

### The certificates fail to load on the modem

If the certificates fail to load, it's probably a pasting error.
Confirm you have pasted everything correctly and each certificate begins and ends with plain text begin and end tags.
If the certificates printed back to you are garbled, but you get a success message, the certificates are probably loaded correctly but the delay in going back and forth on the serial monitor caused the garbling.

### Modem fails to connect to the network

Remember, this may take a **long** time to connect for the first time on a cellular network.
Check that there is cell phone or WiFi signal in your location.
If you are using a cellular connection with a Hologram SIM; they prefer to connect to T-Mobile; ask around how good the T-Mobile signal is where you are.
Move somewhere else if you need to.
Confirm that your SIM card is installed tightly and correctly in the modem's SIM card slot.
Also confirm that the APN is set correctly for your SIM card.
Check that the wire from antenna is securely connected to the bee.
Remove any wires (especially power wires) that are too close to the antenna; don't lay the antenna on top of the USB cable from your computer to your Arduino.
If you got no other error messages, but the modem won't connect, restart and let it try again.
The first connection with a new combination of SIM card, tower, and device can be painfully slow.

### Modem fails to get LTE data connection

Check your APN.
It must match the APN assigned by your SIM card provider.
Check your SIM username and password, if your SIM requires them (uncommon).

### SSL (Client) or MQTT connection fails

Check your pasted certificates.
Check your thing name.
Cross check that the certificates are those valid for your thing name.
Have someone with permissions on the AWS account check that the security policies allow your combination of certificates and thing name to make an MQTT connection.
Also confirm with your AWS administrator that they have not assigned the same certificates and thing name to a second user.
Two devices with the same client id may not connect at the same time.
In this case, we use the thing name as the client id.

Scroll up in the serial monitor output and look for the `Modem Revision:` output.
Confirm that the firmware version you have supports mutual authentication.
If the firmware revision is not 1951B**17**SIM7080 or later, your modem firmware must be upgraded!
Upgrading the firmware on the LTE Bee requires solder connections and special firmware files; contact Stroud for this.

### Publishing fails or the message is not received by the broker

At this time, only the AWS administrator or someone with access to the IoT Core MQTT test client will be able to confirm if the message is received, but you should see an error if the publishing fails on the device side.
Have your AWS administrator confirm that your thing name and certificates give you permission to publish to the topic `YOUR_THING_NAME/init`.
The `init` topic is used in this example, but not in the regular data publishing in the AWS_IoT_Core example.
