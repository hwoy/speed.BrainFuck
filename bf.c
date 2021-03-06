#include <stdio.h>
#include <stdlib.h>

#include "bf.h"

const inst_t bfinst[]={	
	INST_SUCCVALUE,
	INST_PREDVALUE,
	INST_SUCCPTR,
	INST_PREDPTR,
	INST_PUTVALUE,
	INST_GETVALUE,
	INST_WHILE,
	INST_ENDWHILE,
	0};

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
	
}

const_ip_t initprog(prog_t *prog, size_t size)
{
	prog->ptr = malloc(sizeof(prog_t)*size);
	prog->size = size;
	
	return prog->ptr;
}

void destroyprog(prog_t *prog)
{
	free(prog->ptr);
	prog->ptr=NULL;
}

int eleminst(inst_t inst, const inst_t *bfinst)
{
	return (inst==*bfinst) ? 1 : ((!*bfinst) ? 0 : eleminst(inst,bfinst+1));
}

static const_ip_t bfopenbracket(const_ip_t begin,const_ip_t end,int n)
{
	const_ip_t ip = begin;
	
	while(n && ip!=end)
	{
		if(*ip==INST_ENDWHILE) --n; else if(*ip==INST_WHILE) ++n;
		
		if(n) ++ip;
	}
	

	return ip;
}

static const_ip_t bfclosebracket(const_ip_t rbegin,const_ip_t rend,int n)
{
	const_ip_t ip = rbegin;
	
	while(n && ip!=rend)
	{
		if(*ip==INST_ENDWHILE) --n; else if(*ip==INST_WHILE) ++n;
		
		if(n) --ip;
	}
	

	return ip;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfsuccvalue(tape_t* tape)
{
	return ++(*tape->ptr),BF_NORMAL ;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfpredvalue(tape_t* tape)
{
	return --(*tape->ptr),BF_NORMAL ;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfsuccptr(tape_t* tape)
{
	return (tape->ptr+1 >= tape->bptr+tape->size) ? BF_SUCCPTR : (++tape->ptr,BF_NORMAL) ;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfpredptr(tape_t* tape)
{
	return (tape->ptr<= tape->bptr) ? BF_PREDPTR : (--tape->ptr,BF_NORMAL) ;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfputvalue(tape_t* tape,FILE *fp)
{
	return fputc(*tape->ptr,fp),fflush(fp),BF_NORMAL;
}

#if __STDC_VERSION__ >= 199901L
inline
#endif
static unsigned int bfgetvalue(tape_t* tape)
{
	return *tape->ptr=getchar(),BF_NORMAL;
}

unsigned int bfeval(const_ip_t begin,const_ip_t end,tape_t* tape,FILE *fp)
{
	const_ip_t ip = begin;
	
	unsigned int bfno=BF_NORMAL;
	
	while(ip!=end && (bfno==BF_NORMAL))
	{
		switch(*ip)
		{
			case INST_SUCCVALUE:	bfsuccvalue(tape); break;
			case INST_PREDVALUE: bfpredvalue(tape); break;
			
			case INST_SUCCPTR:	bfno=bfsuccptr(tape); break;
			case INST_PREDPTR:	bfno=bfpredptr(tape); break;
			
			case INST_PUTVALUE:	bfputvalue(tape,fp); break;
			case INST_GETVALUE:	bfgetvalue(tape); break;
			
			case INST_WHILE:		if(!*tape->ptr)	ip=bfopenbracket(++ip,end,1); break;
			case INST_ENDWHILE:	if(*tape->ptr)	ip=bfclosebracket(--ip,begin-1,-1); break;
			
			default: break;
		}
		
		if(ip!=end) ++ip;
	}
	
	return bfno;
}
