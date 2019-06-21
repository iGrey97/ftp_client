#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>		// getaddrinfo()
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>



/* constants */
#define MAXSIZE 			512 	// max buffer size


/* Holds command code and argument */
struct command {
	char arg[255];
	char arg1[255];
	char code[5];
};


enum COMMAND
{
	CMD_INVALID,

	CMD_LCD,	 //lcd
	CMD_LLS,	 //!ls
	CMD_LDIR,    //!dir
	CMD_LPWD,    //!pwd
	CMD_LMKDIR,  //!mkdir
	CMD_HOST,

	CMD_OPEN,
	CMD_USER,
	CMD_CD,
	CMD_MKDIR,
	CMD_PWD,
	CMD_CLOSE,
	CMD_QUIT,
	CMD_NO_USE_DATA_SOCKET,

	CMD_LS,  
	CMD_DIR,
	CMD_GET,
	CMD_PUT,
	
	
	
	
};



#endif







