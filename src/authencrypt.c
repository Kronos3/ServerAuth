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
const char *publicKey = "-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDxrHo1+SMguB/UOytsPhjlDx4B\n\
Ix5611YfxyVqPS46szesaQVBwNiIaiMU2IceGMxvF8flovrZ+QbTY9Sd365aNMn7\n\
VpMkGfPa57Ji6COmA/gCBFy1HJ6mJs//xgJRpmDSxzm1LBwmO3w1CbZQh/LFDg4c\n\
+xREZwG+S0JNwDnUGwIDAQAB\n\
-----END PUBLIC KEY-----";

const char *privKey = "-----BEGIN RSA PRIVATE KEY-----\n\
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


int padding = RSA_PKCS1_PADDING;
 
RSA * createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }
 
    return rsa;
}
 
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n",msg, err);
    free(err);
}

void auth_encrypt (unsigned char* asciiData, unsigned char* dest)
{
    int encrypted_length= public_encrypt(asciiData, strlen(asciiData), (unsigned char*)publicKey, dest);
    if(encrypted_length == -1)
    {
        printLastError("Public Encrypt failed ");
        exit (0);
    }
}

unsigned char* auth_decrypt (unsigned char* asciiData)
{
    char* decrypted;
    int decrypted_length = private_decrypt(asciiData, strlen(asciiData), (unsigned char*)privKey, decrypted);
    if(decrypted_length == -1)
    {
        printLastError("Private Decrypt failed ");
        return NULL;
    }
    return decrypted;
}
