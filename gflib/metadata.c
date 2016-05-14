#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void metdata(char *filename);
void main()
{
	metdata("sup1.txt");
}
void metdata(char *filename)
{
	int i,j;
	char **arr;

	FILE *ptr;
	ptr=fopen("counter.txt","r");

	fscanf(ptr,"%d",&i);

	arr=(char **) malloc(sizeof(char *)*10);
	arr[i] = (char *) malloc(sizeof(char)*50);

	strcpy(arr[i],filename);

	for(j=0; j<=i; j++)
		printf("\n%s",arr[j]);

	i=i+1;

	fclose(ptr);
	remove("counter.txt");
	
	ptr=fopen("counter.txt","w");

	fprintf(ptr,"%d",i);
	fclose(ptr);
	
}
