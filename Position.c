#include "Position.h"

Position* pstPush(unsigned long _val, Position* _pst)
{
	Position* ret = malloc(sizeof(*ret));
	if(!ret) return _pst;
	
	ret->value = _val;
	ret->begin = (_pst ? _pst->begin : ret);
	ret->next = 0;
	
	if(_pst) _pst->next = ret;
	return ret;
}

void pstFree(Position* _pst)
{
	if(!_pst) return;
	Position* begin = _pst->begin;
	while(begin)
	{
		_pst = begin->next;
		free(begin);
		begin = _pst;
	}
}