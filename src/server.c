/*
 * server.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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
#include <server.h>
#include <serverauth.h>

void handle_exit(int a) {
    close (listenfd);
    printf ("\nserver shutdown\n");
    exit(1);
}

//start server
void startServer(char *port)
{
  struct addrinfo hints, *res, *p;

  // getaddrinfo for host
  memset (&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo( NULL, port, &hints, &res) != 0)
  {
    perror ("getaddrinfo() error");
    exit(1);
  }
  printf("Started server on port %s\n", port);
  for (p = res; p!=NULL; p=p->ai_next)
  {
    listenfd = socket (p->ai_family, p->ai_socktype, 0);
    if (listenfd == -1) continue;
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
  }
  if (p==NULL)
  {
    perror ("socket() or bind()");
    exit(1);
  }

  freeaddrinfo(res);

  // listen for incoming connections
  if ( listen (listenfd, 1000000) != 0 )
  {
    perror("listen() error");
    exit(1);
  }
}

//client connection
void respond(int n)
{
  char mesg[99999], data_to_send[BYTES], path[99999];
  int rcvd, fd, bytes_read;

  memset( (void*)mesg, (int)'\0', 99999 );

  rcvd=recv(clients[n], mesg, 99999, 0);

  if (rcvd<0)  // receive error
    fprintf(stderr,("recv() error\n"));
  else if (rcvd==0)  // receive socket closed
    fprintf(stderr,"Client disconnected upexpectedly.\n");
  else  // message received
  {
    char req_t[8], req[128], version[32];
    printf("%s", mesg);
    sscanf (mesg, "%s %s %s[^\n]", req_t, req, version);
    printf ("req_t=%s\nreq=%s\nversion=%s\n", req_t, req, version);
    if ( strcmp(req_t, "GET")==0 )
    {
      if ( strncmp( version, "HTTP/1.0", 8)!=0 && strncmp( version, "HTTP/1.1", 8)!=0 )
      {
        write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
        printf("400\n");
      }
      else
      {
        char r_path[128], domain[32];
        if ( strncmp (req, "http://", 7)==0)
        {
            sscanf (req, "%*[^/][^/]%s[^/]%s", domain, r_path);
        }
        else
          strcpy(r_path, req);
        
        if (strcmp(r_path, "/") == 0)
            strcpy(r_path, "/index.html");
        
        strcpy(path, ROOT);
        strcpy(&path[strlen(ROOT)], "/html");
        strcpy(&path[strlen(path)], r_path);
        printf("file: %s\n", path);
        if ( (fd=open(path, O_RDONLY))!=-1 )  //FILE FOUND
        {
          send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
          while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
            write (clients[n], data_to_send, bytes_read);
        }
        else
        {
            write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
            printf("404\n");
        }
      }
    }
    else if ( strcmp(req_t, "REQ")==0 )
    {
      if ( strncmp( version, "HTTP/1.0", 8)!=0 && strncmp( version, "HTTP/1.1", 8)!=0 )
      {
        write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
        printf("400\n");
      }
      else
      {
        char* ret = handle_request (req);
        send(clients[n], ret, 17, 0);
      }
    }
  }

  //Closing SOCKET
  shutdown (clients[n], SHUT_RDWR);     //All further send and recieve operations are DISABLED...
  close(clients[n]);
  clients[n]=-1;
}
