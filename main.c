#include <stdio.h>
#include <stdlib.h>

#include "bf.h"

#define TAPESIZE 1024
#define PROGSIZE (64*1024)


size_t gbracket(FILE *fp,ip_t *prog,size_t size,int n)
{
	size_t i=0;
	int ch;
	
	while(n && i<size && ((ch=fgetc(fp))!=EOF))
	{
		if(!elembf(ch,bfcode)) continue;
		
		if(ch==']') --n;
		else if(ch=='[') ++n;
		prog[i++]=ch;
	}
	
	return i;
}


int main(int argc ,const char *argv[])
{
	FILE *fin,*fout=stdout;
	int ch;
	tape_t tape;
	ip_t *prog;
	size_t size;
	
	if(!(fin=fopen("a.bf","r")))
	{
		fprintf(stderr,"Error file-in\n");
		return 1;
	}
/*	
	if(!(fout=fopen("a.txt","wb")))
	{
		fprintf(stderr,"Error file-out\n");
		fclose(fin);
		return 1;
	}
*/	
	
	if(!inittape(&tape,TAPESIZE))
	{
		fprintf(stderr,"Error alloc mem for tape\n");
		fclose(fin);
		fclose(fout);
		return 1;
	}
	
	if(!(prog=malloc(sizeof(ip_t)*PROGSIZE)))
	{
		fprintf(stderr,"Error alloc mem for tape\n");
		fclose(fin);
		fclose(fout);
		destroytape(&tape);
		return 1;
	}
	
	
	while((ch=fgetc(fin))!=EOF)
	{
		switch(ch)
		{
			case '[': *prog=ch;size=gbracket(fin,prog+1,PROGSIZE-1,1)+1; break;
			case ']': break;
			case '+': 
			case '-': 
			case '>': 
			case '<': 
			case '.': 
			case ',': *prog=ch;size=1; break;
			default: continue;
		}
		
		bfeval(prog,prog+size,&tape,fout);
	}
	
	fclose(fin);
	fclose(fout);
	destroytape(&tape);
	free(prog);
	
	return 0;
}
