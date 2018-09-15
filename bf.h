#ifndef __BF_H__
#define __BF_H__

#ifndef CELL_T
#define CELL_T char
#endif


typedef CELL_T cell_t;
typedef char inst_t;
typedef inst_t *ip_t;
typedef const inst_t *const_ip_t;

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

extern const inst_t bfinst[];


const cell_t* inittape(tape_t *tape, size_t size);

void destroytape(tape_t *tape);

int eleminst(inst_t inst, const inst_t *bfinst);

int bfeval(const_ip_t begin,const_ip_t end,tape_t* tape,FILE *fp);


#endif