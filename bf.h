#ifndef __BF_H__
#define __BF_H__

#ifndef CELL_T
#define CELL_T char
#endif


typedef CELL_T cell_t;
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

enum BFINST
{
	INST_SUCCVALUE='+',
	INST_PREDVALUE='-',
	INST_SUCCPTR='>',
	INST_PREDPTR='<',
	INST_PUTVALUE='.',
	INST_GETVALUE=',',
	INST_WHILE='[',
	INST_ENDWHILE=']'
	
};

extern const ip_t bfcode[];


const cell_t* inittape(tape_t *tape, size_t size);

void destroytape(tape_t *tape);

int elembf(ip_t ip, const ip_t *bfip);

int bfeval(const ip_t *begin,const ip_t *end,tape_t* tape,FILE *fp);


#endif