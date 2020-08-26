// client.cpp
// Cryptobits HTTP Client
// By Abdullah Daud, chelahmy@gmail.com
// 19 August 2020

#include <ESP8266HTTPClient.h>
#include "cb_http_client.h"

cb_http_client::cb_http_client() {
  post_response_code = 0;
}

cb_http_client::cb_http_client(String id, String url, String key) {
  _id = id;
  server_url = url;
  server_key = key;
  setKey(key);
  post_response_code = 0;
}

void cb_http_client::setId(String id) {
  _id = id;
}

void cb_http_client::setUrl(String url) {
  server_url = url;
}

bool cb_http_client::setKey(String key) {
  server_key = key;
  if (cbpack.setHexKey((char*)key.c_str()) != 0) return false;
  return true;
}

int cb_http_client::post(String message, String &respond) {
  int stt = 0;
  post_response_code = 0;
  HTTPClient http;
  http.begin(server_url);
  http.addHeader("Content-Type", "application/json");
  int sz = cbpack.calcPackSize((char*)_id.c_str(), (char*)message.c_str());
  if (sz <= 0) sz = 2000;
  char out[sz];
  memset(out, 0, sizeof(out));
  int pstt = cbpack.pack((char*)_id.c_str(), (char*)message.c_str(), out, sizeof(out) - 1);
  post_response_code = http.POST(out); 
  if (post_response_code == 200) {
    String resp = http.getString();
    sz = cbpack.calcUnpackSize((char*)resp.c_str());
    if (sz <= 0) sz = 2000;
    char out2[sz];
    memset(out2, 0, sizeof(out2));
    pstt = cbpack.unpack((char*)resp.c_str(), (char*)_id.c_str(), out2, sizeof(out2) - 1);
    if (pstt == 0) respond = out2;
    else stt = -1;
  }
  else stt = -2;
  http.end();
  return stt;
}

int cb_http_client::getPostResponseCode() {
  return post_response_code;
}
