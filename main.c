/*
 * main.c
 * 
 * Copyright 2016 Milindu Sanoj Kumarage <agentmilindu@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
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

#include <sys/socket.h>       /*  socket definitions */
#include <sys/types.h>        /*  socket types */
#include <arpa/inet.h>        /*  Internet funtions */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



#include "includes/utils.h"
#include "includes/configs.h"
#include "includes/http.h"

int main(int argc, char **argv)
{
	

    /*  Create socket  */
    
    int listener;

    if ( ( listener = socket(AF_INET, SOCK_STREAM, 0 ) ) < 0 ){
		gracefully_shutdown("Couldn't create the socket connection");
	}
	
	/*  Set socket address structure  */
	
	struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port        = htons(SERVER_PORT);

	/*  Assign the address to socket  */ 

    if ( bind(listener, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 ){
		gracefully_shutdown("Couldn't bind the socket.");
	}
	/*  Make socket a listening socket  */

    if ( listen(listener, MAX_CONNECTIONS) < 0 ){
		gracefully_shutdown("Listening to the socket failed!");
	}
	
	/*  Infinite loop to accept requests  */
	
	int connection;

	for(;;){

		/*  Wait for connection  */

		if ( ( connection = accept(listener, NULL, NULL)) < 0 ){
			gracefully_shutdown("Error accepting the request!");
		}
			
		/*  Fork child process to service connection  */
		
		int pid;

		if ( (pid = fork()) == 0 ) {

	    /*  == Child process == */
	    
			/* Close listening socket */

			if ( close(listener) < 0 ){
				gracefully_shutdown("Error closing listening socket in child.");
			}
			
			/* Handle the request */
			
			handle_request(connection);
			
			/* Close connected socket and exit  */

			if ( close(connection) < 0 ){
				gracefully_shutdown("Error closing the connection!");
			}
			
			/* Exit the child process */
		
			exit(EXIT_SUCCESS);
			
			
		}
		
		/* == Parent process == */

		if ( close(connection) < 0 )
			gracefully_shutdown("Error closing connection socket in parent.");

		//waitpid(-1, NULL, WNOHANG);
	}
	
	fprintf(stdout, "Bye!!\n");
	return 0;
}

