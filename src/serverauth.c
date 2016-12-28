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

#include <stdio.h>
#include <authencrypt.h>
#include <hashcheck.h>
#include <server.h>
#include <signal.h>
#include <time.h>

int main (int argc, char** argv)
{
    main_f_struct = malloc (sizeof (struct file_structure));
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
    ROOT = getenv("PWD");
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
