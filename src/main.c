
#include <stdio.h>

#define IRREGEX_IMPLEMENTATION
#include "../irregex.h"

int main(void)
{
	char *pattern = "test";
	char *string = "this is a test";

	da_irx_token tokens = irx_tokenize(pattern);
	
	return 0;
}