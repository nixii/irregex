
#include <stdio.h>
#include <string.h>

#define IRREGEX_IMPLEMENTATION
#include "irregex.h"

int main(void) 
{
	irregex ix = irregex_from("abcd");
	
	irregex_destroy(&ix);
	
	return 0;
}