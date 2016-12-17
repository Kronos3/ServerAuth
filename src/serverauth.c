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
#include <time.h>

char* authurl;

struct request_handler * m_request_handler;
char* save_creds (char* arg) {
    FILE *creds = fopen("creds.cache", "w+");
    fprintf(creds, auth_encrypt (arg));
    fclose(creds);
    return "creds.cache";
}

void gen_random(char *s, const int len)
{
    srand(time(NULL));
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

char* auth_url (char* arg) {
    gen_random(authurl, 12);
    return authurl;
}

void init_request_handler (void) {
    m_request_handler = malloc (sizeof(struct request_handler));
    m_request_handler->request[0] = "save_creds";
    m_request_handler->request[0] = "auth_url";
    m_request_handler->handler[0] = save_creds;
    m_request_handler->handler[1] = auth_url;
}

void exit_request_handler (void) {
    free(m_request_handler);
}

int main (int argc, char** argv)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;
    
    char PORT[6];
    strcpy(PORT,"8001");
    int slot=0;
    
    int i;
    for (i=0; i<CONNMAX; i++)
      clients[i]=-1;
    startServer(PORT);
    
    // ACCEPT connections
    while (1)
    {
      addrlen = sizeof(clientaddr);
      clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
    
      if (clients[slot]<0)
        error ("accept() error");
      else
      {
        if ( fork()==0 )
        {
          respond(slot);
          exit(0);
        }
      }
    
      while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
    return 0;
}
