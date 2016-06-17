/*
 * request.c
 * 
 * Copyright 2016 Milindu Sanoj Kumarage <agentmilindu@Agentstation>
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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#include "includes/request.h"
/*
 This function recieves the buffer
 until an "End of line(EOL)" byte is recieved
 */
int read_request(int fd, char *buffer) {
 char *p = buffer; // Use of a pointer to the buffer rather than dealing with the buffer directly
 int eol_matched = 0; // Use to check whether the recieved byte is matched with the buffer byte or not
 while (recv(fd, p, 1, 0) != 0) // Start receiving 1 byte at a time
 {
  if (*p == EOL[eol_matched]) // if the byte matches with the first eol byte that is '\r'
    {
   ++eol_matched;
   if (eol_matched == EOL_SIZE) // if both the bytes matches with the EOL
   {
    *(p + 1 - EOL_SIZE) = '\0'; // End the string
    return (strlen(buffer)); // Return the bytes recieved
   }
  } else {
   eol_matched = 0;
  }
  p++; // Increment the pointer to receive next byte
 }
 return (0);
}

/*
 A helper function
 */
void write_response(int fd, char *msg) {
 int len = strlen(msg);
 if (send(fd, msg, len, 0) == -1) {
  printf("Error in send\n");
 }
}

/*
 A helper function: Returns the
 web root location.
 */
char* webroot() {
 // open the file "conf" for reading
 FILE *in = fopen("conf", "rt");
 // read the first line from the file
 char buff[1000];
 fgets(buff, 1000, in);
 // close the stream
 fclose(in);
 char* nl_ptr = strrchr(buff, '\n');
 if (nl_ptr != NULL)
  *nl_ptr = '\0';
 return strdup(buff);
}

/*
 Handles php requests
 */
void php_cgi(char* script_path, int fd) {
 write_response(fd, "HTTP/1.1 200 OK\n Server: Web Server in C\n Connection: close\n");
 dup2(fd, STDOUT_FILENO);
 char script[500];
 strcpy(script, "SCRIPT_FILENAME=");
 strcat(script, script_path);
 putenv("GATEWAY_INTERFACE=CGI/1.1");
 putenv(script);
 putenv("QUERY_STRING=");
 putenv("REQUEST_METHOD=GET");
 putenv("REDIRECT_STATUS=true");
 putenv("SERVER_PROTOCOL=HTTP/1.1");
 putenv("REMOTE_HOST=127.0.0.1");
 execl("/usr/bin/php", "php-cgi", NULL);
}

