/*
 * hashcheck.h
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

#ifndef __HASHCHECK_H__
#define __HASHCHECK_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <authencrypt.h>
#include <httpencrypt.h>
#include <unistd.h>
#include <encode.h>

struct file_structure {
    unsigned char* f_md5[128];
    unsigned char* f_paths[128];
};

extern struct file_structure * main_f_struct;

int ar_index (char** in_arr, char* to_index);
unsigned long get_size_by_fd(int fd);
void get_file_md5 (char* filepath, unsigned char md5array[MD5_DIGEST_LENGTH]);
void hash_init (char* hash_conf, struct file_structure * fs);
int check_hash (struct file_structure * fs);
void removechar( char str[], char t );
void dump_md5 (char*, struct file_structure*);
void get_md5 (char*, struct file_structure*);

#endif
