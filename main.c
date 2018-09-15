#include <stdio.h>
#include <stdlib.h>

#include "bf.h"

#ifndef TAPESIZE
#define TAPESIZE (1*1024)
#endif

#ifndef PROGSIZE
#define PROGSIZE (64*1024)
#endif

#define BITPERBYTE 8

enum STATE
{
	STATE_NORMAL_EVAL=BF_NORMAL,
	STATE_ERR_EVAL_SUCCPTR=BF_SUCCPTR,
	STATE_ERR_EVAL_PREDPTR=BF_PREDPTR,
	STATE_ERR_EVAL_WHILE,
	STATE_ERR_EVAL_ENDWHILE,
	STATE_ERR_FIN,
	STATE_ERR_FOUT,
	STATE_ERR_MEMTAPE,
	STATE_ERR_MEMPROG
};

static const char *statemsg[]={
	"Normal Operation",
	"Can not SUCC PTR, increase TAPESIZE",
	"Can not PRED PTR, check your bf code",
	"Long while loop or [ not match ], check your bf code or increase PROGSIZE",
	"] not match [, check your bf code",
	"Can not access input File:",
	"Can not access output File:",
	"Can not alloc memory for TAPE",
	"Can not alloc memory for PROG",
	NULL
};

static int showerr(const char *statemsg[],size_t state,const char *str)
{
	#if __STDC_VERSION__ >= 199901L
	fprintf(stderr,"\nERROR %zu: %s %s\n",state,statemsg[state],str);
	
	#else
	fprintf(stderr,"\nERROR %u: %s %s\n",state,statemsg[state],str);

	#endif

	return state;
}

static const char *grappath(const char *path)
{
	const char *gpath;
	
	for(gpath=path;*path;++path)
		if(*path== '\\' || *path== '/') gpath=path+1;
	
	return gpath;

}

static int showhelp(const char *path)
{
	fprintf(stderr,"\n%s is a brainf**k interpreter.\n\n",path);
	fprintf(stderr,"USAGE: %s infile\n",path);
	fprintf(stderr,"USAGE: %s infile outfile\n",path);
	
	fprintf(stderr,"\nTAPESIZE = %u, PROGSIZE = %u, size of a cell = %u bits\n",TAPESIZE,PROGSIZE,sizeof(cell_t)*BITPERBYTE);
	
	return 1;
}


static size_t gbracket(FILE *fp,ip_t progptr,size_t size,int n)
{
	size_t i=0;
	int inst;
	
	while(n && i<size && ((inst=fgetc(fp))!=EOF))
	{
		if(eleminst(inst,bfinst))
		{
			if(i>=size) return i;
				
			if(inst==INST_ENDWHILE) --n; else if(inst==INST_WHILE) ++n;
			
			progptr[i++]=inst;
		}
	}
	
	return i;
}

static int bfevalstream(FILE *fin,FILE *fout,tape_t *tape,prog_t *prog)
{
	size_t size,state=STATE_NORMAL_EVAL;
	int inst;
	
	while((state==STATE_NORMAL_EVAL) && (inst=fgetc(fin))!=EOF)
	{
		switch(inst)
		{
			case INST_WHILE: *prog->ptr=inst;
							size=gbracket(fin,prog->ptr+1,prog->size-1,1)+1;
							
							if(size>=prog->size) 
								return STATE_ERR_EVAL_WHILE;
							
							break;
							
			case INST_ENDWHILE: return STATE_ERR_EVAL_ENDWHILE;
			
			case INST_SUCCVALUE: 
			case INST_PREDVALUE: 
			case INST_SUCCPTR: 
			case INST_PREDPTR: 
			case INST_PUTVALUE: 
			case INST_GETVALUE: *prog->ptr=inst;size=1; break;
			
			default: continue;
		}
		
		state=bfeval(prog->ptr,prog->ptr+size,tape,fout);
	}
	
	return state;
	
}


int main(int argc ,const char *argv[])
{
	FILE *fin,*fout=stdout;
	tape_t tape;
	prog_t prog;
	int ret=0;
	
	if(argc<=1)
		return showhelp(grappath(argv[0]));
	
	if(argc>=2)
	{
		if(!(fin=fopen(argv[1],"r")))
		{
			return showerr(statemsg,STATE_ERR_FIN,argv[1]);
		}		
	}
	
	if(argc>=3)
	{
		if(!(fout=fopen(argv[2],"wb")))
		{
			fclose(fin);
			return showerr(statemsg,STATE_ERR_FOUT,argv[2]);
		}
	}

	if(!inittape(&tape,TAPESIZE))
	{
		fclose(fout);
		fclose(fin);
		return showerr(statemsg,STATE_ERR_MEMTAPE,NULL);
	}
	
	if(!initprog(&prog,PROGSIZE))
	{
		destroytape(&tape);
		fclose(fout);
		fclose(fin);
		return showerr(statemsg,STATE_ERR_MEMPROG,NULL);
	}
	
	{
		size_t state;
		if((state=bfevalstream(fin,fout,&tape,&prog))!=STATE_NORMAL_EVAL)
			ret=showerr(statemsg,state,NULL);
	}
	
	destroyprog(&prog);
	destroytape(&tape);
	fclose(fout);
	fclose(fin);
	
	return ret;
}
