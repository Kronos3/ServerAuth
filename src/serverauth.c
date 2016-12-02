/*
 * serverauth.c
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

#include <serverauth.h>

char* save_creds (char* arg) {
    FILE *creds = fopen("creds.cache", "w+");
    fprintf(creds, auth_encrypt (arg));
    fclose(fp);
    return "creds.cache"
}

char* auth_url (char* arg) {
    return "localhost:8001/"
}

void init_request_handler (void) {
    struct request_handler m_request_handler = malloc (sizeof(request_handler));
    m_request_handler->request = ["save_creds", "auth_url"];
    m_request_handler->handler[0] = save_creds;
    m_request_handler->handler[1] = auth_url;
}

void exit_request_handler (void) {
    free(m_request_handler);
}
