
#include <stdio.h>
#include <string.h>

#define IRREGEX_IMPLEMENTATION
#include "irregex.h"

int main(void) 
{
	ix_tokens toks = _ix_tokenize("abcd\\*abc");
	
	for (size_t i = 0; i < toks.len; i++)
	{
		printf("t %d: %zu %zu\n", toks.data[i].type, toks.data[i].val.lit.start, toks.data[i].val.lit.len);
	}
	
	return 0;
}