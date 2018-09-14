#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPESIZE 1024
#define PROGSIZE (64*1024)

typedef char cell_t;
typedef char ip_t;

typedef struct{
	cell_t *ptr;
	cell_t *bptr;
	size_t size;
}tape_t;

cell_t* inittape(tape_t *tape, size_t size)
{
	size_t i;
	
	tape->bptr=tape->ptr = malloc(sizeof(cell_t)*size);
	if(tape->ptr)
	{
		tape->size=size;
		for(i=0;i<size;++i)
			tape->ptr[i]=0;
	}
	
	return tape->ptr;
}

void destroytape(tape_t *tape)
{
	free(tape->bptr);
	tape->bptr=tape->ptr=NULL;
	tape->size=0;
}

int elembf(ip_t ip, const ip_t *bfip)
{
	return (ip==*bfip) ? 1 : ((!*bfip) ? 0 : elembf(ip,bfip+1));
}

const ip_t* openbracket(const ip_t *begin,const ip_t *end,int n)
{
	const ip_t *ip = begin;
	
	while(n && ip!=end)
	{
		if(*ip==']') --n;
		else if(*ip=='[') ++n;
		
		if(n) ++ip;
	}
	
	return ip;
}

const ip_t* closebracket(const ip_t *begin,const ip_t *end,int n)
{
	const ip_t *ip = begin;
	
	while(n && ip!=end)
	{
		if(*ip==']') --n;
		else if(*ip=='[') ++n;
		
		if(n) --ip;
	}
	
	return ip;
}

int bfeval(const ip_t *begin,const ip_t *end,tape_t* tape)
{
	const ip_t *ip = begin;
	
	while(ip!=end)
	{
		switch(*ip)
		{
			case '+': if(tape->ptr+1 >= tape->bptr+tape->size) return 1; ++(*tape->ptr); break;
			case '-': if(tape->ptr<= tape->bptr) return 2; --(*tape->ptr); break;
			case '>': ++tape->ptr; break;
			case '<': --tape->ptr; break;
			case '.': putchar(*tape->ptr); fflush(stdout); break;
			case ',': *tape->ptr=getchar(); break;
			case '[': if(!*tape->ptr) ip=openbracket(++ip,end,1); break;
			case ']': if(*tape->ptr) ip=closebracket(--ip,begin,-1); break;
			default: break;
		}
		
		if(ip!=end) ++ip;
	}
	
	return 0;
}

static const ip_t bfcode[]="+-><.,[]";

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


int main(void)
{
	FILE *fp;
	int ch;
	tape_t tape;
	ip_t *prog;
	size_t size;
	
	if(!(fp=fopen("a.bf","r")))
	{
		fprintf(stderr,"Error file\n");
		return 1;
	}
	
	
	if(!inittape(&tape,TAPESIZE))
	{
		fprintf(stderr,"Error alloc mem for tape\n");
		fclose(fp);
		return 1;
	}
	
	if(!(prog=malloc(sizeof(ip_t)*PROGSIZE)))
	{
		fprintf(stderr,"Error alloc mem for tape\n");
		fclose(fp);
		destroytape(&tape);
		return 1;
	}
	
	
	while((ch=fgetc(fp))!=EOF)
	{
		if(!elembf(ch,bfcode)) continue;
		*prog=ch;size=1;
		
		switch(ch)
		{
			case '[': size+=gbracket(fp,prog+1,PROGSIZE-1,1); break;
			case ']': break;
			default: break;
		}
		bfeval(prog,prog+size,&tape);
	}
	
	fclose(fp);
	destroytape(&tape);
	free(prog);
	
	return 0;
}
