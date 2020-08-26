// cb_json.cpp
// Cryptobits JSON Parser
// By Abdullah Daud, chelahmy@gmail.com
// 26 August 2020

#include <stdlib.h>
#include <string.h>
#include "cb_json.h"

cb_json::cb_json() {
  
}

// Concatenate chr to out buffer
// out buffer must have initialized with 0
// return false on out of out buffer
bool cb_json::ocat(char chr, char *out, int out_len) {
  if (out == NULL) return true;
  for (int i = 0; i < (out_len - 1); i++, ++out) {
    if (*out == '\0') {
      *out = chr; ++out; *out = '\0';
      return true;
    }
  }
  return false;
}

unsigned char cb_json::hex2Byte(char *hex) {
  if (hex == NULL || strlen(hex) < 2) return (unsigned char)0x00;
  char buff[3];
  memset(buff, 0, sizeof(buff));
  memcpy(buff, hex, 2);
  return (unsigned char)strtol(buff, NULL, 16);
}

bool cb_json::isTrue(char *jstr) {
  if (jstr == NULL) return false;
  if (strncmp(jstr, "true", 4) == 0) return true;
  return false;
}

bool cb_json::isFalse(char *jstr) {
  if (jstr == NULL) return false;
  if (strncmp(jstr, "false", 5) == 0) return true;
  return false;
}

bool cb_json::isNull(char *jstr) {
  if (jstr == NULL) return false;
  if (strncmp(jstr, "null", 4) == 0) return true;
  return false;
}

char *cb_json::skipSpaces(char *jstr) {
  if (jstr == NULL) return NULL;
  while (*jstr == ' ' || *jstr == '\n' || *jstr == '\r' || *jstr == '\t')
    ++jstr;
  return jstr;  
}

char *cb_json::skipN(char *jstr, int n, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  if (n <=0) return jstr;
  if (n >= (out_len - 1)) return NULL;
  memcpy(out, jstr, n);
  return jstr + n;
}

char *cb_json::skipColon(char *jstr) {
  if (jstr == NULL) return NULL;
  jstr = skipSpaces(jstr);
  if (*jstr != ':') return NULL;
  return skipSpaces(++jstr);
}

char *cb_json::skipStr(char *jstr, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  jstr = skipSpaces(jstr);
  if (jstr == NULL | *jstr != '"') return NULL;
  ++jstr;
  while (*jstr != '\0') {
    if (*jstr == '"')
      return ++jstr;
    char ch = *jstr, ch2, dch = (char)0x00;
    if (*jstr == '\\') {
      ++jstr;
      if (*jstr == '\0') return NULL;
      ch = *jstr;
      if (*jstr == 'b') ch = '\b';
      else if (*jstr == 'f') ch = '\f';
      else if (*jstr == 'n') ch = '\n';
      else if (*jstr == 'r') ch = '\r';
      else if (*jstr == 't') ch = '\t';
      else if (*jstr == '"') ch = '"';
      else if (*jstr == '\\') ch = '\\';
      else if (*jstr == '/') ch = '/';
      else if (*jstr == 'u') { // expecting 4 hex digits
        ++jstr;
        if (strlen(jstr) < 4) return NULL;
        dch = (char)0x01;
        ch = hex2Byte(jstr); jstr += 2;
        ch2 = hex2Byte(jstr); ++jstr;
      }
      else {
        ++jstr; // ignore other control codes
        continue;
      }
    }
    if (out != NULL){
      if (!ocat(ch, out, out_len)) return NULL; // not enough buffer
      if (dch == (char)0x01) {
        if (!ocat(ch2, out, out_len)) return NULL; // not enough buffer
      }
    }
    ++jstr;
  }
  return NULL;  
}

bool cb_json::isDigit(char chr) {
  if (chr >= '0' && chr <= '9') return true;
  return false;  
}

bool cb_json::isNumber(char chr) {
  if (chr == '-') return true;
  return isDigit(chr);
}

