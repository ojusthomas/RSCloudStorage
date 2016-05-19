/*
Procedures and Programs for Galois-Field Arithmetic and Reed-Solomon Coding.  
Copyright (C) 2003 James S. Plank

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

---------------------------------------------------------------------------
Please see http://www.cs.utk.edu/~plank/plank/gflib
for instruction on how to use this library.

Jim Plank
plank@cs.utk.edu
http://www.cs.utk.edu/~plank

Associate Professor
Department of Computer Science
University of Tennessee
203 Claxton Complex
1122 Volunteer Blvd.
Knoxville, TN 37996-3450

     865-974-4397
Fax: 865-974-4404

$Revision: 1.2 $
*/

#include <stdio.h>
#include "gflib.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>//Added by : Supriya
#include <netinet/in.h>//Added by : Supriya
#include <string.h>//Added by : Supriya
#include <errno.h>//Added by : Supriya
#include "rs_encode_file.h"
#include "fn_call_function.h" // added by ojus

/* This one is going to be in-core */
void createpath(char * filename)
{
	char cwd[1024];
	printf("\n Inside createpath %s ",filename);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
    	fprintf(stdout, "Current working dir: %s\n", cwd);
  	else
    	error("getcwd() error");
    // Writing the fragments to Data folder in the storage node, added by Ojus  # start
    mkdir("MetaData", S_IRUSR | S_IWUSR | S_IXUSR);
    strncat(cwd,"/MetaData/",sizeof("/MetaData/"));
    //sprintf(buf_file, "%s-%04d.rs", stem, i);
    strcat(cwd,filename);
    strncpy(filename,cwd,sizeof(cwd));
    printf("\n Exiting createpath : %s ",filename);
    //return filename;
}

