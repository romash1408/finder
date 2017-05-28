#ifndef POSITION_H
#define POSITION_H

#include <stdlib.h>

typedef struct position_s Position;
struct position_s{
	unsigned long value;
	Position* begin;
	Position* next;
};

Position* pstPush(unsigned long _val, Position* _pst);
void pstFree(Position* _pst);

#endif