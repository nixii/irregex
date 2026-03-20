
#ifndef IRREGEX_H
#define IRREGEX_H

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

// dynamic array implementation
#define DA_INIT_CAPACITY 4
#define DA_GROW_SCALE 2
#define DA_HEADERS(type, name) \
	typedef struct {\
		type *data;\
		size_t cap;\
		size_t len;\
	} name;\
	name name##_new();\
	void name##_append(name *da, type data);\
	void name##_destroy(name *da);

#define DA_IMPLEMENTATION(type, name)\
	name name##_new() {\
		type *data = (type*)malloc(sizeof(type) * DA_INIT_CAPACITY);\
		name da = (name){\
			.data = data,\
			.cap = DA_INIT_CAPACITY,\
			.len = 0\
		};\
		if (data == NULL)\
			da.cap = 0;\
		return da;\
	}\
	void name##_extend(name *da) {\
		if (da->cap >= da->len + 1 || da->cap == 0) return;\
		da->cap *= DA_GROW_SCALE;\
		type *new_data = (type*)realloc(da->data, sizeof(type) * da->cap);\
		if (new_data == NULL) {\
			free(da->data);\
			da->cap = 0;\
		}\
		da->data = new_data;\
	}\
	void name##_append(name *da, type data) {\
		name##_extend(da);\
		if (da->data == NULL)\
			return;\
		da->data[da->len] = data;\
		da->len++;\
	}\
	void name##_destroy(name *da){\
		free(da->data);\
		da->cap = 0;\
		da->data = NULL;\
		da->len = 0;\
	}

// token type
typedef enum {
	IX_TT_LITERAL,
	IX_TT_WILDCARD,
	IX_TT_ERR
} ix_token_type;

// literal value
typedef struct {
	size_t start;
	size_t len;
} ix_tv_literal;

// token val
typedef union {
	ix_tv_literal lit;
} ix_token_val;

// create a token
typedef struct {
	ix_token_type type;
	ix_token_val val;
} ix_token;

// create an irregex view
typedef struct {
	char *source;
} irregex;

// create dynamic arrays
DA_HEADERS(ix_token, ix_tokens)

// actually compile regex

// tokenize a single literal from the input.
// backend
ix_token _ix_tokenize_literal(const char *input, size_t *pos, size_t len);

// tokenize a single expression from the input.
// backend
ix_token _ix_tokenize_expr(const char *input, size_t *pos, size_t len);

// tokenize the whole thing into a dynamic array.
ix_tokens _ix_tokenize(const char *input);

// compile a new irregex expression
irregex irregex_from(const char *input);

// destroy your irregex expression
void irregex_destroy(irregex *irx);

#endif // IRREGEX_H



#ifdef IRREGEX_IMPLEMENTATION

extern int errno;

// define dynamic arrays
DA_IMPLEMENTATION(ix_token, ix_tokens);

// start parsing
ix_token _ix_tokenize_literal(const char *input, size_t *pos, size_t len)
{

	// loop through each char
	size_t start = *pos;
	size_t length = 0;
	
	while (*pos < len)
	{
		char c = input[*pos];
		
		switch (c)
		{
		
		// end if you find a new thing
		case '\\':
			(*pos)--;
			goto complete;
			
		// add the next character
		default:
			length++;
			break;
		}
		(*pos)++;
	}

	// when it is done
complete:
	return (ix_token){
		.type = IX_TT_LITERAL,
		.val = (ix_tv_literal){
			.start = start,
			.len = length
		}
	};
}

ix_token _ix_tokenize_expr(const char *input, size_t *pos, size_t len)
{
	
	// loop through all the operators
	(*pos)++;
	while (*pos < len)
	{
		char c = input[*pos];
		
		switch (c)
		{
		
		// wildcard
		case '*': // wildcard
			return (ix_token){
				.type = IX_TT_WILDCARD
			};
		
		// invalid operator
		default:
			errno = 1;
			return (ix_token){
				.type = IX_TT_ERR
			};
		}
		
		(*pos)++;
	}
	
	// nothing was found
	errno = 1;
	return (ix_token){
		.type = IX_TT_ERR
	};
}

ix_tokens _ix_tokenize(const char *input)
{

	// init the loop
	ix_tokens tokens = ix_tokens_new();
	size_t max_chars = strlen(input);
	size_t idx = 0;
	
	// for each letter in the loop
	while (idx < max_chars)
	{
		char c = input[idx];
		switch (c)
		{
		
		// use an expression
		case '\\':
			errno = 0;
			ix_tokens_append(&tokens, _ix_tokenize_expr(input, &idx, max_chars));
			if (errno != 0)
			{
				ix_tokens_destroy(&tokens);
				return tokens;
			}
			break;
		
		// use a literal
		default:
			ix_tokens_append(&tokens, _ix_tokenize_literal(input, &idx, max_chars));
		}
		idx++;
	}
	
	// return the tokens
	return tokens;
}

// create an irregex
irregex irregex_from(const char *input)
{
	char *chars = strdup(input);
	
	ix_tokens tokens = _ix_tokenize(chars);
	ix_tokens_destroy(&tokens);
	
	return (irregex){
		.source = chars
	};
}

// destroy an irregex instance
void irregex_destroy(irregex *irx)
{
	free(irx->source);
}

#endif // IRREGEX_IMPLEMENTATION