void encode(char *filename,int n,int m,char *stem)
{
  int i, j, *vdm, *inv, *prod, cache_size;
  int rows, cols, blocksize, orig_size;
  int sz, *factors, tmp, factor;
  //char *stem; 
  char **buffer, buf_file[1024] ="", *block;
  struct stat buf;
  FILE *f;
  FILE *fpm,*fls;

  /*if (argc != 5) {
    fprintf(stderr, "usage: rs_encode_file filename n m server_ip_addr\n");
    exit(1);
  }
  
  n = atoi(argv[2]);
  m = atoi(argv[3]);
  stem=argv[1];
  filename = argv[1];
  ip_addr=argv[4];*/
  //strcpy(stem,filename);/*modified*/


  //Author : Supriya
  char files[40]="",file_name_tmp[40]="";
  strcat(files,"ls ");
  //strcat(files,stem);
  strcpy(file_name_tmp,filename);
  
  /*ends*/


  rows = n+m;
  cols = n;

  if (stat(filename, &buf) != 0) {
    perror(filename);
    exit(1);
  }

  sz = buf.st_size;
  orig_size = buf.st_size;
  if (sz % (n*sizeof(unit)) != 0) {
    sz += (n*sizeof(unit) - (sz % (n*sizeof(unit))));
  }
  blocksize = sz/n;

  buffer = (char **) malloc(sizeof(char *)*n);
  for (i = 0; i < n; i++) {
    buffer[i] = (char *) malloc(blocksize);
    if (buffer[i] == NULL) {
      perror("Allocating buffer to store the whole file");
      exit(1);
    }
  }


	

  f = fopen(filename, "r");
  if (f == NULL) { perror(filename); }
  cache_size = orig_size;

  for (i = 0; i < n; i++) {
    if (cache_size < blocksize) memset(buffer[i], 0, blocksize);
    if (cache_size > 0) {
      if (fread(buffer[i], 1, (cache_size > blocksize) ? blocksize : cache_size, f) <= 0) {
        fprintf(stderr, "Couldn't read the right bytes into the buffer\n");
        exit(1);
      }
    }
    cache_size -= blocksize;
  }
  fclose(f);

  //buf_file = (char *) malloc(sizeof(char)*(strlen(stem)+30));
  //if (buf_file == NULL) { perror("malloc - buf_file"); exit(1); }

  // @info Creating Data fragments
  char cwd[1024];
  block = (char *) malloc(sizeof(char)*blocksize);
  if (block == NULL) { perror("malloc - block"); exit(1); }
  char tmp_filename[]="tmp.txt";
  fls = fopen(tmp_filename,"w");
  for (i = 0; i < n; i++) {
  	/*if (getcwd(cwd, sizeof(cwd)) != NULL)
    	fprintf(stdout, "Current working dir: %s\n", cwd);
  	else
    	error("getcwd() error");
    // Writing the fragments to Data folder in the storage node, added by Ojus  # start
    mkdir("MetaData", S_IRUSR | S_IWUSR | S_IXUSR);
    strncat(cwd,"/MetaData/",sizeof("/MetaData/"));*/
    
    sprintf(buf_file, "%s-%04d.rs", stem, i);
    //strncat(cwd,buf_file,sizeof(buf_file));
    //strncpy(buf_file,cwd,sizeof(cwd));
    fprintf(fls,"%s\n",buf_file);
    createpath(buf_file);
    printf("Writing %s ...", buf_file); fflush(stdout);
   
    f = fopen(buf_file, "w");
    if (f == NULL) { perror(buf_file); exit(1); }
    fwrite(buffer[i], 1, blocksize, f);
    fclose(f);
    printf(" Done\n");
  }
  fclose(fls);
//@ info end of creating data fragments
	// Added by : supriya 
	
	/*collect the fragments name in tmp.txt*/  
    //createpath(files);
   // printf("\n 1 ~~~~~~~~ > %s....",files);
  	//strcat(files,"-* >tmp.txt");
  	//printf("\n 2 ~~~~~~~~ > %s....",files);
 	//system(files);
	
	
	/*ends*/

	// @ info Start of creating parity fragments	
  factors = (int *) malloc(sizeof(int)*n);
  if (factors == NULL) { perror("malloc - factors"); exit(1); }

  for (i = 0; i < n; i++) factors[i] = 1;
  
  vdm = gf_make_dispersal_matrix(rows, cols);
  fls = fopen(tmp_filename,"a");
  for (i = cols; i < rows; i++) {
    sprintf(buf_file, "%s-%04d.rs", stem, i);
    printf("Calculating  %s ...", buf_file); fflush(stdout);
    memset(block, 0, blocksize); 
    for (j = 0; j < cols; j++) {
      tmp = vdm[i*cols+j]; 
      if (tmp != 0) {
        factor = gf_single_divide(tmp, factors[j]);
/*        printf("M[%02d,%02d] = %3d.  Factors[%02d] = %3d.  Factor = %3d.\n",
                i, j, tmp, j, factors[j], factor); */
        factors[j] = tmp;
/*        printf("     Block %2d Bef: %3d.  ", j, buffer[j][0]); */
        gf_mult_region(buffer[j], blocksize, factor);
/*        printf("Block %2d Aft: %3d.  ", j, buffer[j][0]); */
/*         printf("Block %2d Bef: %3d.  ", i, block[0]); */
        gf_add_parity(buffer[j], block, blocksize);
       /*  printf("Block %2d Aft: %3d.\n", i, block[0]); */
      }
    }
    
    //if (getcwd(cwd, sizeof(cwd)) != NULL)
    	///fprintf(stdout, "Current working dir: %s\n", cwd);
  	//else
    	//error("getcwd() error");
    // Writing the fragments to Data folder in the storage node, added by Ojus  # start
    //mkdir("MetaData", S_IRUSR | S_IWUSR | S_IXUSR);
    //strncat(cwd,"/MetaData/",sizeof("/MetaData/"));
    sprintf(buf_file, "%s-%04d.rs", stem, i);
    //strcat(cwd,buf_file);
    fprintf(fls,"%s\n",buf_file);
    createpath(buf_file);
    strncpy(buf_file,cwd,sizeof(cwd));
    printf(" \n writing  ...%s ", buf_file); fflush(stdout);
    f = fopen(buf_file, "w");
    if (f == NULL) { perror(buf_file); exit(1); }
    printf("\n %%%%%%%%%%%%%%%%%%%%%");
    fwrite(block, 1, blocksize, f);
    printf(" Done\n");
    fclose(f);
  } // end of for
  
  //@ info end of creating parity fragments
  printf ("\n ******************************* ooooo ***********************************\n");

  sprintf(buf_file, "%s-info.txt", stem, i);
  //if (getcwd(cwd, sizeof(cwd)) != NULL)
    	//fprintf(stdout, "Current working dir: %s\n", cwd);
  	//else
    //	error("getcwd() error");
    // Writing the fragments to Data folder in the storage node, added by Ojus  # start
    //mkdir("MetaData", S_IRUSR | S_IWUSR | S_IXUSR);
   // strncat(cwd,"/MetaData/",sizeof("/MetaData/"));
   // sprintf(buf_file, "%s-%04d.rs", stem, i);
   // strncat(cwd,buf_file,sizeof(buf_file));
    fprintf(fls,"%s\n",buf_file);
    createpath(buf_file);
    strcpy(buf_file,cwd);
    
  f = fopen(buf_file, "w");
  if (f == NULL) { perror(buf_file); exit(1); }
  fprintf(f, "%d\n", orig_size);
  fprintf(f, "%d\n", sz);
  fprintf(f, "%d\n", blocksize);
  fprintf(f, "%d\n", n);
  fprintf(f, "%d\n", m);
  gf_write_matrix(f, vdm, rows, cols);
  fclose(f);
  fclose(fls);

	//Added by :supriya
	/*Socket program in order to send all the encoded files to different nodes*/

  	printf ("\n ******************************* ooooo ***********************************\n");
	int clientsocket;/* Socket descriptor for client */
	int enable=1,num=n+m,port=7856,bytes_read,a,pid,k=0;
	char sup[1024]="",msg[10]="",frag_name[100]="",temp_frag_name[100] = "",str[INET_ADDRSTRLEN],ack[10],file_name[100];
	struct sockaddr_in serverAddr;/* client address */
	socklen_t addr_size;
	char *ip_addr[] ={"192.168.42.190","192.168.42.193","192.168.42.83","192.168.42.191","192.168.42.189","192.168.42.186","192.168.42.70"}; /*stores the ip address of all the storage nodes*/
	// start - ojus
	 // file for passing filename and filefragments for meta data storage using python added by Ojus
	fpm = fopen( "metadata_exchangefile.txt" , "w" );
    fprintf(fpm,"%s",filename); 
    fprintf(fpm,"%s","\n");
    fclose(fpm);
    //  end -Ojus
    printf("num : %d ",num);
	while(num>0)
	{
		printf("\n ********************* Start Socket Creation ***************************");
		strcpy(file_name,file_name_tmp);
		fpm = fopen( "metadata_exchangefile.txt" , "a" ); // opening the meta data exchange file
		port = 7856;
		/* Create socket for connections */
		clientsocket=socket(PF_INET,SOCK_STREAM,0);
		if(clientsocket == -1)
			printf("\n\nSocket creation: %s\n",strerror(errno));
		while(1){
		/* Construct local address structure */
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr=inet_addr(ip_addr[num]);
		memset(serverAddr.sin_zero,'\0',sizeof (serverAddr.sin_zero));
		printf("\n*******Node :%d *** trying on portno : %d ",num,port);
		port=port+1;
		k++;
		char *ip = ip_addr[num];
		printf("Before connect\n");
		if (setsockopt(clientsocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		{
	    		error("setsockopt(SO_REUSEADDR) failed");
	    		printf("\nsetsockopt(SO_REUSEADDR) failed");
		}
		/*connect to the nodes to receive the encoded files*/
		if ((connect(clientsocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))==-1)
		{
			printf("Connection to %s : Failed %s \n",ip_addr[num],strerror(errno));
			//num--;
			continue;
		}
		printf("Connection to %s : Success\n",ip);
		fflush(stdout);
		num--;
		break;
		}

		printf("After connect\n");
		
		/*sending the files to the storage nodes */
		/*pid = fork();
		
	        if (pid < 0) {
		   perror("ERROR on fork");
		   exit(1);
	        }
	      
	        if (pid == 0) {	*/
	    printf("\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2");     	
		send(clientsocket,"Encode",7,0);/*option chosen*/
		printf("\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@3");
		bytes_read=recv(clientsocket,ack,sizeof(ack),0);
		ack[bytes_read]='\0';
		/*Master log file is opened to write the details of storage nodes*/
		FILE *mf,*t,*fp;
		mf=fopen("Master_file.txt","a+");
	
		strcat(file_name,"\t");/*wrting the file name*/
		fprintf(mf,"%s",file_name);
		printf("\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@4");
		t=fopen(tmp_filename,"r");/*fragment file names are stored in tmp.txt*/
		printf("\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@5");
		
		a=fscanf(t,"%s",frag_name); /*reading the names of fragment files (stem)*/
		strcpy(temp_frag_name,frag_name);
		createpath(temp_frag_name);
		printf("\n ##### C : %s",temp_frag_name);
		printf("\n  **** read : %d ",a);
		printf("\n &&  &&&& &&&& C = %s ",temp_frag_name);
	
		fp=fopen(temp_frag_name,"r"); /*opening the fragments*/
		

			
		/*sending the fragment file name nad its content*/
		send(clientsocket,frag_name,strlen(frag_name),0);
		bytes_read=recv(clientsocket,msg,sizeof(msg),0);
		msg[bytes_read]='\0';
		struct stat st;
		stat(frag_name, &st);
		int size = st.st_size;
		//printf("\n\t%s\n",msg);
		if(strcmp(msg,"recv")==0)
		{
		 	bytes_read = fread(sup, sizeof(char),sizeof(sup), fp);
			sup[bytes_read]='\0';
			//printf("\nFile content :  %s",sup);
		 	send(clientsocket,sup,strlen(sup),0);
		 	inet_ntop(AF_INET, &(serverAddr.sin_addr), str, INET_ADDRSTRLEN);
		 	fprintf(fpm,"%s  ",frag_name);
            fprintf(fpm,"%s  ",str);
            fprintf(fpm,"%d  ",size);
            fprintf(fpm,"%s","\n");
			
			printf("\nFile : %s has been sent to %s\n",frag_name,str);
		}
		//remove(frag_name);
		
		
		/*Writing in the Master log file*/	
		printf("\n******start******");
		strcat(frag_name,"\t"); /*fragment name*/
		strcat(frag_name,str);  /*ip address of the storage node receiving  above fragment*/
		strcat(frag_name,"\n");
		fprintf(mf,"%s",frag_name);	
		printf("\n******ends******");
		fclose(mf);
	
		/*remove the Fragment filename from file*/
		char b;
		int tmp=1;
		rewind(t);
		FILE *fp2;
		//open new file in write mode
		fp2 = fopen("copy.c", "w");
		b = getc(t);
		while (b != EOF) {
			  b= getc(t);
			  if (b == '\n')
			  tmp++;
		  //except the line to be deleted
			  if (tmp != 1)
			  {
		    //copy all lines in file copy.c
			    putc(b, fp2);
			  }
		}
		//close both the files.
		fclose(t);
		fclose(fp2);
		fclose(fpm);
		printf("\n****** Hello ****** \n");
		char *param[] = {"readfile","update_file_metadata","/home/user/Dropbox/Code/file8/gflib/metadata_exchangefile.txt"};
		fn_call(3,param); // call to transfer the meta data to python meta data keeper
		printf("\n I 'm back ......");
		//remove original file
		remove(tmp_filename);
		//rename the file copy.c to original name
		rename("copy.c", tmp_filename);					      
		
		//}
		//else
		//{
			close(clientsocket);
		//}
			
	}/*end of the while */
	printf("\n All files transferred");
	/*FILE *zx;
	zx=fopen("Master_file.txt","a+");	
	fprintf(zx,"%s","Stripe ends\n");
	fclose(zx);*/
	
	
   //remove("tmp.txt");
	/*socket program ends*/
}
