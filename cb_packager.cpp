// cb_packager.cpp
// Cryptobits Packager
// By Abdullah Daud, chelahmy@gmail.com
// 22 August 2020

#include "cb_packager.h"

cb_packager::cb_packager() {
  memset(_key, 0, sizeof(_key));
}

cb_packager::cb_packager(byte key[]) {
  memcpy(_key, key, sizeof(_key));
}

int cb_packager::cleanHex(char *hex, char *out, int out_len) {
  if (hex == NULL || out == NULL || out_len <= 0) return -1;
  int hex_len = strlen(hex);
  if (hex_len <= 0) return -1;
  memset(out, 0, out_len);
  while (*hex != '\0' && out_len > 0) {
    if ((*hex >= '0' && *hex <= '9') ||
      (*hex >= 'A' && *hex <= 'F') ||
      (*hex >= 'a' && *hex <= 'f')) {
      	*out = *hex;
      	++out;
      	--out_len;
      }
    ++hex;  
  }
  if (out_len <= 0 && *hex != '\0') return -2;
  return 0;
}

int cb_packager::bytes2Hex(byte data[], int data_len, char *out, int out_len) {
  if (data_len <= 0 || out == NULL || out_len <= (data_len * 2) + 1) return -1;
  int i;
  for (i = 0; i < data_len && out_len > 0; i++, --out_len) {
    byte nib1 = (data[i] >> 4) & 0x0F;
    byte nib2 = (data[i] >> 0) & 0x0F;
    *out = (char)(nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA); out++;
    if (--out_len <= 0) break;
    *out = (char)(nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA); out++;
  }
  if (out_len <= 0 && i < data_len) return -2;
  return 0;
}

int cb_packager::hex2Bytes(char *hex, byte out[], int out_len) {
  if (hex == NULL || out_len <= 0) return -1;
  char buff[3];
  memset(out, 0, out_len);
  int i;
  for (i = 0; i < out_len && *hex != '\0'; i++, hex++) {
    memset(buff, 0, 3);
    memcpy(buff, hex, 2);
    out[i] = (byte)strtol(buff, NULL, 16);
    ++hex;
    if (*hex == '\0') break;
  }
  if (i < out_len) return -2;
  return 0;
}

void cb_packager::setKey(byte key[]) {
  memcpy(_key, key, sizeof(_key));
}

int cb_packager::setHexKey(char *hex) {
  if (hex == NULL) return -1;
  int hex_len = strlen(hex);
  if (hex_len <= 0) return -1;
  char out[hex_len + 1];
  memset(out, 0, hex_len + 1);
  if (cleanHex(hex, out, hex_len) != 0) return -2;
  int out_len = strlen(out);
  if (out_len < (sizeof(_key) * 2)) return -3;
  if (hex2Bytes(out, _key, sizeof(_key)) != 0) return -4;
  return 0;
}

uint8_t cb_packager::getrnd()
{
   uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
   return really_random;
}

void cb_packager::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) getrnd();
    }
}

// aes-128-cbc encryption
// return base64 string of <cipher+iv>
// NOTE: Must include the NULL terminating character to aes.do_aes_encrypt().
//       Otherwise, some strings will not be encrypted properly.
int cb_packager::encrypt(char *msg, char *out, int out_len) {
  if (msg == NULL || out == NULL || out_len <= 0) return -1;
  int msg_len = strlen(msg);
  if (msg_len <= 0) return -1;
  int bufflen = (msg_len * 5) / 3; // b64 encoding
  if (bufflen < (N_BLOCK * 2)) bufflen = (N_BLOCK * 2);
  char b64data[bufflen + (N_BLOCK * 2)]; // +iv +padding
  byte cipher[bufflen];
  memset(b64data, 0, sizeof(b64data));
  memset(cipher, 0, sizeof(cipher));

  byte iv[N_BLOCK], iv2[N_BLOCK];
  gen_iv(iv);
  memcpy(iv2, iv, N_BLOCK); // iv2 will be changed by aes.do_aes_encrypt()

  AES aes;
  int adj_len = msg_len + 1; // Include NULL terminating character
  
  // Encrypt! With AES128, our key and IV, CBC and pkcs7 padding.
  // Shift cipher N_BLOCK bytes to the right to make room for iv.
  aes.do_aes_encrypt((byte *)msg, adj_len, cipher, _key, 128, iv2);

  int csize = aes.get_size();
  memcpy(&(cipher[csize]), iv, N_BLOCK); // append iv    
  base64_encode(b64data, (char *)cipher, csize + N_BLOCK);  
  if (strlen(b64data) >= out_len) return -2;
  memcpy(out, b64data, out_len);
  return 0;  
}

