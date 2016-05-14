#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include "rs_encode_file.h"
#include "gflib.h"

void delete_files()
{
	int a;
	char c[50];

	FILE *fp;
	fp=fopen("tmp.txt","r");
	
	while(a!=EOF)
	{

		a=fscanf(fp,"%s",c); 
	
		remove(c);
	}
	fclose(fp);
	remove("tmp.txt");

	
}


void recovery(int attempt)
{
	char pingcommand[100],ip[50],filename[50],stem[70];
	int c,d,flag,atmp,i=0,k;
	pingcommand[0]='\0';
	
	FILE *fp;
	fp=fopen("Master_file.txt","r");
	char *ip_addr[] = {"127.0.0.1","127.0.0.1"};;

	while(c!=EOF)
	{
		atmp=attempt,i=0;
		//fp=fopen("Master_file.txt","r");


		strcpy(pingcommand,"");

		fscanf(fp,"%s",filename);
		fscanf(fp,"%s",stem);
		c=fscanf(fp,"%s",ip);

		//printf("\n%s\t",ip);
	
	
		strcat(pingcommand,"ping -c 1 -w 2 ");
		strcat(pingcommand,ip);
		strcat(pingcommand," | grep icmp");
	
		while(atmp>0)
		{

			FILE *cmdoutput = popen ( pingcommand, "r" );

			char str[500];
			if(fgets(str,500,cmdoutput)!=NULL)
			{
				printf("%s\tReachable\n",ip);
				flag=1;
			}
			else
			{
				printf("%s\tNot reachable\n",ip);
				flag=0;
				i++;
			
			}
			pclose ( cmdoutput );
	
			attempt--;
		
			sleep(20);
		}
		

		if(flag==0 && i==attempt)

		{
			printf("\n**successful**\n");
			
						
			decode(filename,filename);
			rename("tmp.txt",filename);
			
			g_encode(filename,4,2,filename);

			
			/*Socket Programming */
			int clientsocket,enable=1,bytes_read,bytes_received,port=7860;
			char buffer[1024]="",ack[6]="";

			
			struct sockaddr_in serverAddr;
			socklen_t addr_size;
			k=0;
			clientsocket=socket(PF_INET,SOCK_STREAM,0);
			printf("\n\nSocket creation: %s\n",strerror(errno));
			/* Construct local address structure */
			serverAddr.sin_family=AF_INET; /* Internet address family */
			serverAddr.sin_port = htons(port);/*server port*/
			serverAddr.sin_addr.s_addr=inet_addr(ip_addr[k]);/*server address*/
			memset(serverAddr.sin_zero,'\0',sizeof serverAddr.sin_zero);/* Zero out structure */ 

			port=port+1;
			k=k+1;
	
			printf("Before connect\n");
			/*reuse the same port*/
			if (setsockopt(clientsocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		    		error("setsockopt(SO_REUSEADDR) failed");

			/*connect to the nodes to receive the encoded files*/
			if ((connect(clientsocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))==-1)
			continue;
	   		 //printf("Connected to the node with ip address : %s\n",ip_addr);

	
			printf("After connect\n");
			
			FILE *ptr;
			ptr=fopen(stem,"r");
			
			bytes_read= fread(buffer, sizeof(char),sizeof(buffer),ptr);
			buffer[bytes_read]='\0';


			send(clientsocket,stem,strlen(stem),0);

			bytes_received=recv(clientsocket,ack,5,0);
			ack[bytes_received]='\0';
			
			send(clientsocket,buffer,strlen(buffer),0);

			close(clientsocket);
		}
		
		int pos;
		pos=ftell(fp);
		pos=pos-strlen(ip);
		fseek( fp, pos, SEEK_SET );

		fprintf(fp,"%s",ip_addr[k]);
		delete_files();
	}
			
		
 //return 0;
}
