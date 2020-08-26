// cb_packager.h
// Cryptobits Packager
// By Abdullah Daud, chelahmy@gmail.com
// 22 August 2020

#ifndef __cb_packager__
#define __cb_packager__

#include "AES.h"
#include "Base64.h"
#include "cb_json.h"

class cb_packager {
	private:
		byte _key[16];
		uint8_t getrnd();
		void gen_iv(byte  *iv);
	public:
		cb_packager();
		cb_packager(byte key[]);
		int cleanHex(char *hex, char *out, int out_len);
		int bytes2Hex(byte data[], int data_len, char *out, int out_len);
		int hex2Bytes(char *hex, byte data[], int len);
		void setKey(byte key[]);
		int setHexKey(char *hex);
		int encrypt(char *msg, char *out, int out_len);
		int decrypt(char *msg, char *out, int out_len);
		int calcPackSize(char *a, char *m);
		int pack(char *a, char *m, char *out, int out_len);
		int calcUnpackSize(char *pkg);
		int unpack(char *pkg, char *a, char *out, int out_len);
};

#endif
