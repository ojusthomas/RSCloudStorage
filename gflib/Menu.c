#include <stdio.h>
#include "gflib.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "rs_encode_file.h"

int main()
{
	int ch,n,m,pid;
	char filename[50],stem[40],exe[80],a,b,file[20],ip_addr[30];
	int available_node,alive_node,dead_node,used_node,atmpt=3;

	/*pid = fork();
		
      	if (pid < 0) 
	{
         	perror("ERROR on fork");
         	exit(1);
      	}
      
	if (pid == 0) 
	{*/
         
      	
    do
	{
	printf("\n\tOPTIONS :----- ");
	printf("\n\t 1. Encode the File ");
	printf("\n\t 2. Decode the File ");
	printf("\n\t 3. Recovery ");
	printf("\n\t 4. Exit");
	printf("\nEnter your choice 1 or 2 or 3 :- ");
	scanf("%d",&ch);
	strcpy(exe,"");

	switch(ch)
	{
		case 1 :
			fflush(stdin);
			printf("\nEnter the filename to be encoded :- ");
			scanf("%s",filename);
			
			if( access(filename, F_OK ) == -1 ) 
			{			
				printf("File doesn't exist \n");
				break;
			}		

			printf("\nEnter the values of n(data node) and m(parity node) :- ");
			scanf("%d %d",&n,&m);
			
			strcpy(stem,filename);
			//printf("\nEnter the server Ip address :- ");
			//scanf("%s",ip_addr);
			
			encode(&filename,n,m,&stem);
			break;

		case 2 :			
  			printf("\nEnter the file name  :- "); 
  			scanf("%s",file);
			strcpy(stem,file);
			a=decode(&stem,&file);			
                        break;
		//case 3: recovery(3);
		case 3 : break;
		default : 
			printf("\n....Wrong choice entered ...");
	}
 }while(ch!=3);

//}

	recovery(atmpt);
return 0;
}