#include "common.h"

int socket_control; 
int socket_control_close = 0;
int ftp_client_close = 0;
int have_login = 0;

int socket_client_connect(char *ip, int port , int type)
{


	struct sockaddr_in server_addr;
	int socket_fd = socket(AF_INET, type, 0);

	if(socket_fd < 0){
		perror("socket err \n");
		return -1;
	}
	//printf("ip = %s ,port = %d \n",ip, port);
	bzero(&server_addr,sizeof(server_addr));	//init 0
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	if(connect(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		//perror("connect err \n");
		return -1;
	}

	return socket_fd;


}
/**
 * Receive a response from server
 * display 1 dis. 0 no dis
 * Returns  0 on success,
 *		   -1 on error,
 *		   -2 on time out,
 *         -3 on connect close
 */
int read_reply(int display){
	int ret = 0;
	int num_recvd = 0;
	char buf[MAXSIZE] = "";
	//if(num_recvd = read(socket_control, buf, MAXSIZE),num_recvd < 0){
	if(num_recvd = recv(socket_control, buf, MAXSIZE, 0),num_recvd < 0){
		//printf("EINTR = %d, EWOULDBLOCK = %d, EAGAIN = %d  num_recvd = %d \n", EINTR, EWOULDBLOCK, EAGAIN, num_recvd);
		if(errno  == EINTR || errno  == EWOULDBLOCK || errno  == EAGAIN){
			//printf("time out errno	= %d\n",errno );
			ret = -2;//time out
		}
	    else
			ret = -3;//connect close
			
	}
	else if(display){
		printf("%s",buf);
		fflush(stdout); 	

	}

	if(buf[0] == '5'){
		ret = -1;
	}
	return ret;
}



/** 
 * Read input from command line
 */
void input(char* buffer, int size)
{
	
	memset(buffer, 0, size);

	if(fgets(buffer, size, stdin) != NULL ) {//fgets \n end
		char *n = strchr(buffer, '\n');
		if (n) {
			*n = 0;
		}
		
	}
	/*if (scanf("%s",buffer) != EOF){//scanf \0 end

		strcat(buffer,"\r\n");//FTP cmd need '\r\n' ending
	
	}*/
	
}


/**
 * Parse command in cstruct
 */ 
int read_cmd(char* buf, int size, struct command *cmd)
{
	int cmd_id = CMD_INVALID;
	memset(cmd, 0, sizeof(struct command));
	
	printf("ftp_client> ");	// prompt for input		
	fflush(stdout); 	


	//scanf("%s",buf);//don't use for scanf will be finish by space key,and lost cmd->arg
	input(buf, size);
	//printf("cmd_buf = %s \n",buf);	
	char *arg = NULL;
	arg = strtok (buf," ");
	for(int i = 0; i < 2; i++){
		arg = strtok (NULL, " ");
		if (arg != NULL){
			// store the argument if there is one
			if(i == 0)
				strcpy(cmd->arg, arg);
			else
				strcpy(cmd->arg1, arg);	
			//printf("arg != NULL,arg:%s \n",arg);
	   }else
	   		break;
		

	}

	
	

	
	//printf("buf : %s\n",buf);

	// buf = command
	if (strcmp(buf, "open") == 0 ) {	
		cmd_id = CMD_OPEN;
	}else if (strcmp(buf, "user") == 0 ){	
		cmd_id = CMD_USER;
	}else if (strcmp(buf, "ls") == 0 || strcmp(buf, "dir") == 0) {
		
		strcpy(cmd->code, "LIST");		
		cmd_id = CMD_LS;
	}else if (strcmp(buf, "!ls") == 0) {	
		strcpy(cmd->code, "ls");	
		cmd_id = CMD_LLS;
	}else if (strcmp(buf, "!dir") == 0) {
		strcpy(cmd->code, "dir");	
		cmd_id = CMD_LDIR;
	}else if (strcmp(buf, "cd") == 0) {
		strcpy(cmd->code, "CWD");
		cmd_id = CMD_CD;
	}else if (strcmp(buf, "lcd") == 0) {
		strcpy(cmd->code, "cd");	
		cmd_id = CMD_LCD;
    }else if (strcmp(buf, "pwd") == 0) {
		strcpy(cmd->code, "PWD");
		cmd_id = CMD_PWD;
	}else if (strcmp(buf, "!pwd") == 0) {
		strcpy(cmd->code, "pwd");	
		cmd_id = CMD_LPWD;
	}else if(strcmp(buf, "mkdir") == 0){
		strcpy(cmd->code, "MKD");	
		cmd_id = CMD_MKDIR;

	}
	else if(strcmp(buf, "!mkdir") == 0){
		strcpy(cmd->code, "mkdir"); 
		cmd_id = CMD_LMKDIR;

	}else if (strcmp(buf, "close") == 0) {
		strcpy(cmd->code, "QUIT");
		cmd_id = CMD_CLOSE;
	}else if (strcmp(buf, "bye") == 0 || strcmp(buf, "quit") == 0 ) {
		strcpy(cmd->code, "QUIT");
		ftp_client_close = 1;
		cmd_id = CMD_QUIT;
	}
	else if (strcmp(buf, "get") == 0) {
		strcpy(cmd->code, "RETR");		
		cmd_id = CMD_GET;
	}else if (strcmp(buf, "put") == 0) {
		strcpy(cmd->code, "STOR");		
		cmd_id = CMD_PUT;
	}else {//invalid
		cmd_id = CMD_INVALID;
	}
	//printf("cmd->arg:%s \n",cmd->arg);
	//printf("cmd->code:%s \n",cmd->code);

	return cmd_id;
}



/**
 * Input: cmd struct with an a code and an arg
 * Concats code + arg into a string and sends to server
 */
int send_cmd(struct command *cmd)
{
	char buffer[MAXSIZE];
	int rc;
	if(strlen(cmd->arg) != 0){
		if(strcmp(cmd->code,"STOR") ==0)
		{
			if(strlen(cmd->arg1) != 0)
				sprintf(buffer, "%s %s\r\n", cmd->code, cmd->arg1);//FTP cmd need '\r\n' ending
			else
				sprintf(buffer, "%s %s\r\n", cmd->code, cmd->arg);//FTP cmd need '\r\n' ending
				
		}else
			sprintf(buffer, "%s %s\r\n", cmd->code, cmd->arg);//FTP cmd need '\r\n' ending
	}
	else{
		sprintf(buffer, "%s\r\n", cmd->code);//FTP cmd need '\r\n' ending
	}
	//printf("Grey:%s,%d  cmd = %s, strlen = %d \n",__FUNCTION__, __LINE__, buffer, strlen(buffer));
	// Send command string to server
	rc = send(socket_control, buffer, (int)strlen(buffer), 0);	
	if (rc < 0) {
		//perror("Error sending command to server");
		return -1;
	}
	
	return 0;

}

/**
 * Do get <filename> command 
 */
int do_cmd_get(int data_socket, char* filename)
{
    char data[MAXSIZE];
    int size;
    FILE* f = fopen(filename, "wb");

    
    while (1) {
		memset(data, 0, MAXSIZE);
		size = recv(data_socket, data, MAXSIZE, 0);
		if(size <= 0){// 0 -- connect close
			//printf("Grey:recv  file end.%d \n", size);
			break;
		}
        fwrite(data, 1, size, f);
    }

    if (size < 0) {
        perror("error\n");
    }

    fclose(f);
    return 0;
}


/**
 * Do put <filename> command 
 */
int do_cmd_put(int data_socket, char* filename)
{
    char data[MAXSIZE];
    int size;
    FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("local: %s: No such file or directory \n",filename);	
		return -1;
	}
	
    while(!feof(f))
	{
		memset(data, 0, MAXSIZE);
		size = fread(data, 1, MAXSIZE, f);
		//printf("read size : %d \n", size);
		size = send(data_socket, data, size,0);
		//printf("send size : %d \n", size);
	
	}
	//printf("send finish \n");


    fclose(f);
    return 0;
}




