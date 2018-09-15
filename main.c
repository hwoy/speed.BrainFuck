#include <stdio.h>
#include <stdlib.h>

#include "bf.h"

#define TAPESIZE 1024
#define PROGSIZE (64*1024)


static size_t gbracket(FILE *fp,ip_t *prog,size_t size,int n)
{
	size_t i=0;
	int inst;
	
	while(n && i<size && ((inst=fgetc(fp))!=EOF))
	{
		if(elembf(inst,bfcode))
		{
			if(inst==INST_ENDWHILE) --n; else if(inst==INST_WHILE) ++n;
			prog[i++]=inst;
		}
	}
	
	return i;
}


int main(int argc ,const char *argv[])
{
	FILE *fin,*fout=stdout;
	int inst;
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
	
	if(!(prog=malloc(sizeof(ip_t)*(PROGSIZE+1))))
	{
		fprintf(stderr,"Error alloc mem for tape\n");
		fclose(fin);
		fclose(fout);
		destroytape(&tape);
		return 1;
	}
	
	
	while((inst=fgetc(fin))!=EOF)
	{
		switch(inst)
		{
			case INST_WHILE: *prog=inst;size=gbracket(fin,prog+1,PROGSIZE-1,1)+1; break;
			case INST_ENDWHILE: break;
			case INST_SUCCVALUE: 
			case INST_PREDVALUE: 
			case INST_SUCCPTR: 
			case INST_PREDPTR: 
			case INST_PUTVALUE: 
			case INST_GETVALUE: *prog=inst;size=1; break;
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
