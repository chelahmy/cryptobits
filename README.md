# Cryptobits
### IoT Secure Communication Protocol

Cryptobits is a shared key messaging protocol based on [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) encryption wrapped in JSON. The JSON object contains a property address **a** and a base64 encrypted message **m**. The message contains the *cipher* and the *iv*. A typical JSON object looks like the following:

**{"a":"Terminal 1","m":"qLN5mK0c3yFKiaRmbAvjxwk+QSw+Ch3RC7o8+BxNI8c="}**

AES is sufficiently secured for many IoT implementations. Cryptobits can be implemented in a microcontroller with a small memory and processing footprint. The messages can be delivered through various communication channels such as serial, wireless or WiFi. 
