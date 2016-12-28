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

char* S_PORT;
char *ROOT;

char RSA_ENCRYPT[3072];

void handle_exit(int a) {
    close (listenfd);
    printf ("\nserver shutdown\n");
    exit(1);
}

void strrev(char *p)
{
  char *q = p;
  while(q && *q) ++q;
  for(--q; p < q; ++p, --q)
    *p = *p ^ *q,
    *q = *p ^ *q,
    *p = *p ^ *q;
}

void error(char *msg) {
    perror(msg);
    exit(0);
}

void send_request  (char *request, char* res)
{
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    
    hostname = "localhost";
    portno = 8000;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");

    /* get message line from the user */
    bzero(buf, BUFSIZE);
    strcpy(buf, request);

    /* send the message line to the server */
    n = write(sockfd, buf, strlen(buf));
    if (n < 0) 
      error("ERROR writing to socket");

    /* print the server's reply */
    bzero(buf, BUFSIZE);
    n = read(sockfd, buf, BUFSIZE);
    if (n < 0)
      error("ERROR reading from socket");
    strcpy (res, buf);
    close(sockfd);
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
  S_PORT = port;
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
    char req_t[8], req[128], version[32], body[2048];
    sscanf (mesg, "%s %s %s[^\n]", req_t, req, version);
    printf ("%s\n", mesg);
    strcpy(body, mesg);
    strrev(body);
    sscanf(body, "%s[^\n]", body);
    strrev(body);
    if ( strcmp(req_t, "GET")==0 )
    {
      if ( strncmp( version, "HTTP/1.0", 8)!=0 && strncmp( version, "HTTP/1.1", 8)!=0 )
      {
        write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
      }
      else
      {
        char r_path[128], domain[32];
        if ( strncmp (req, "http://", 7)==0)
        {
          sscanf (req, "%*[^/][^/]%[^/]%s", domain, r_path);
        }
        else
          strcpy(r_path, req);
        
        if (strcmp(r_path, "/") == 0)
            strcpy(r_path, "/index.html");
        strcpy(path, ROOT);
        strcpy(&path[strlen(ROOT)], "/html");
        strcpy(&path[strlen(path)], r_path);
        if ( (fd=open(path, O_RDONLY))!=-1 )  //FILE FOUND
        {
          send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
          while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
            write (clients[n], data_to_send, bytes_read);
        }
        else
        {
            write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
        }
      }
    }
    else if (strcmp(req_t, "POST")==0 && strcmp(req, "/")==0)
    {
        if (strcmp(body, "en-US,en;q=0.8")==0)
            write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
        else
        {
            char user[32], pass[32], save[2], encrypted[256];
            FILE * fp = fopen ("html/passwd.cache", "w+b");
            strcpy (encrypted, body);
            strcpy (body, http_decrypt(body));
            sscanf(body, "user=%[^\\]\\pass=%[^\\]\\save=%s", user, pass, save);
            char request[128], *response;
            response = malloc (sizeof (char)*128);
            sprintf(request, "r_login|%s|%s|%s", user, pass, save);
            send_request (request, response);
            char ret[10];
            sscanf(response, "%*[^\"]\"%[^\"]", ret);
            if (strcmp(ret, "false")==0)
                write(clients[n], "HTTP/1.0 401 Unauthorized", 25);
            else
                write(clients[n], "HTTP/1.0 200 OK", 25);
                fwrite (encrypted, strlen(encrypted), 1, fp);
            fclose (fp);
        }
    }
    else if ( strcmp(req_t, "PASS")==0 )
    {
        if ( strncmp( version, "HTTP/1.0", 8)!=0 && strncmp( version, "HTTP/1.1", 8)!=0 )
        {
            write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
        }
        else
        {
            char user[32], pass[32], save[2], enc[256];
            FILE * fp = fopen ("html/passwd.cache", "rb");
            fread (enc, sizeof(char), 172, fp);
            fclose (fp);
            strcpy (enc, http_decrypt(enc));
            sscanf(enc, "user=%[^\\]\\pass=%[^\\]\\save=%s", user, pass, save);
            char request[128], *response;
            response = malloc (sizeof (char)*128);
            sprintf(request, "r_login|%s|%s|%s", user, pass, save);
            send_request (request, response);
            char ret[10];
            sscanf(response, "%*[^\"]\"%[^\"]", ret);
            if (strcmp(ret, "false")==0)
                write(clients[n], "HTTP/1.0 401 Unauthorized", 25);
            else
                write(clients[n], "HTTP/1.0 200 OK", 25);
        }
    }
  }

  //Closing SOCKET
  shutdown (clients[n], SHUT_RDWR);     //All further send and recieve operations are DISABLED...
  close(clients[n]);
  clients[n]=-1;
}