char *cb_json::skipNumber(char *jstr, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  jstr = skipSpaces(jstr);
  if (jstr == NULL) return NULL;
  if (!isNumber(*jstr)) return NULL;
  if (!ocat(*jstr, out, out_len)) return NULL; // not enough buffer
  ++jstr;
  bool has_digit = false;
  while (*jstr != '\0') {
    if (isDigit(*jstr)) {
      if (!ocat(*jstr, out, out_len)) return NULL;
      has_digit = true;
    }
    else if (*jstr == '.') {
      if (!has_digit) return NULL; // bad number
      if (*(jstr+1) != '\0') { // look ahead for more number (*)
        if (!isDigit(*(jstr+1))) return jstr; // end of number
      }
      if (!ocat(*jstr, out, out_len)) return NULL;
      ++jstr;
      has_digit = false;
      while (*jstr != '\0') {
        if (isDigit(*jstr)) {
          if (!ocat(*jstr, out, out_len)) return NULL;
          has_digit = true;
        }
        else if (*jstr == 'E' || *jstr == 'e') {
          if (!has_digit) return jstr; // end of number (unlikely see (*) above)
          if (!ocat(*jstr, out, out_len)) return NULL;
          ++jstr;
          has_digit = false;
          bool has_sign = false;
          while (*jstr != '\0') {
            if (*jstr == '+' || *jstr == '-') {
              if (has_digit || has_sign) return jstr; // end of number
              if (!ocat(*jstr, out, out_len)) return NULL;
              has_sign = true;
            }
            else if (isDigit(*jstr)) {
              if (!ocat(*jstr, out, out_len)) return NULL;
              has_digit = true;
            }
            else return jstr; // end of number
            ++jstr;
          }
          return NULL; // bad json
        }
        else return jstr; // end of number
        ++jstr;
      }
      return NULL; // bad json
    }
    else return jstr; // end of number
    ++jstr;
  }
  return NULL; // bad json
}

char *cb_json::skipObject(char *jstr, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  int jstr_len = strlen(jstr);
  if (jstr_len <= 0) return NULL;
  jstr = skipSpaces(jstr);
  if (jstr == NULL || *jstr != '{') return NULL;
  if (!ocat(*jstr, out, out_len)) return NULL;
  ++jstr;
  char buff[jstr_len];
  int lout, lbuff;
  while (jstr != NULL && *jstr != '\0' && *jstr != '}') {
    jstr = skipStr(jstr, buff, sizeof(buff));
    if (jstr == NULL) return NULL;
    if (!ocat('"', out, out_len)) return NULL;
    if (out != NULL && out_len > 0) {
      lout = strlen(out);
      lbuff = strlen(buff);
      if ((lout + lbuff) >= out_len) return NULL;
      strcat(out, buff);
    }
    if (!ocat('"', out, out_len)) return NULL;
    jstr = skipColon(jstr);
    if (jstr == NULL) return NULL;
    if (!ocat(':', out, out_len)) return NULL;
    bool quoted = *jstr == '"' ? true : false;
    jstr = skipValue(jstr, buff, sizeof(buff));
    if (jstr == NULL) return NULL;
    if (quoted)
      if (!ocat('"', out, out_len)) return NULL;
    if (out != NULL && out_len > 0) {
      lout = strlen(out);
      lbuff = strlen(buff);
      if ((lout + lbuff) >= out_len) return NULL;
      strcat(out, buff);
    }
    if (quoted)
      if (!ocat('"', out, out_len)) return NULL;
    jstr = skipSpaces(jstr);
    if (jstr == NULL) return NULL;
    if (*jstr == ',') {
      if (!ocat(',', out, out_len)) return NULL;
    }
    else if (*jstr == '}') {
      if (!ocat('}', out, out_len)) return NULL;
      return ++jstr;
    }
     ++jstr;
  }
  return NULL;
}

char *cb_json::skipArray(char *jstr, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  int jstr_len = strlen(jstr);
  if (jstr_len <= 0) return NULL;
  jstr = skipSpaces(jstr);
  if (jstr == NULL || *jstr != '[') return NULL;
  if (!ocat(*jstr, out, out_len)) return NULL;
  ++jstr;
  char buff[jstr_len];
  int lout, lbuff;
  while (jstr != NULL && *jstr != '\0' && *jstr != ']') {
    jstr = skipSpaces(jstr);
    if (jstr == NULL) return NULL;
    bool quoted = *jstr == '"' ? true : false;
    jstr = skipValue(jstr, buff, sizeof(buff));
    if (jstr == NULL) return NULL;
    if (quoted)
      if (!ocat('"', out, out_len)) return NULL;
    if (out != NULL && out_len > 0) {
      lout = strlen(out);
      lbuff = strlen(buff);
      if ((lout + lbuff) >= out_len) return NULL;
      strcat(out, buff);
    }
    if (quoted)
      if (!ocat('"', out, out_len)) return NULL;
    jstr = skipSpaces(jstr);
    if (jstr == NULL) return NULL;
    if (*jstr == ',') {
      if (!ocat(',', out, out_len)) return NULL;
    }
    else if (*jstr == ']') {
      if (!ocat(']', out, out_len)) return NULL;
      return ++jstr;
    }
     ++jstr;
  }
  return NULL;
}