/** 
 * Do list commmand
 */
int do_cmd_list(int data_socket)
{
	size_t num_recvd;			// number of bytes received with recv()
	char buf[MAXSIZE];			// hold a filename received from server
	printf("%s, %d\n",__FUNCTION__, __LINE__);
	memset(buf, 0, sizeof(buf));
	while ((num_recvd = recv(data_socket, buf, MAXSIZE, 0)) > 0) {
        printf("%s", buf);
		memset(buf, 0, sizeof(buf));
	}
	
	if (num_recvd < 0) {
	    perror("error");
	}


	return 0;
}




/**
 * enter pasv mode get server'data port 
 * and connect with server'data port
 * return socket_data
 */

int enter_pasv_mode(char *ip)
{
	

	struct command pasv_cmd = {0}; 
	strcpy(pasv_cmd.code , "PASV");
	if (send_cmd(&pasv_cmd) != 0) {
		printf("%s,%d :send_cmd error \n", __FUNCTION__, __LINE__);
		close(socket_control);
		exit(1);
	}
	
	int num_recvd = 0;
	int addr[6];
	char buf[MAXSIZE] = "";
	if(num_recvd = recv(socket_control, buf, MAXSIZE, 0),num_recvd < 0){
		perror("client: error reading message from server\n");
		return -1;
	}	
	if(buf[0] == '5')
	{	
		//printf("%s,%d buf:%s \n",__FUNCTION__, __LINE__, buf);
		//fflush(stdout); 
		return -1;
	}
	//printf("%s,%d buf :%s\n",__FUNCTION__, __LINE__,buf);
	

	sscanf(buf,"%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
	/*for(int i = 0; i < 6; i++){
		printf("addr[%d] = %d \n",i, addr[i]);
	}*/
	
	int port_data = addr[4]*256 + addr[5];


	int socket_data = socket_client_connect(ip, port_data, SOCK_STREAM);
	return socket_data;

}



/**
 * Get login details from user and
 * send to server for authentication
 */
int login(char *name)
{
	struct command cmd;
	char user[256];
	memset(user, 0, 256);

	if(name == NULL){
		// Get username from user
		printf("Name: ");	
		fflush(stdout); 	
		
		
		input(user,256);

	}else{
		strcpy(user,name);
	}
	
	// Send USER command to server
	strcpy(cmd.code, "USER");
	strcpy(cmd.arg, user);
	if (send_cmd(&cmd) != 0) {
		printf("%s,%d :send_cmd error \n", __FUNCTION__, __LINE__);
		close(socket_control);
		exit(1);
	}
	//printf("Grey:client_send_cmd finish \n");
	 
	
	read_reply(1);

	// Get password from user
	char *pass = getpass("Password: ");	
    
	// Send PASS command to server
	strcpy(cmd.code, "PASS");
	strcpy(cmd.arg, pass);
	if (send_cmd(&cmd) != 0) {
		close(socket_control);
		exit(1);
	}
	
	int ret = read_reply(1);
	if(ret == -1)
		return -1;
	return 0;
	
}



int control_channel_open(char *ip, int port)
{
	socket_control = socket_client_connect(ip, port, SOCK_STREAM);
    if(socket_control == -1)
		return -1;
	
	//int flags = fcntl(socket_control, F_GETFL, 0);	 
	//fcntl(socket_control, F_SETFL, flags | O_NONBLOCK);	 ////non-blocking

	/*struct timeval
	 * {
	 *	   time_t tv_sec;
	 *	   time_t tv_usec;
	 * };
	 */
	struct timeval timeout={0,50000};//50ms
	 //int ret=setsockopt(sock_fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
	 int ret=setsockopt(socket_control,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
	 if(ret != 0)
	 {
		perror("setsockopt");
		return -1;
	 }


	printf("Connected to %s.\n", ip);
	read_reply(1); 
	return 0;

}



int main(int argc, char* argv[]) 
{		
	
	char buffer[MAXSIZE];
	struct command cmd;	
	int socket_data = 0;


	if (argc != 3) {
		printf("usage: ./ftp_client hostname port\n");
		exit(0);
	}

	char ip[20];
	memset(ip, 0, 20);
	strcpy(ip,argv[1]);
	//printf("ip:%s \n",ip);
	int port = atoi(argv[2]);

	int ret = control_channel_open(ip, port);
	if(ret == -1)
		exit(0);


	have_login = login(0) == 0? 1: 0;


	while (!ftp_client_close) { 

	

		int cmd_id = read_cmd(buffer, sizeof buffer, &cmd);
		
		if (cmd_id == CMD_INVALID){
			printf("Invalid command\n");
			continue;

		}else if(cmd_id < CMD_HOST){
			char host_cmd[64];
			sprintf(host_cmd, "%s %s",cmd.code,cmd.arg);
			system(host_cmd);
		}
		else{
			if(!socket_control_close){	
				if(read_reply(1) == -3){//for test connect situation
					socket_control_close = 1;
					close(socket_control);
				}
			}
			if(!socket_control_close){	
				if(cmd_id < CMD_NO_USE_DATA_SOCKET){
					if(cmd_id == CMD_OPEN){
						printf("Already connected to %s(%d), use close first.\n",ip, port);
						continue;
					}
					if(cmd_id == CMD_USER){
						if(strlen(cmd.arg) != 0)
							have_login = login(cmd.arg) == 0? 1: 0;
	
						else
							have_login = login(0) == 0? 1: 0;
						continue;
					}	
					if (send_cmd(&cmd) != 0) {
						printf("%s,%d :send_cmd error \n", __FUNCTION__, __LINE__);
						//close(socket_control);
						
					}
					
					do{
						ret = read_reply(1);//read_reply after data trans
					}while(ret == -2);
					if(ret != 0){
						//printf("%s,%d err!!!\n",__FUNCTION__,__LINE__);//ret code 5xx or close connect
						continue;
					}
					
					if(cmd_id == CMD_CLOSE || cmd_id == CMD_QUIT){
						socket_control_close = 1;
						close(socket_control);
	
					}

				}else{
					//printf("code: %s, arg: %s, arg1:%s \n",cmd.code, cmd.arg, cmd.arg1);
					socket_data = enter_pasv_mode(ip);
					if(socket_data == -1){
						//printf("no login!\n");//no login will come
						continue;
					}
					if (send_cmd(&cmd) != 0) {
						//printf("%s,%d :send_cmd error \n", __FUNCTION__, __LINE__);
						close(socket_control);
						exit(1);
					}
					
					do{
						ret = read_reply(1);//read_reply before data trans
					}while(ret == -2);
					if(ret != 0){
						//printf("%s,%d err!!!\n",__FUNCTION__,__LINE__);//ret code 5xx or close connect
						continue;
					}
			
					int display = 1;
					
					switch (cmd_id)
					{		
					case CMD_LS:		
					case CMD_DIR:	
						do_cmd_list(socket_data);
						break;
						
					case CMD_GET:
						
						if(strlen(cmd.arg1) != 0)
							do_cmd_get(socket_data, cmd.arg1);
							
						else
							do_cmd_get(socket_data, cmd.arg);
						
						break;
			
					case CMD_PUT:
						display = do_cmd_put(socket_data, cmd.arg) == 0? 1: 0;
						break;
						
					}
					close(socket_data);//!!! must first close fd,especially when put,is's a signal of trans finish
					do{
						ret = read_reply(1);//read_reply after data trans
					}while(ret == -2);
					if(ret != 0){
						//printf("%s,%d err!!!\n",__FUNCTION__,__LINE__);//ret code 5xx or close connect
						continue;
					}

				}
			}else{
				if(cmd_id == CMD_OPEN){
					if(strlen(cmd.arg) == 0 || strlen(cmd.arg1) == 0){
						printf("usage: open ip port\n");
					}else{
						memset(ip, 0, 20);
						strcpy(ip,cmd.arg);
						port = atoi(cmd.arg1);
				
						ret = control_channel_open(ip, port);
						if(ret == 0){
							socket_control_close = 0;				
							have_login = login(0) == 0? 1: 0;
						}

					}
					

				}else if(cmd_id != CMD_QUIT){
					printf("Not connected.\n");
				}

			}
	
		}
			

	}

		

    return 0;  
}
