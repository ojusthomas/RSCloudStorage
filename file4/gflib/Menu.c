#include <stdio.h>
#include "gflib.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
	int ch;
	char filename[50],stem[20],exe[80],a,b,n[10],m[10],file[20],ip_addr[30];
	
	
    do
	{
	printf("\n\tOPTIONS :----- ");
	printf("\n\t 1. Encode the File ");
	printf("\n\t 2. Decode the File ");
	printf("\n\t 3. Exit");
	printf("\nEnter your choice 1 or 2 or 3 :- ");
	scanf("%d",&ch);
	strcpy(exe,"");

	switch(ch)
	{
		case 1 :
			fflush(stdin);
			printf("\nEnter the filename to be encoded :- ");
			scanf("%s",filename);
			printf("\nEnter the values of n(data node) and m(parity node) :- ");
			scanf("%s %s",n,m);
			//a=itoa(n);
			//b=itoa(m);
			//printf("\nEnter the stem :- ");
			//scanf("%s",stem);
			printf("\nEnter the server Ip address :- ");
			scanf("%s",ip_addr);
			
			strcat(exe,"./rs_encode_file");
			strcat(exe," ");
			strcat(exe,filename);
			strcat(exe," ");
			strcat(exe,n);
			strcat(exe," ");
			strcat(exe,m);
			//strcat(exe," ");
			//strcat(exe,stem);
			strcat(exe," ");
			strcat(exe,ip_addr);
			printf("\n %s",exe);
			
			system(exe);
			break;
		case 2 :			
  			printf("\nEnter the file name  :- "); 
  			scanf("%s",file);
			strcpy(stem,file);
			strcat(exe,"./rs_decode_file");
			strcat(exe," ");
			strcat(exe,stem);
			strcat(exe," ");
			strcat(exe,file);
			system(exe);
                        break;
		case 3 : break;
		default : 
			printf("\n....Wrong choice entered ...");
	}
 }while(ch!=3);
return 0;
}
