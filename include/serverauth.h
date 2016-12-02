/*
 * serverauth.h
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


#include <stdio.h>
#include <authencrypt.h>
#include <hashcheck.h>

struct request_handler {
    char *  request [2];
    char *  (*handler[2]) (char* arg);
}

extern struct request_handler m_request_handler;

void init_request_handler (void) __attribute__ ((constructor));
void exit_request_handler (void) __attribute__ ((destructor));
char* handle_request(char* req);
void file_write (struct file_structure * fs);
void run_server (int port);
