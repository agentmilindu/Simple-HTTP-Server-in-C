/*
 * request.h
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
 
#define EOL "\r\n"
#define EOL_SIZE 2

 int read_request(int fd, char *buffer);
 void write_response(int fd, char *msg);
 char* webroot();
 void php_cgi(char* script_path, int fd);



