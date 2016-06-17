/*
 * http.c
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>
 #include <sys/sendfile.h>

#include "includes/utils.h"
#include "includes/http.h"
#include "includes/request.h"


typedef struct {
 char *ext;
 char *mediatype;
} extn;

//Possible media types
extn extensions[] ={
 {"gif", "image/gif" },
 {"txt", "text/plain" },
 {"jpg", "image/jpg" },
 {"jpeg","image/jpeg"},
 {"png", "image/png" },
 {"ico", "image/ico" },
 {"zip", "image/zip" },
 {"gz",  "image/gz"  },
 {"tar", "image/tar" },
 {"htm", "text/html" },
 {"html","text/html" },
 {"css","text/css" },
 {"js","text/javascript" },
 {"php", "text/html" },
 {"pdf","application/pdf"},
 {"zip","application/octet-stream"},
 {"rar","application/octet-stream"},
 {0,0} };



void handle_request(int connection) {
 char request[500], resource[500], *ptr;
 int fd1, length;
 if (read_request(connection, request) == 0) {
  printf("Recieve Failed\n");
 }
 printf("%s\n", request);
 
 ptr = strstr(request, " HTTP/");
 if (ptr == NULL) {
  printf("NOT HTTP !\n");
 } else {
  *ptr = 0;
  ptr = NULL;

  if (strncmp(request, "GET ", 4) == 0) {
   ptr = request + 4;
  }
  if (ptr == NULL) {
   printf("Unknown Request ! \n");
  } else {
   if (ptr[strlen(ptr) - 1] == '/') {
    strcat(ptr, "index.html");
   }
   strcpy(resource, webroot());
   strcat(resource, ptr);
   char* s = strchr(ptr, '.');
   int i;
   for (i = 0; extensions[i].ext != NULL; i++) {
    if (strcmp(s + 1, extensions[i].ext) == 0) {
	 
     fd1 = open(resource, O_RDONLY, 0);
     printf("Opening \"%s\"\n", resource);
     
     if (fd1 == -1) {
		 
      printf("404 File not found Error\n");
      write_response(connection, "HTTP/1.1 404 Not Found\r\n");
      write_response(connection, "Content-length: 131\n");
	  write_response(connection, "Content-Type: text/html\n\n");
      write_response(connection, "<html><head><title>404 Not Found</title></head>");
      write_response(connection, "<body><p>404 Not Found: The requested resource could not be found!</p></body></html>");
      
     } else if (strcmp(extensions[i].ext, "php") == 0) {
		 
      php_cgi(resource, connection);
      sleep(1);
      
     } else {
		 
      printf("200 OK, Content-Type: %s\n\n", extensions[i].mediatype);
      write_response(connection, "HTTP/1.1 200 OK\n");
      
      
      if (ptr == request + 4){
		  
       struct stat stat_struct;
	   if (fstat(fd1, &stat_struct) == -1)
		printf("Error in getting size!\n");
	   printf("File : %s\n", ptr);
		
	   struct stat meta_buf;
	   fstat(fd1, &meta_buf);
	   off_t offset = 0;
	   off_t sent_bytes = 0;
	   off_t remain_data = meta_buf.st_size;
	   int content_length = (int)meta_buf.st_size;
	   
	   char content_length_string[50];
	   sprintf(content_length_string, "Content-length: %d\n", content_length);
	   
	   char content_type_string[50];
	   sprintf(content_type_string, "Content-Type: %s\n", extensions[i].mediatype);
	   
	   write_response(connection, "HTTP/1.1 200 OK\n");
	   write_response(connection, content_length_string);
	   write_response(connection, content_type_string);
	   write_response(connection, "Server : Web Server in C\n\n");
	   
        /* Sending file data */
        
        while (((sent_bytes = sendfile (connection, fd1, &offset, meta_buf.st_size)) > 0) && (remain_data > 0))
        {
				fprintf(stdout, "Sent %d bytes : Remaining data = %d\n", sent_bytes, remain_data);
                remain_data -= sent_bytes;
                fprintf(stdout, "Sent %d bytes : Remaining data = %d\n", sent_bytes, remain_data);
        }
        
       printf("Sent the file!\n");
       write_response(connection, "");
      }
      
     }
     break;
    }
    int size = sizeof(extensions) / sizeof(extensions[0]);
    if (i == size - 2) {
     printf("415 Unsupported Media Type\n");
     write_response(connection, "HTTP/1.1 415 Unsupported Media Type\n");
     write_response(connection, "Content-length: 200\n");
     write_response(connection, "Content-Type: text/html\n");
     write_response(connection, "Server : Web Server in C\n\n");
     write_response(connection, "<html><head><title>415 Unsupported Media Type</head></title>");
     write_response(connection, "<body><p>415 Unsupported Media Type!</p></body></html>");
    }
   }
  }
 }
}
