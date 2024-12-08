/**
 * @file      main.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */
#include <Arduino.h>
//#include <WiFi.h>
//#include <WiFiMulti.h>
//#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
//#include <Arduino.h>
//#include <Adafruit_TinyUSB.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
//#include "camera_pins.h"
#include <base64.h> // Include the Base64 library

#define RXPin 17
#define TXPin 18
#define CHUNK_SIZE 512  // Define the size of each chunk to send
#define CAMERA_ON
#define USE_WIFI
//#define SDCARD_IMAGE
//#define WAVESHARE
#define CAMERA_IMAGE
//#define WAVESHARE_SDCARD
#define LILYGO
#define GSM_TEST
//#define LILYGO_SDCARD


#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"
#include "utilities.h"

const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";



void        startCameraServer();

XPowersPMU  PMU;
//WiFiMulti   wifiMulti;
String      hostName = "LilyGo-Cam-";
String      ipAddress = "";
bool        use_ap_mode = false;

// Wi-Fi credentials
const char* ssid = "Ngoc My";
const char* password = "0857270632";

// Flask server URL
const char* serverName = "sunnyiot.org"; // Change to your Flask server URL
unsigned long startAttemptTime;









void uploadImageFromSDCard() {
    const char* myDomain = "sunnyiot.org";

   #ifdef CAMERA_IMAGE
    // Capture the image from the camera    
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }    
    // Encode the image to Base64
    String encoded = base64::encode(fb->buf, fb->len);
    Serial.printf("Image size: %d bytes\n", fb->len);
    Serial.printf("Encoded size: %d bytes\n", encoded.length());
    #endif    
    
    // Initialize WiFi client
    WiFiClientSecure client;
    client.setCACert(test_root_ca); 
    client.setInsecure(); // Disable SSL certificate verification (not recommended for production)
    
    if (!client.connect(myDomain, 443)) { // Connect to the server
        Serial.println("Connection to server failed");
        return;
    } else {
        Serial.println("Connected to server");
    }

    Serial.println("SEND DATA IN CHUNKS");
    int position = 0; // Position for tracking the current location in the Base64 string
    while (position < encoded.length()) {
        int chunk_size = min(CHUNK_SIZE, static_cast<int>(encoded.length() - position)); // Ensure chunk size is valid
        String jsonBody = "{\"image_chunk\":\"" + encoded.substring(position, position + chunk_size) + "\"}";

        // Construct the POST request
        String httpRequest = String("POST /upload64 HTTP/1.1\r\n") +
                             "Host: " + myDomain + "\r\n" +
                             "Content-Type: application/json\r\n" +
                             "Content-Length: " + jsonBody.length() + "\r\n" +
                             "Connection: keep-alive\r\n\r\n" +
                             jsonBody;

        // Send the request
        client.print(httpRequest);

        // Wait for the server response
        while (client.connected() || client.available()) {
            String line = client.readStringUntil('\n');
            if (line.length() == 0) {
                break; // Empty line indicates headers are complete
            }
        }

        // Read the response
        String response = client.readString();
        Serial.println("Response: " + response);

        // Move to the next chunk
        position += chunk_size;
    }

    // Now call the finish endpoint to process the image
    String finishRequest = String("POST /upload_finish HTTP/1.1\r\n") +
                           "Host: " + myDomain + "\r\n" +
                           "Content-Type: application/json\r\n" +
                           "Content-Length: 0\r\n" +
                           "Connection: close\r\n\r\n"; // Use "close" here to ensure connection closes after response

    client.print(finishRequest);

    // Wait for a response
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line.length() == 0) {
            break; // Empty line indicates headers are complete
        }
    }

    // Read the finish response
    String finishResponse = client.readString();
    Serial.println("Finish Response: " + finishResponse);

    Serial.println("Image upload completed");

    #ifdef CAMERA_IMAGE
    esp_camera_fb_return(fb);
    #endif
}




















void setup()
{

    Serial.begin(115200);
    while (!Serial);
    delay(3000);
    Serial.println("START APP");
    
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
        Serial.println("Failed to initialize power.....");
        while (1) {
            delay(5000);
        }
    }
    //Set the working voltage of the camera, please do not modify the parameters
    PMU.setALDO1Voltage(1800);  // CAM DVDD  1500~1800
    PMU.enableALDO1();
    PMU.setALDO2Voltage(2800);  // CAM DVDD 2500~2800
    PMU.enableALDO2();
    PMU.setALDO4Voltage(3000);  // CAM AVDD 2800~3000
    PMU.enableALDO4();

    // TS Pin detection must be disable, otherwise it cannot be charged
    PMU.disableTSPinMeasure();

    
    Serial.print("Start Wi-Fi");    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");    
    startAttemptTime = millis();
    // Loop to wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        // Check if more than 10 seconds have passed
        if (millis() - startAttemptTime > 10000) { // 10 seconds timeout
            Serial.println("Connection attempt timed out. Restarting...");
            ESP.restart(); // Restart the ESP32
        }
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi!");

    /*********************************
     *  step 3 : Initialize camera
    ***********************************/
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (config.pixel_format == PIXFORMAT_JPEG) {
        if (psramFound()) {
            config.jpeg_quality = 10;
            config.fb_count = 2;
            config.grab_mode = CAMERA_GRAB_LATEST;
        } else {
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }

    } else {
        // Best option for face detection/recognition
        config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
        config.fb_count = 2;
#endif
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x Please check if the camera is connected well.", err);
        while (1) {
            delay(5000);
        }
    }

    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1); // flip it back
        s->set_brightness(s, 1); // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    if (config.pixel_format == PIXFORMAT_JPEG) {
        s->set_framesize(s, FRAMESIZE_QVGA);
    }

#if defined(LILYGO_ESP32S3_CAM_PIR_VOICE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif



    /*********************************
     *  step 4 : start camera web server
    ***********************************/
    //startCameraServer();

}

void loop()
{
    delay(30000);
    uploadImageFromSDCard();
}
