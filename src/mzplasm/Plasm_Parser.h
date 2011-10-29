#ifndef _PLASM_PARSER_H__

#include <scheme.h>

typedef Scheme_Object* PScheme_Object;

#define YYSTYPE PScheme_Object

int Str_2_Token (const char* str);

char* Token_2_String (int token);

Scheme_Object* Plasm_Lex_Init (int argc, Scheme_Object **argv);

Scheme_Object* Plasm_Parser (int argc,Scheme_Object **argv);

Scheme_Object* Plasm_Parser_Debug (int argc,Scheme_Object **argv);

extern YYSTYPE Parsed_Value;

#if (defined (__MWERKS__) || defined (_WINDOWS))
int strncasecmp (const char *S1, const char *S2, size_t N);
#endif      


#endif //_PLASM_PARSER_H__

