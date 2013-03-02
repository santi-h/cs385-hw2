#ifndef MYTYPES_H
#define MYTYPES_H

#include <stdint.h>
#include <sys/types.h>

typedef enum {READ_ERR, DONE} opcodes_e;

typedef struct 
{
	long workerID;
	opcodes_e op;
} msg_hdr;

typedef struct
{
	int buf_idx;
	int ini_value;
	int fin_value;	
} msg_readerr;


#endif //MYTYPES_H


