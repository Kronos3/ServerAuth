/*
 * authencrypt.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <authencrypt.h>

// private key
const char *b64_pKey = "-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDxrHo1+SMguB/UOytsPhjlDx4B\n\
Ix5611YfxyVqPS46szesaQVBwNiIaiMU2IceGMxvF8flovrZ+QbTY9Sd365aNMn7\n\
VpMkGfPa57Ji6COmA/gCBFy1HJ6mJs//xgJRpmDSxzm1LBwmO3w1CbZQh/LFDg4c\n\
+xREZwG+S0JNwDnUGwIDAQAB\n\
-----END PUBLIC KEY-----";

const char *b64priv_key = "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXAIBAAKBgQDxrHo1+SMguB/UOytsPhjlDx4BIx5611YfxyVqPS46szesaQVB\n\
wNiIaiMU2IceGMxvF8flovrZ+QbTY9Sd365aNMn7VpMkGfPa57Ji6COmA/gCBFy1\n\
HJ6mJs//xgJRpmDSxzm1LBwmO3w1CbZQh/LFDg4c+xREZwG+S0JNwDnUGwIDAQAB\n\
AoGAbozifqwPqJjxXmkOrP4e7zEiHY0OFU1JxVGYyOdhrVJsBj8oO3LW2lgK4i9G\n\
Z7saUTtEosQSEgG7YJmLGgjHPPLL9aceIB6GoQO+GyQtBflUlmC/8pxcKBGK/02V\n\
rolr8kBKMIKbEhuJK2GvLebJmlMhgVdWiCOUxdvOlJDxWrkCQQDz/RHCrK4e2fTZ\n\
LEQkQ1BsnWq7qgbFziuC+/A6/lm3nuzzhbG07UgvCm+lTCxG6U/2DXk2EbK0nsfc\n\
FXhyfba3AkEA/ZI8JvdVH+QC0F+vi/fQ8ETYBIJAYmMamdQJUkn7M8sqa9X1gWqw\n\
DPrjMhig/yBQ8Mqbg23g6kBiGfu0EG6JvQJAcnUNl92Z8S8eBaMWdEx/tfRC1bZ1\n\
ZuXI07GMiKA5RkJvte3J565IZojmxwxs2PcqY6r+abbztnemGkCYOF1xEQJANaoz\n\
TNPfkiZBajQ2BNtnvtHzHdErKydqQ9AaQO4jWezPh476rG8V30lhuAJ6YERInQ1k\n\
KEai6TDDEliw3SFnyQJBAN0REj+2h1j9qPVW0XvGwrDP3s0rYBYUdd47oG3i43vH\n\
zh+EfzHY5/m1Y0IfgVRPJlJSfS2TJMMp/g0k2dzRrBc=\n\
-----END RSA PRIVATE KEY-----\n";


RSA* loadPUBLICKeyFromString( const char* publicKeyStr )
{
  BIO* bio = BIO_new_mem_buf( publicKeyStr, strlen(publicKeyStr) ) ;
  
  // Load the RSA key from the BIO
  RSA* rsaPubKey = NULL;
  PEM_read_bio_RSA_PUBKEY( bio, &rsaPubKey, NULL, NULL ) ;
  if( !rsaPubKey )
    printf( "ERROR: Could not load PUBLIC KEY!  PEM_read_bio_RSA_PUBKEY FAILED: %s\n", ERR_error_string( ERR_get_error(), NULL ) ) ;
  
  BIO_free( bio ) ;
  return rsaPubKey ;
}

RSA* loadPRIVATEKeyFromString( const char* privateKeyStr )
{
  BIO *bio = BIO_new_mem_buf( (void*)privateKeyStr, -1 );
  //BIO_set_flags( bio, BIO_FLAGS_BASE64_NO_NL ) ; // NO NL
  RSA* rsaPrivKey = PEM_read_bio_RSAPrivateKey( bio, NULL, NULL, NULL ) ;
  
  if ( !rsaPrivKey )
    printf("ERROR: Could not load PRIVATE KEY!  PEM_read_bio_RSAPrivateKey FAILED: %s\n", ERR_error_string(ERR_get_error(), NULL));
  
  BIO_free( bio ) ;
  return rsaPrivKey ;
}

unsigned char* rsaEncrypt( RSA *pubKey, const unsigned char* str, int dataSize, int *resultLen )
{
  int rsaLen = RSA_size( pubKey ) ;
  unsigned char* ed = (unsigned char*)malloc( rsaLen ) ;
  
  // RSA_public_encrypt() returns the size of the encrypted data
  // (i.e., RSA_size(rsa)). RSA_private_decrypt() 
  // returns the size of the recovered plaintext.
  *resultLen = RSA_public_encrypt( dataSize, (const unsigned char*)str, ed, pubKey, PADDING ) ; 
  if( *resultLen == -1 )
    printf("ERROR: RSA_public_encrypt: %s\n", ERR_error_string(ERR_get_error(), NULL));

  return ed ;
}

unsigned char* rsaDecrypt( RSA *privKey, const unsigned char* encryptedData, int *resultLen )
{
  int rsaLen = RSA_size( privKey ) ; // That's how many bytes the decrypted data would be
  
  unsigned char *decryptedBin = (unsigned char*)malloc( rsaLen ) ;
  *resultLen = RSA_private_decrypt( RSA_size(privKey), encryptedData, decryptedBin, privKey, PADDING ) ;
  if( *resultLen == -1 )
    printf( "ERROR: RSA_private_decrypt: %s\n", ERR_error_string(ERR_get_error(), NULL) ) ;
    
  return decryptedBin ;
}

unsigned char* makeAlphaString( int dataSize )
{
  unsigned char* s = (unsigned char*) malloc( dataSize ) ;
  
  int i;
  for( i = 0 ; i < dataSize ; i++ )
    s[i] = 65 + i ;
  s[i-1]=0;//NULL TERMINATOR ;)
  
  return s ;
}

char* rsaEncryptThenBase64( RSA *pubKey, unsigned char* binaryData, int binaryDataLen, int *outLen )
{
  int encryptedDataLen ;
  
  // RSA encryption with public key
  unsigned char* encrypted = rsaEncrypt( pubKey, binaryData, binaryDataLen, &encryptedDataLen ) ;
  
  // To base 64
  int asciiBase64EncLen ;
  char* asciiBase64Enc = base64( encrypted, encryptedDataLen, &asciiBase64EncLen ) ;
  
  // Destroy the encrypted data (we are using the base64 version of it)
  free( encrypted ) ;
  
  // Return the base64 version of the encrypted data
  return asciiBase64Enc ;
}

unsigned char* rsaDecryptThisBase64( RSA *privKey, char* base64String, int *outLen )
{
  int encBinLen ;
  unsigned char* encBin = unbase64( base64String, (int)strlen( base64String ), &encBinLen ) ;
  
  // rsaDecrypt assumes length of encBin based on privKey
  unsigned char *decryptedBin = rsaDecrypt( privKey, encBin, outLen ) ;
  free( encBin ) ;
  
  return decryptedBin ;
}
  


char* auth_encrypt (char* asciiData)
{
  ERR_load_crypto_strings();  
  
  // LOAD PUBLIC KEY
  RSA *pubKey = loadPUBLICKeyFromString( b64_pKey ) ;
  int asciiB64ELen ;
  char *temp = rsaEncryptThenBase64(pubKey, (unsigned char*)asciiData, (size_t)sizeof(asciiData)/sizeof(char), &asciiB64ELen );
  RSA_free( pubKey ) ;
  return temp;
}

char* auth_decrypt (char* asciiEncrypted)
{
  RSA *privKey = loadPRIVATEKeyFromString( b64priv_key ) ;
  
  int rBinLen ;
  char* asciiDecrypted = (char*)rsaDecryptThisBase64(privKey, asciiEncrypted, &rBinLen ) ;
  
  RSA_free(privKey) ;
  ERR_free_strings();
  return asciiDecrypted;
}

int main (int argc, char** argv)
{
  printf(auth_encrypt(argv[1]));
  printf("\n");
  
  printf (auth_decrypt(argv[2]));
  printf("\n");
  return 0;
}
