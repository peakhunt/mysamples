#ifndef __HEX_UTIL_DEF_H__
#define __HEX_UTIL_DEF_H__
extern void to_hex_array(char* ascii, int len, unsigned char* hex);
extern void to_ascii_array(unsigned char* hex, int len, char* ascii);
extern void print_hex_array(unsigned char* hex, int len);
extern void print_ascii_hex_array(char* hex, int len);
#endif //!__HEX_UTIL_DEF_H__

