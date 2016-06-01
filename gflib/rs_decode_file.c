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
#include<unistd.h>
#include<sys/errno.h>
#include<sys/socket.h>//Added by : Supriya
#include<netinet/in.h>//Added by : Supriya
#include<string.h>//Added by : Supriya
#include "rs_encode_file.h"
#include "fn_call_function.h" // added by ojus @info for python C interconnection



/* This one is going to be in-core */

int decode(char *stem,char *file_name)
{
  int i, j, k, *vdm, *inv, *prod, cache_size;
  int rows, cols, blocksize, orig_size;
  int n, m, sz, *factors, tmp, factor, *exists, *map;
  char *filename; 
  char **buffer, buf_file[1024], *block;
  struct stat buf;
  Condensed_Matrix *cm;
  int *mat, *id;
  FILE *f;

  /*if (argc != 3) { 
    fprintf(stderr, "usage: rs_decode_file stem file_name\n");
    exit(1);
  }
  
  file_name=argv[2];

  stem = argv[1];*/
  //buf_file = (char *) malloc(100*sizeof(char));
  if (buf_file == NULL) { perror("malloc - buf_file"); exit(1); }
  sprintf(buf_file, "/home/user/Dropbox/Code/file8/gflib/MetaData/%s-info.txt", stem, i);
  //createpath(buf_file);
  printf("\n File -- > %s \n ", buf_file);
  f = fopen(buf_file, "r");
  printf("\n &&&&&&&&&&&&&&&&&&&&&&&&&&");
  printf("\n Stem : %s , Filename : %s ", stem, file_name);
  if (f == NULL) { perror(buf_file); exit(1); }
  if (fscanf(f, "%d\n", &orig_size) != 1) { fprintf(stderr, "Error reading info file 1\n"); exit(1); }
  if (fscanf(f, "%d\n", &sz) != 1) { fprintf(stderr, "Error reading info file 2\n"); exit(1); }
  if (fscanf(f, "%d\n", &blocksize) != 1) { fprintf(stderr, "Error reading info file 3\n"); exit(1); }
  if (fscanf(f, "%d\n", &n) != 1) { fprintf(stderr, "Error reading info file 4\n"); exit(1); }
  if (fscanf(f, "%d\n", &m) != 1) { fprintf(stderr, "Error reading info file 5\n"); exit(1); }
  vdm = gf_read_matrix(f, &rows, &cols);
  if (vdm == NULL) { fprintf(stderr, "Error reading info file matrix\n"); exit(1); }
  fclose(f);
  
  if (rows != n+m) {
    fprintf(stderr, "Error in %s - rows != n+m\n", buf_file);
    exit(1);
  }
  if (cols != n) {
    fprintf(stderr, "Error in %s - cols != n\n", buf_file);
    exit(1);
  }

  exists = (int *) malloc(sizeof(int) * rows);
  if (exists == NULL) { perror("malloc - exists"); exit(1); }
  factors = (int *) malloc(sizeof(int) * rows);
  if (factors == NULL) { perror("malloc - factors"); exit(1); }
  map = (int *) malloc(sizeof(int) * rows);
  if (map == NULL) { perror("malloc - map"); exit(1); }

  buffer = (char **) malloc(sizeof(char *)*n);
  for (i = 0; i < n; i++) {
    buffer[i] = (char *) malloc(blocksize);
    if (buffer[i] == NULL) {
      perror("Allocating buffer to store the whole file");
      exit(1);
    }
  }

	/* Added by : Supriya */
	/*Socket program in order to fetch all the encoded files from different nodes*/

	
	int clientsocket;/* Socket descriptor for client */
	int enable=1,num=n,flag=0,p=0,pos,port=7855; 
	char file[40]="",stem_filename[1024]="",ip_addr[40]="",ack[10];
	char sup[1024]="",c,line[100]="",search_string[30]="",cwd[1024] = "";
	int bytes_received,y;
	strcpy(search_string,file_name);
	//printf("\n%s***",search_string);
	struct sockaddr_in serverAddr;/* client address */
	socklen_t addr_size;
  printf("\n------------- Filename to be send %s ",file_name);
	char *param[] = {"readfile","read_file_metadata",file_name};
  fn_call(3,param); // call to transfer the meta data to python meta data keeper
  printf("\n I 'm back ......");
	
	/*opening the master to file*/
	FILE *fp,*fq;
	fp=fopen("filefrags_meta_file_read.txt","r");
	
	if(!fp)
  {
      perror("\nCould not find the Master file");
      return 0;
  }
	//while ( c!= EOF )/* read a line */
  /*{
		c=fscanf( fp, "%s",line ) ;
		//printf("%s\n",line);
    if(strstr(line,search_string)){
			p=1;
			break;	
		}
  }
		//printf("ssss");
		pos=ftell(fp);
		//printf("%d\n",n);
		pos=pos-strlen(search_string);
		fseek( fp, pos, SEEK_SET );
		if(p!=1){
			perror("\nNo such File exists in the encoded file list");
			return 0;
		}*/
		//printf("%d\n",fseek( fp, n, SEEK_SET ));
		//c=fscanf( fp, "%s",line ) ;
		//printf("%s\n",line);

	

	printf("@@@@@@@@@@@@@@@ %d\n",num);
	while(num>0)
	{
	strcpy(sup,"");
	/*reading the master file to get the ip address of different nodes*/
	fscanf(fp,"%s",file);
	//
	fscanf(fp,"%s",ip_addr);
  char temp[30];
  fscanf(fp,"%s",temp);
  strcpy(stem_filename,file);
	//fscanf(fp,"%s",port);
	printf("%s...%s...%s\n",file,ip_addr,temp);

	/* Create socket for connections */
	clientsocket=socket(PF_INET,SOCK_STREAM,0);
	printf("\n\nSocket creation: Success\n");//%s %d\n",strerror(errno));

	//printf("%d\n",clientsocket);
	port=port+1;
	num=num-1;
	
	//if(errno!=0)
	//	continue;

	
	/* Construct local address structure */
	serverAddr.sin_family=AF_INET; /* Internet address family */
	serverAddr.sin_port = htons(port);/*server port*/
	serverAddr.sin_addr.s_addr=inet_addr(ip_addr);/*server address*/
	memset(serverAddr.sin_zero,'\0',sizeof serverAddr.sin_zero);/* Zero out structure */ 

	
	
	printf("Before connect\n");
	/*reuse the same port*/
	if (setsockopt(clientsocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    		error("setsockopt(SO_REUSEADDR) failed");

	/*connect to the nodes to receive the encoded files*/
	if ((connect(clientsocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))==-1)
		continue;
		printf("Connection: Success\n");

	//if(errno!=0)
		//continue;
	    //printf("Connected to the node with ip address : %s\n",ip_addr);

	
	printf("After connect\n");

	send(clientsocket,"Decode",7,0);
	bytes_received=recv(clientsocket,ack,sizeof(ack),0);
  bytes_received = 0;
  printf("\n ACK --> %s ",ack);
	send(clientsocket,stem_filename,strlen(stem_filename),0);
	bytes_received=recv(clientsocket,sup,sizeof(sup),0);

	sup[bytes_received]='\0';
	printf("AAAAA ************** bytes received : %d ",bytes_received);

	/*checking if the file has been received or not */
	if(strcmp(sup,"File does not exist")!=0){

		/*write the received content into file*/
    
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
    // Writing the fragments to Data folder in the storage node, added by Ojus  # start
    mkdir("ReceivedData", S_IRUSR | S_IWUSR | S_IXUSR);
    printf("\n &&&&&&&&&&&&&&&&&&&&&&&&&& 777");
    strcat(cwd,"/ReceivedData/");
    strcat(cwd,stem_filename);
    strcpy(stem_filename,cwd);
    printf("\n stem_file : %s ", stem_filename);
	
		FILE *ft;
		ft=fopen(stem_filename,"wb");
		
		flag = fwrite(sup, sizeof(char), bytes_received, ft);
    printf("\n #### content: \n");
    fwrite(sup, sizeof(char), bytes_received, stdout);
		
		//fprintf(ft,"%s",sup);
		flag++;
		fclose(ft);

		
	}
	else{
		printf("\n BBBBBBB %s \n",sup);		
         }

		//printf("%d\t%d\n",num,port);
	}	
	fclose(fp);
	

fflush(stdin);
	/*if(flag<n){
	   fprintf(stderr, "Only %d fragments -- need %d.  Sorry\n", flag, n);
   	   exit(1);
	}*/
/*end of socket program*/
  sprintf(file, "%s-gen.rs", stem);
	for(i=0;i< rows;i++)
	{
		map[i] = 1;
		printf("\n @@@@@ ******************** map[%d] : %d ",i,map[i]);
	}

  j = 0;
  printf("\n ^^^^^ rows : %d",rows);
  for (i = 0; i < rows && j < cols; i++) {
	  printf("\n i = %d",i);
    sprintf(buf_file, "%s-%04d.rs", stem, i);
    if (getcwd(cwd, sizeof(cwd)) != NULL)
            fprintf(stdout, "Current working dir: %s\n", cwd);
        else
    perror("getcwd() error");
    mkdir("ReceivedData", S_IRUSR | S_IWUSR | S_IXUSR);
    strncat(cwd,"/ReceivedData/",sizeof("/ReceivedData/"));
    strcat(cwd,buf_file);
    printf("\n cwd : %s ",cwd);
    strcpy(buf_file,"\0");
    strcat(buf_file,cwd);
    printf("\n buf_file : %s ",buf_file);
    if (stat(buf_file, &buf) != 0) {
	     printf("\nfailed stem1= ****%s-%04d.rs", stem, i);
        map[i] = -1;
    } else 
    {
        if (buf.st_size != blocksize) {
	      printf("\nfailed stem2= ****%s-%04d.rs", stem, i);
        map[i] = -1;
    } else
    {
        printf("\nstem= ****%s-%04d.rs", stem, i);//testing
	      printf("\n j=%d",j);//testing
		    //printf("\n******************** map[%d] : %d ",i,map[i]);
        //if(i<4){
		    map[i] = j++;
		// }
        //printf("\n**********buf_file : %s ********* map[%d] : %d ",buf_file, i,map[i]);

        f = fopen(buf_file, "rb");
        if (f == NULL) { perror(buf_file); exit(1); }
        k = fread(buffer[i], 1, blocksize, f);
        printf("\n k = %d , blocksize = %d \n ********* \n content : %s \n ***********\n ",k,blocksize,buffer[map[i]]);
        if (k != blocksize) {
          fprintf(stderr, "%s -- stat says %d bytes, but only read %d\n",buf_file, buf.st_size, k);
          exit(1);
        }
        fclose(f);
      }
    }
  }
 
printf("\n********************");
fq = fopen(file,"w");

  if (j < cols) {
    fprintf(stderr, "Only %d fragments -- need %d.  Sorry\n", j, cols);
    return 0;
  }
  
  j = 0;
  for (i = 0; i < cols; i++) if (map[i] == -1) j++;
  fprintf(stderr, "Blocks to decode: %d\n", j);
  if (j == 0) {
    cache_size = orig_size;
    for (i = 0; i < cols; i++) {
      if (cache_size > 0) {
        fwrite(buffer[i], 1, (cache_size > blocksize) ? blocksize : cache_size, stdout);
        fwrite(buffer[i], 1, (cache_size > blocksize) ? blocksize : cache_size,fq);
        cache_size -= blocksize;
        
      }
    }
    fclose(fq);
    return 0;
  } 

  block = (char *) malloc(sizeof(char)*blocksize);
  if (block == NULL) { perror("malloc - block"); exit(1); }
  
  for (i = 0; i < rows; i++) exists[i] = (map[i] != -1);
  cm = gf_condense_dispersal_matrix(vdm, exists, rows, cols);
  mat = cm->condensed_matrix;
  id = cm->row_identities;
  /* Fix it so that map[i] for i = 0 to cols-1 is defined correctly.
     map[i] is the index of buffer[] that holds the blocks for row i in 
     the condensed matrix */

  for (i = 0; i < cols; i++) {
    if (map[i] == -1) map[i] = map[id[i]];
  }

  fprintf(stderr, "Inverting condensed dispersal matrix ... "); fflush(stderr);
  inv = gf_invert_matrix(mat, cols);
  if (inv == NULL) {
    fprintf(stderr, "\n\nError -- matrix unvertible\n");
    exit(1);
  }
  fprintf(stderr, "Done\n"); fflush(stderr);
  
  fprintf(stderr, "\nCondensed matrix:\n\n");
  gf_fprint_matrix(stderr, mat, cols, cols);

  fprintf(stderr, "\nInverted matrix:\n\n");
  gf_fprint_matrix(stderr, inv, cols, cols);

  for(i = 0; i < rows; i++) factors[i] = 1;

  cache_size = orig_size;
  for (i = 0; i < cols && cache_size > 0; i++) {
    if (id[i] < cols) {
      fprintf(stderr, "Writing block %d from memory ... ", i); fflush(stderr);
      if (factors[i] != 1) {
        tmp = gf_single_divide(1, factors[i]);
/*        fprintf(stderr, "Factor = %3d.  Tmp = %3d.  Before[0] = %3d.  ",
                factors[i], tmp, (unsigned char) buffer[map[i]][0]); */
        factors[i] = 1;
        gf_mult_region(buffer[map[i]], blocksize, tmp);
/*        fprintf(stderr, "After[0] = %3d.\n", (unsigned char) buffer[map[i]][0]); */
      } else {
/*        fprintf(stderr, "Factor = %3d.  Buffer[0] = %3d.\b", factors[i], 
             (unsigned char) buffer[map[i]][0]); */
      }
      fwrite(buffer[map[i]], 1, (cache_size > blocksize) ? blocksize : cache_size, stdout);
      cache_size -= blocksize;
      fprintf(stderr, "Done\n"); fflush(stderr);
    } else {
      fprintf(stderr, "Decoding block %d ... ", i); fflush(stderr);
      memset(block, 0, blocksize);
      for (j = 0; j < cols; j++) {
        tmp = inv[i*cols+j];
        factor = gf_single_divide(tmp, factors[j]);
/*        fprintf(stderr, "Factors[%d] = %3d.  Tmp = %3d.  Factor = %3d\n    Before[j][0] = %3d.  ", 
                j, factors[j], tmp, factor, (unsigned char) buffer[map[j]][0]); */
        factors[j] = tmp;
        gf_mult_region(buffer[map[j]], blocksize, factor);
/*        fprintf(stderr, "After[j][0] = %3d.  ", (unsigned char) buffer[map[j]][0]);
        fprintf(stderr, "Before-block[0] = %3d.  ", (unsigned char) block[0]); */
        gf_add_parity(buffer[map[j]], block, blocksize);
/*        fprintf(stderr, "After-block[0] = %3d.\n", (unsigned char) block[0]); */
      }
      fprintf(stderr, "writing ... "); fflush(stderr);
      fwrite(block, 1, (cache_size > blocksize) ? blocksize : cache_size, stdout);
      //fwrite(block, 1, (cache_size > blocksize) ? blocksize : cache_size, fq);
      cache_size -= blocksize;
      fprintf(stderr, "Done\n"); fflush(stderr);
      fclose(fq);
    }
  }

return 1;
}

