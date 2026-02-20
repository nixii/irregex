
#include <stdio.h>
#include "../irregex.h"

int main(void)
{

	irregex my_irx = irx_new("{int|float|double|void|return|for|while}"); // match some keywords
	irregex id = irx_new("(*{ |\n}){_|(A-Z)|(a-z)}(*{_|(0-9)|(A-Z)|(a-z)})"); // match identifiers / keywords
	
	irx_res r = irx_matchfirst(&id, "let x = 10"); // { .start = 0, .len = 3 }; if len == 0, then it didn't match

	printf("Hello, world!\n");
	return 0;
}

