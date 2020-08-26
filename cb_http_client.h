// client.h
// Cryptobits HTTP Client
// By Abdullah Daud, chelahmy@gmail.com
// 19 August 2020

#ifndef __CB_HTTP_CLIENT__
#define __CB_HTTP_CLIENT__

#include <cb_packager.h>

class cb_http_client {
  private:
    cb_packager cbpack;
    String _id;
    String server_url;
    String server_key;
    int post_response_code;
  public:
    cb_http_client();
    cb_http_client(String id, String url, String key);
    void setId(String id);
    void setUrl(String url);
    bool setKey(String key);
    int post(String message, String &respond);
    int getPostResponseCode();
};

#endif
