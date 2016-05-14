#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

int main(){
	int welcomeSocket;/* Socket descriptor for server */
	int newSocket;/* Socket descriptor for client */
	int enable=1,bytes_received,flag=1,size=0;
 	char buffer[1024]="",stem_filename[40]="",option[30];
 	struct sockaddr_in serverAddr;/* Local address */
	struct sockaddr_storage serverStorage; /* client address */
	socklen_t addr_size; /* Length of client address data structure */

	/* Create socket for incoming connections */
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  	//printf("%d\n",welcomeSocket);
  	/* Construct local address structure */
	serverAddr.sin_family = AF_INET;  /* Internet address family */
	serverAddr.sin_port = htons(7857);  /* Local port */
	serverAddr.sin_addr.s_addr = INADDR_ANY;  /* incoming interface */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); /* Zero out structure */ 

	/*resuse the port. */
	if (setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    		error("setsockopt(SO_REUSEADDR) failed");
	
	/* Bind to the local address */
	bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


	while(1)
	{
	 	strcpy(buffer,"");
		/* Mark the socket so it will listen for incoming connections */
	  	if(listen(welcomeSocket,5)==0)
	    		printf("\n******Listening******\n");
	  	else
	    		printf("Error\n");

		/* Wait for a client to connect */
	 	addr_size = sizeof serverStorage;	
	  	newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
		/* newSocket is connected to a client! */
		printf("%d\n",newSocket);

		/*choice of encoding or decoding is received*/
		bytes_received=recv(newSocket,option,sizeof(option),0);
		option[bytes_received]='\0';
		printf("\n%s....\n",option);
		send(newSocket,"ack",sizeof("ack"),0);


		if(strcmp(option,"Encode")==0)
		{
			bytes_received=recv(newSocket,stem_filename,sizeof(stem_filename),0);
			stem_filename[bytes_received]='\0';
	
			send(newSocket,"recv",sizeof("recv"),0);
		
			/*receiving the file content*/
			FILE *f;
			f=fopen(stem_filename,"w");

			recv(newSocket,&size,sizeof(size),0);
			//buffer[bytes_received]='\0';
			size=ntohl(size);

			send(newSocket,"recv",sizeof("recv"),0);			

			bytes_received=recv(newSocket,buffer,size,0);
			buffer[bytes_received]='\0';
			
			printf("\nbytes_received :- %d",bytes_received);

			printf("File : %s has been received\n ",stem_filename);
			printf("\nFile content :\n %s",buffer);
		
			fprintf(f,"%s",buffer);
			fclose(f);


		}
		else
		{
			bytes_received=recv(newSocket,stem_filename,sizeof(stem_filename),0);
			stem_filename[bytes_received]='\0';
			printf("filename : %s \n",stem_filename);
	
			/* checking whether the stem_filename(encoded file) exists or not */	
			if( access(stem_filename, F_OK ) != -1 ) 
				printf("%s File exists\n",stem_filename);
			else
			{
				printf("%s File doesn't exist \n",stem_filename);
				flag=0;
			}

			/*if file exists ,send it to client requesting for it */
			if(flag==1)
			{
				FILE *fp;
				fp=fopen(stem_filename,"r");
	
			 	bytes_received = fread(buffer, sizeof(char),sizeof(buffer), fp);//read the content of file
				buffer[bytes_received]='\0';
				printf("\nFile content :\n %s",buffer);
			 	send(newSocket,buffer,strlen(buffer),0); //file sent
			}
			else{
				send(newSocket,"File does not exist",20,0); //file sent
			}
		}
	}
 return 0;
}

