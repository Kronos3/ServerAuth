/*
 * httpencrypt.h
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

#ifndef __HTTPENCRYPT_H__
#define __HTTPENCRYPT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>

#define PADDING RSA_PKCS1_PADDING

const char *b64_pKey;
const char *b64priv_key;

RSA* loadPUBLICKeyFromString( const char* publicKeyStr );
RSA* loadPRIVATEKeyFromString( const char* privateKeyStr );
unsigned char* rsaEncrypt( RSA *pubKey, const unsigned char* str, int dataSize, int *resultLen );
unsigned char* rsaDecrypt( RSA *privKey, const unsigned char* encryptedData, int *resultLen );
unsigned char* makeAlphaString( int dataSize );
char* rsaEncryptThenBase64( RSA *pubKey, unsigned char* binaryData, int binaryDataLen, int *outLen );
unsigned char* rsaDecryptThisBase64( RSA *privKey, char* base64String, int *outLen );
char* http_encrypt (char* asciiData);
char* http_decrypt (char* asciiEncrypted);

#endif
