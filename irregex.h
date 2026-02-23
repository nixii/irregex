
// embed dynamic arrays
#ifndef DA_H
#define DA_H

#ifndef DA_START_CAPACITY
#	define DA_START_CAPACITY 8
#endif // DA_START_CAPACITY

#ifndef DA_RESIZE_MULT
#	define DA_RESIZE_MULT 2
#endif // DA_RESIZE_MULT

#define DA_HEADER(type)\
typedef struct da_##type {\
	size_t len;\
	size_t capacity;\
	type *data;\
} da_##type;\
\
da_##type da_##type##_new();\
void da_##type##_free(da_##type *da);\
void da_##type##_resize(da_##type *da);\
void da_##type##_append(da_##type *da, type v);

#define DA_IMPLEMENTATION(type)\
da_##type da_##type##_new()\
{\
	errno = 0;\
\
	type *data = (type*)malloc(sizeof(type) * DA_START_CAPACITY);\
	if (data == NULL)\
	{\
		errno = 1;\
		return (da_##type){\
			.data = NULL,\
			.len = 0,\
			.capacity = 0\
		};\
	}\
\
	return (da_##type){\
		.data = data,\
		.len = 0,\
		.capacity = 8\
	};\
}\
\
void da_##type##_free(da_##type *da)\
{\
	free(da->data);\
	da->data = NULL;\
	da->len = 0;\
	da->capacity = 0;\
}\
\
void da_##type##_resize(da_##type *da)\
{\
	errno = 0;\
	if (da->capacity > da->len) return;\
	if (da->capacity == 0)\
	{\
		errno = 2;\
		return;\
	}\
\
	type *d = (type*)realloc(da->data, sizeof(type) * da->capacity * DA_RESIZE_MULT);\
	if (d == NULL)\
	{\
		errno = 1;\
		return;\
	}\
\
	da->data = d;\
	da->capacity *= 2;\
}\
\
void da_##type##_append(da_##type *da, type v)\
{\
	while (da->capacity <= da->len) {\
		da_##type##_resize(da);\
		if (errno == 2)\
		{\
			fprintf(stderr, "attempt to write to freed dynamic array\n");\
			exit(1);\
		} else if (errno != 0)\
		{\
			return;\
		}\
	}\
\
	da->data[da->len] = v;\
	da->len += 1;\
}

#endif // DA_H



// regex implementation
#ifndef IRREGEX_H
#define IRREGEX_H

#include <stddef.h>

// token types
typedef enum irx_token_type {
	IRX_TT_LITERAL,
} irx_token_type;

// a literal token value
typedef struct irx_token_value_literal {
	size_t start;
	size_t len;
} irx_token_value_literal;

// token value
typedef union irx_token_value {
	irx_token_value_literal lit;
} irx_token_value;

// the final token type
typedef struct irx_token {
	irx_token_type type;
	irx_token_value value;
} irx_token;

// the node types
typedef enum irx_node_type {
	IRX_NODE_LITERAL
} irx_node_type;

// values
typedef struct irx_node_value_literal {
	size_t start;
	size_t end;
} irx_node_value_literal;
typedef union irx_node_value {
	irx_node_value_literal lit;
} irx_node_value;

// create the node
typedef struct irx_node {
	irx_node_type type;
	irx_node_value value;
} irx_node;

// create the dynamic array headers
DA_HEADER(irx_token)
DA_HEADER(irx_node)

// tokenize the pattern
da_irx_token irx_tokenize(const char *pattern);

// parse the pattern into logic
da_irx_node irx_parse(const da_irx_token *tokens);

#endif // IRREGEX_H

#ifdef IRREGEX_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// implement the dynamic arrays
DA_IMPLEMENTATION(irx_token)
DA_IMPLDA_IMPLEMENTATION(irx_node)

// tokenize a literal
irx_token irx_tokenize_literal(const char *pattern, size_t len, size_t *pos)
{

	// looping values
	size_t start = *pos;
	size_t pattern_len = 0;
	
	// the final token value
	irx_token_value val;
	
	// for every character, get it to the string
	while (*pos < len)
	{
		char c = pattern[*pos];
		switch (c)
		{
			case '?':
				goto done;
			default:
				pattern_len += 1;
		}
		*pos += 1;
	}
	
	// you are done
done:
	// create the value literal
	val.lit = (irx_token_value_literal){
		.start = start,
		.len = pattern_len
	};
	
	// return the literal token
	return (irx_token){
		.type = IRX_TT_LITERAL,
		.value = val
	};
}

// tokenize the pattern
da_irx_token irx_tokenize(const char *pattern)
{

	// create the dynamic array
	da_irx_token da = da_irx_token_new();

	// looping vars
	size_t len = strlen(pattern);
	size_t i = 0;
	
	// loop through every single character
	while (i < len)
	{
	
		// detect the correct tokenization function
		char c = pattern[i];
		irx_token tok;
		switch (c)
		{
			case '?':
				break;
			default:
				tok = irx_tokenize_literal(pattern, len, &i);
				da_irx_token_append(&da, tok);
		}
	
		i++;
	}

	return da;
}

// parse text
da_irx_node irx_parse(const da_irx_token *tokens)
{
	// TODO (create this)
}

#endif // IRREGEX_IMPLEMENTATION