// client.cpp
// Cryptobits HTTP Client
// By Abdullah Daud, chelahmy@gmail.com
// 19 August 2020

#include <ESP8266HTTPClient.h>
#include "cb_http_client.h"

cb_http_client::cb_http_client() {
  packing_status = 0;
  post_response_code = 0;
}

cb_http_client::cb_http_client(String id, String url, String key) {
  _id = id;
  server_url = url;
  shared_key = key;
  setKey(key);
  packing_status = 0;
  post_response_code = 0;
}

void cb_http_client::setId(String id) {
  _id = id;
}

void cb_http_client::setUrl(String url) {
  server_url = url;
}

bool cb_http_client::setKey(String key) {
  shared_key = key;
  if (cbpack.setHexKey((char*)key.c_str()) != 0) return false;
  return true;
}

int cb_http_client::post(String message, String &respond) {
  packing_status = 0;
  post_response_code = 0;
  int stt = 0, trials = 3;
  int sz = cbpack.calcPackSize((char*)_id.c_str(), (char*)message.c_str());
  if (sz <= 0) sz = 2000;
  char out[sz];
  memset(out, 0, sizeof(out));
  packing_status = cbpack.pack((char*)_id.c_str(), (char*)message.c_str(), out, sizeof(out) - 1);
  if (packing_status != 0) return -1;
  HTTPClient http;
  http.begin(server_url); // Note: this begin() method is being deprecated.
  http.addHeader("Content-Type", "application/json");
  post_response_code = http.POST(out);
  while (post_response_code < 0) { // system or network error
    if (--trials <= 0) break;
    delay(1000);
    post_response_code = http.POST(out);
  } 
  if (post_response_code == 200) {
    String resp = http.getString();
    sz = cbpack.calcUnpackSize((char*)resp.c_str());
    if (sz <= 0) sz = 2000;
    char out2[sz];
    memset(out2, 0, sizeof(out2));
    packing_status = cbpack.unpack((char*)resp.c_str(), (char*)_id.c_str(), out2, sizeof(out2) - 1);
    if (packing_status == 0) respond = out2;
    else stt = -2;
  }
  else stt = -3;
  http.end();
  return stt;
}

int cb_http_client::getPackingStatus() {
  return packing_status;
}

int cb_http_client::getPostResponseCode() {
  return post_response_code;
}