// aes-128-cbc decryption
// msg is base64 string of <cipher+iv> as returned by encrypt()
// out buffer can be smaller than msg_len but not too small.
// return decrypted string
int cb_packager::decrypt(char *msg, char *out, int out_len) {
  if (msg == NULL || out == NULL || out_len <= 0) return -1;
  int msg_len = strlen(msg);
  if (msg_len <= 0) return -1;
  char buff[msg_len];
  byte plain[msg_len];
  memset(buff, 0, sizeof(buff));
  memset(plain, 0, sizeof(plain));
  int buff_len = base64_decode(buff, msg, msg_len);
  char *pl = buff;
  byte *iv = (byte*)&(buff[buff_len - N_BLOCK]);
  AES aes;
  aes.do_aes_decrypt((byte *)pl, buff_len - N_BLOCK, plain, _key, 128, iv);
  // strip out pads 
  int plain_len = strlen((char*)plain);
  if (aes.CheckPad(plain, plain_len)) {
    if (plain[plain_len-1] <= 0x0f){
      int pad_len = (int)plain[plain_len-1];
      for (int i = plain_len - 1; i >= plain_len - pad_len; i--)
        plain[i] = '\0'; 
      plain_len = strlen((char*)plain); // recalculate length
    }
  }
  if (plain_len >= out_len) return -2;
  memcpy(out, plain, out_len);
  return 0;
}

int cb_packager::calcPackSize(char *a, char *m) {
  if (a == NULL || m == NULL) return -1;
  int a_len = strlen(a);
  int m_len = strlen(m);
  if (a_len <= 0 || m_len <= 0) return -1;
  return a_len + (((m_len + 32) * 5) / 3) + 15;
}

// Pack a and m into cryptobits json package.
// Return the package into out buffer.
int cb_packager::pack(char *a, char *m, char *out, int out_len) {
  if (a == NULL || m == NULL || out == NULL || out_len <= 0) return -1;
  int a_len = strlen(a);
  int m_len = strlen(m);
  if (a_len <= 0 || m_len <= 0) return -1;
  int buff_len = ((m_len + 32) * 5) / 3; // base4 cipher+iv
  char buff[buff_len];
  memset(buff, 0, sizeof(buff));
  int stt = encrypt(m, buff, sizeof(buff) - 1);
  if (stt != 0) return -2;
  int lbuff = strlen(buff);
  if (lbuff <= 0) return -2;
  if (out_len <= (a_len + lbuff + 15)) return -3;  
  memset(out, 0, out_len);
  strcpy(out, "{\"a\":\"");
  strcat(out, a);
  strcat(out, "\",\"m\":\"");
  strcat(out, buff);
  strcat(out, "\"}"); 
  return 0;
}

int cb_packager::calcUnpackSize(char *pkg) {
  if (pkg == NULL) return -1;
  int pkg_len = strlen(pkg);
  if (pkg_len <= 0) return -1;
  char buff[pkg_len];
  memset(buff, 0, sizeof(buff));
  cb_json j;
  int stt = j.getProp(pkg, "m", buff, sizeof(buff) - 1);
  if (stt != 0) return -2; // not a proper package
  int buff_len = strlen(buff);
  return ((buff_len * 3) / 4) - 16;
}

int cb_packager::unpack(char *pkg, char *a, char *out, int out_len) {
  if (pkg == NULL || a == NULL || out == NULL || out_len <= 0) return -1;
  int pkg_len = strlen(pkg);
  int a_len = strlen(a);
  if (pkg_len <= 0 || a_len <= 0) return -1;
  char buff[pkg_len];
  memset(buff, 0, sizeof(buff));
  cb_json j;
  int stt = j.getProp(pkg, "a", buff, sizeof(buff) - 1);
  if (stt != 0) return -2; // not a proper package
  if (strcmp(buff, a) != 0) return -2; // not a package for a 
  memset(buff, 0, sizeof(buff));
  stt = j.getProp(pkg, "m", buff, sizeof(buff) - 1);
  if (stt != 0) return -2; // not a proper package
  stt = decrypt(buff, out, out_len);
  if (stt == 0) return 0;
  if (stt == -1) return -2;
  if (stt == -2) return -3;
  return -2;
}

