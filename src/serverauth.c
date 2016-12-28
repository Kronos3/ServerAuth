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
#include <server.h>
#include <signal.h>
#include <time.h>

struct request_handler * m_request_handler;

char* save_creds (char** arg) {
    FILE *creds = fopen("creds.cache", "w+");
    //fprintf(creds, auth_encrypt (arg[0]));
    fclose(creds);
    return "creds.cache";
}

char* handle_request(char* req)
{
    char* req_args[2];
    req_args[0] = strtok (req, "|");  // Function name
    req_args[1] = strtok (NULL, "|"); // argc;
    
    int argc = atoi (req_args[1]);
    char* argv[argc];
    
    int i;
    for (i = 0; i!=argc; i++)
    {
        argv[i] = strtok(NULL, "|");
    }
    
    return (*m_request_handler->handler[ar_index (m_request_handler->request, req_args[0])])(argv);
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

char* check_creds(char** arg) {
    
}

void init_request_handler (void) {
    main_f_struct = malloc (sizeof(struct file_structure));
    m_request_handler = malloc (sizeof(struct request_handler));
    m_request_handler->request[0] = "save_creds";
    m_request_handler->request[1] = "check_creds";
    m_request_handler->handler[0] = save_creds;
    m_request_handler->handler[1] = check_creds;
}

void exit_request_handler (void) {
    free(m_request_handler);
}

int main (int argc, char** argv)
{
    hash_init ("hash.conf", main_f_struct);
    dump_md5 ("test", main_f_struct);
    struct file_structure * file_s = malloc (sizeof(struct file_structure));
    get_md5 ("test", file_s);
    /* The Server */
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;
    
    signal(SIGINT, handle_exit);
    
    char PORT[6];
    strcpy(PORT,"8002");
    if (getcwd(ROOT, sizeof(ROOT)) != NULL)
        ;
    else
        perror("getcwd() error");
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
        }
      }
    
      while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
    return 0;
}