char *cb_json::skipValue(char *jstr, char *out, int out_len) {
  if (out != NULL && out_len > 0)
    memset(out, 0, out_len);
  jstr = skipSpaces(jstr);
  if (jstr == NULL) return NULL;
  if (*jstr == '"') return skipStr(jstr, out, out_len);
  if (*jstr == '{') return skipObject(jstr, out, out_len);
  if (*jstr == '[') return skipArray(jstr, out, out_len);
  if (isNumber(*jstr)) return skipNumber(jstr, out, out_len);
  if (isTrue(jstr)) return skipN(jstr, 4, out, out_len);
  if (isFalse(jstr)) return skipN(jstr, 5, out, out_len);
  if (isNull(jstr)) return skipN(jstr, 4, out, out_len);
  return NULL;
}

char *cb_json::start(char *jstr) {
  if (jstr == NULL) return NULL;
  while (*jstr != '\0') {
    if (*jstr == '{' || *jstr == '[')
      return jstr;
    ++jstr;
  }
  return NULL;  
}

/**
 * Get a property value from a json object.
 * Params:
 * jstr - json string object
 * prop - property name
 * out - property value buffer
 * out_len - buffer size
 * Return:
 * The property value set into the out buffer.
 *  0 - property value copied into the out buffer.
 * -1 - parameter error.
 * -2 - jstr is not a json object.
 * -3 - buffer too short.
 * -4 - property does not exist.
 */
int cb_json::getProp(char *jstr, char *prop, char *out, int out_len) {
  if (jstr == NULL || prop == NULL || out == NULL || out_len <= 0) return -1;
  int jstr_len = strlen(jstr);
  if (jstr_len <= 0) return -1;
  int prop_len = strlen(prop);
  if (prop_len <= 0) return -1;
  jstr = start(jstr);
  if (jstr == NULL || *jstr != '{') return -2;
  ++jstr;
  char buff[jstr_len];
  bool matched = false;
  while (jstr != NULL && *jstr != '\0' && *jstr != '}') {
    jstr = skipStr(jstr, buff, sizeof(buff));
    if (jstr == NULL) return -2;
    if (strcmp(prop, buff) == 0)
      matched = true;
    jstr = skipColon(jstr);
    jstr = skipValue(jstr, buff, sizeof(buff));
    if (jstr == NULL) return -2;
    if (matched) {
      int val_len = strlen(buff);
      if (val_len > out_len) return -3;
      memset(out, 0, out_len);
      memcpy(out, buff, val_len);
      return 0;
    }
    jstr = skipSpaces(jstr);
    if (jstr == NULL || *jstr != ',') {
      if (*jstr != '}') return -2;
    }
    ++jstr;
  }
  return -4;
}

/**
 * Get an item from a json array.
 * Params:
 * jstr - json string array
 * index - item index
 * out - item buffer
 * out_len - buffer size
 * Return:
 * The item set into the out buffer.
 *  0 - item copied into the out buffer.
 * -1 - parameter error.
 * -2 - jstr is not a json array.
 * -3 - buffer too short.
 * -4 - item index not exist.
 */
int cb_json::getItem(char *jstr, int index, char *out, int out_len) {
  if (jstr == NULL || out == NULL || out_len <= 0) return -1;
  int jstr_len = strlen(jstr);
  if (jstr_len <= 0) return -1;
  jstr = start(jstr);
  if (jstr == NULL || *jstr != '[') return -2;
  ++jstr;
  char buff[jstr_len];
  int i = 0;
  while (jstr != NULL && *jstr != '\0' && *jstr != ']') {
    jstr = skipValue(jstr, buff, sizeof(buff));
    if (jstr == NULL) return -2;
    if (index == i++) {
      int val_len = strlen(buff);
      if (val_len > out_len) return -3;
      memset(out, 0, out_len);
      memcpy(out, buff, val_len);
      return 0;
    }
    jstr = skipSpaces(jstr);
    if (jstr == NULL || *jstr != ',') {
      if (*jstr != ']') return -2;
    }
    ++jstr;
  }
  return -4;
}

