# Publishing data to AWS IoT Core<!--! {#example_aws_iot_core} -->

This example demonstrates publishing data to AWS IoT Core.
It uses a EnviroDIY Wifi Bee (ESP32) and an EnviroDIY Stonefly (SAMD51).

> [!WARNING]
> You should have loaded all your certificates to your modem and tested your connection *before* running this program.
> This program depends on correct certificates being pre-loaded onto the modem.
> Use the AWS_IoT_SetCertificates program in the extras folder to load your certificates.

_______

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Publishing data to AWS IoT Core](#publishing-data-to-aws-iot-core)
  - [Unique Features of the IoT Core Example](#unique-features-of-the-iot-core-example)
  - [To Use this Example](#to-use-this-example)
    - [Set your AWS IoT Core Endpoint](#set-your-aws-iot-core-endpoint)
    - [Set your Thing Name](#set-your-thing-name)
    - [Set your Sampling Feature (Site) ID](#set-your-sampling-feature-site-id)
    - [Set your WiFi Credentials](#set-your-wifi-credentials)
    - [Set your Variable UUIDs](#set-your-variable-uuids)

<!--! @endif -->

_______

## Unique Features of the IoT Core Example<!--! {#example_aws_iot_core_unique} -->

<!--! @todo Document AWS Example -->

TODO

## To Use this Example<!--! {#example_aws_iot_core_using} -->

### Set your AWS IoT Core Endpoint

Find and replace the text `YOUR_ENDPOINT-ats.iot.YOUR_REGION.amazonaws.com` with your real endpoint.
Make sure there are quotation marks around the endpoint string, as there are in the example.
This must be the same value you used in the AWS_IoT_SetCertificates sketch.

### Set your Thing Name

Find and replace the text `YOUR_THING_NAME` with your assigned thing name.
Make sure there are quotation marks around the name string, as there are in the example.
This must be the same value you used in the AWS_IoT_SetCertificates sketch.

### Set your Sampling Feature (Site) ID

Find and replace the text `YOUR_SAMPLING_FEATURE_ID` with your assigned sampling feature ID or UUID.
Make sure there are quotation marks around the name string, as there are in the example.

### Set your WiFi Credentials

Find and replace the text `YourWiFiSSID` with your WiFi name (SSID) and `YourWiFiPassword` with your WiFi password.
Make sure there are quotation marks around the name string, as there are in the example.

### Set your Variable UUIDs

In the section beginning with `Start [variable_arrays]`, find and replace the text `"12345678-abcd-1234-ef00-1234567890ab` with the UUIDs for each of your variables, if they have UUIDs.
Make sure there are quotation marks around the name string, as there are in the example.
If you do not have UUIDs for your variables, delete the string entirely, leaving empty quotes (`""`).

_______

<!--! @section example_aws_iot_core_pio_config PlatformIO Configuration -->

<!--! @include{lineno} AWS_IoT_Core/platformio.ini -->

<!--! @section example_aws_iot_core_code The Complete Code -->

<!--! @include{lineno} AWS_IoT_Core/AWS_IoT_Core.ino -->
