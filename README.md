# Cryptobits
### IoT Secure Communication Protocol

Cryptobits is a shared key messaging protocol based on [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) encryption wrapped in JSON. The JSON object contains a property address **a** and a base64 encrypted message **m**. The message contains the *cipher* and the *iv*. A typical JSON object looks like the following:

**{"a":"Terminal 1","m":"qLN5mK0c3yFKiaRmbAvjxwk+QSw+Ch3RC7o8+BxNI8c="}**

AES is sufficiently secured for many IoT implementations. Cryptobits can be implemented in a microcontroller with a small memory and processing footprint. The messages can be delivered through various communication channels such as serial, wireless or WiFi. 

### HTTP Client

The Cryptobits HTTP client is an implementation for [Arduino on ESP8266](https://github.com/esp8266/Arduino). It needs a cryptobits server on the other side. There is a PHP example.

The following is a method to send and receive secure messages. The device must already be connected to the network before calling this:
```cpp
#include <cb_http_client.h>

String id = "Terminal 1";
String server_url = "http://192.168.0.1/test.php";
String shared_key = "2B7E151628AED2A6ABF7158809CF4F3C";

cb_http_client my_client = cb_http_client(id, server_url, shared_key);

String response;
int stt = my_client.post("My message", response);
Serial.println(String("Post status: ") + stt);
Serial.println(String("Post response: ") + response);
```
*stt* must return 0. Otherwise, please check the response code:
```cpp
int rc = my_client.getPostResponseCode();
Serial.println(String("Post response code: ") + rc);
```
The code is defined in [ESP8266HTTPClient.h](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h).
