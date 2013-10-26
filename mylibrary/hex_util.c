#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
to_hex_array(char* ascii, int len, unsigned char* hex)
{
   int i;
   char one_byte[3];

   for(i = 0; i < len; i += 2)
   {
      one_byte[0] = ascii[i];
      one_byte[1] = ascii[i+1];
      one_byte[2] = '\0';
      hex[i/2] = (unsigned char)strtol(one_byte, NULL, 16);
   }
}

void
to_ascii_array(unsigned char* hex, int len, char* ascii)
{
   int i;
   char one_byte[3];

   for(i = 0; i < len; i++)
   {
      sprintf(one_byte, "%02X", hex[i]);
      ascii[i * 2 + 0] = one_byte[0];
      ascii[i * 2 + 1] = one_byte[1];
   }
}

void
print_hex_array(unsigned char* hex, int len)
{
   int i;

   for(i = 0; i < len; i++)
   {
      printf("%02X ", hex[i]);
   }
   printf("\n");
}

void
print_ascii_hex_array(char* hex, int len)
{
   int i;

   for(i = 0; i < len/2; i++)
   {
      printf("%c%c ", hex[i*2], hex[i*2+1]);
   }
   printf("\n");
}

#if 0
int
main(int argc, char** argv)
{
   unsigned char hex[128];
   char  *ascii =
      "00010203040506070809900102030405060708090A0B0C0D0Eaabbccee",
         ascii2[128];
   int len,
       i;

   len = strlen(ascii);

   to_hex_array(ascii, len, hex);
   print_hex_array(hex, len / 2);

   to_ascii_array(hex, len / 2, ascii2);
   print_ascii_hex_array(ascii2, len);
   ascii2[len] = '\0';

   if(strcmp(ascii, ascii2) == 0)
   {
      printf("Good. Match!\n");
   }
   else
   {
      printf("Bug. No Match!\n");
   }
   return 0;
}
#endif
