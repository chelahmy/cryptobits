// cb_json.h
// Cryptobits JSON Parser
// By Abdullah Daud, chelahmy@gmail.com
// 26 August 2020

#ifndef __cb_json__
#define __cb_json__

class cb_json {
  private:
    bool ocat(char chr, char *out, int out_len);
    char *skipSpaces(char *jstr);
    char *skipN(char *jstr, int n, char *out, int out_len);
    char *skipColon(char *jstr);
    char *skipStr(char *jstr, char *out, int out_len);
    bool isDigit(char chr);
    bool isNumber(char chr);
    char *skipNumber(char *jstr, char *out, int out_len);
    char *skipObject(char *jstr, char *out, int out_len);
    char *skipArray(char *jstr, char *out, int out_len);
    char *skipValue(char *jstr, char *out, int out_len);
    char *start(char *jstr);
  public:
    cb_json();
    unsigned char hex2Byte(char *hex);
    bool isTrue(char *jstr);
    bool isFalse(char *jstr);
    bool isNull(char *jstr);
    int getProp(char *jstr, char *prop, char *out, int out_len);
    int getItem(char *jstr, int index, char *out, int out_len);
};

#endif
