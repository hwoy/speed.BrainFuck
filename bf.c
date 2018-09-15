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

enum BFCODE
{
	BF_NORMAL,BF_SUCCPTR,BF_PREDPTR
};

const ip_t bfcode[]="+-><.,[]";



const cell_t* inittape(tape_t *tape, size_t size)
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

static const ip_t* openbracket(const ip_t *begin,const ip_t *end,int n)
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

static const ip_t* closebracket(const ip_t *rbegin,const ip_t *rend,int n)
{
	const ip_t *ip = rbegin;
	
	while(n && ip!=rend)
	{
		if(*ip==']') --n;
		else if(*ip=='[') ++n;
		
		if(n) --ip;
	}
	

	return ip;
}

static int bfsuccvalue(tape_t* tape)
{
	return ++(*tape->ptr),BF_NORMAL ;
}

static int bfpredvalue(tape_t* tape)
{
	return --(*tape->ptr),BF_NORMAL ;
}

static int bfsuccptr(tape_t* tape)
{
	return (tape->ptr+1 >= tape->bptr+tape->size) ? BF_SUCCPTR : (++tape->ptr,BF_NORMAL) ;
}

static int bfpredptr(tape_t* tape)
{
	return (tape->ptr<= tape->bptr) ? BF_PREDPTR : (--tape->ptr,BF_NORMAL) ;
}

static int bfputvalue(tape_t* tape)
{
	return fputc(*tape->ptr,fp),BF_NORMAL;
}

static int bfgetvalue(tape_t* tape)
{
	return *tape->ptr=getchar(),BF_NORMAL;
}

int bfeval(const ip_t *begin,const ip_t *end,tape_t* tape,FILE *fp)
{
	const ip_t *ip = begin;
	
	int bfno=BF_NORMAL;
	
	while(ip!=end && (bfno==BF_NORMAL))
	{
		switch(*ip)
		{
			case '+': bfno=bfsuccvalue(tape); break;
			case '-': bfno=bfpredvalue(tape); break;
			
			case '>': bfno=bfsuccptr(tape); break;
			case '<': bfno=bfpredptr(tape); break;
			
			case '.': bfno=bfputvalue(tape); fflush(fp); break;
			case ',': bfno=bfgetvalue(tape); break;
			
			case '[': if(!*tape->ptr) ip=openbracket(++ip,end,1); break;
			case ']': if(*tape->ptr) ip=closebracket(--ip,begin-1,-1); break;
			
			default: break;
		}
		
		if(ip!=end) ++ip;
	}
	
	return bfno;
}

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
