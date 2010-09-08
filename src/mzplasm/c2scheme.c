#include <scheme.h>

#include "c2scheme.h"

#include "Plasm_Parser.h"
#include "Plasm_Parser.tab.h"

/* memorize the called environment */
Scheme_Env *The_Scheme_Env;
Scheme_Object **Plasm_Syntax_Table;

#ifdef YYDEBUG 
extern int yydebug;
extern int yy_flex_debug;
#endif

int yyparse();
 


 
Scheme_Object *Plasm_Parser (int argc,Scheme_Object **argv)
{
	Scheme_Object *tmp;
	
	#ifdef YYDEBUG 
	yydebug=0;
	yy_flex_debug=0;
	if(yydebug || yy_flex_debug) printf("Parser Debugging Activated\n");
	#endif
	
	if (argc==0) 
	{
		if(yyparse()) 
			return scheme_null;
		else 
			return Parsed_Value;
	}
	else if (argc==1 || argc==2 || argc==3) 
	{
		tmp=Plasm_Lex_Init(argc,argv);

		if (tmp==scheme_true)
		{
			if(yyparse()) 
				return scheme_null;
			else 
				return Parsed_Value;
		}
		else 
			return scheme_null;
	}
	
	else 
		return scheme_null;
}



Scheme_Object *Plasm_Parser_Debug (int argc,Scheme_Object **argv)
{
#ifdef YYDEBUG 
	if (argv[0]==scheme_true) 
	{
		yydebug=1;
		return scheme_true;
	}
	else
	{
		yydebug=0;
		return scheme_false;
	}
#endif
	return scheme_false;
}






Scheme_Object *scheme_Plasm_initialize(Scheme_Env *env)
{
	The_Scheme_Env=env;
	
	Plasm_Syntax_Table=0;
	
	/* Rendi disponibili nell'ambiente Scheme le seguenti funzioni : */
	scheme_add_global("Plasm_Parser",scheme_make_prim_w_arity(Plasm_Parser,"Plasm_Parser",0, 3),env);
	scheme_add_global("Plasm_Lex_Init",scheme_make_prim_w_arity(Plasm_Lex_Init,"Plasm_Lex_Init",0, 3),env);
	scheme_add_global("Plasm_Parser_Debug",scheme_make_prim_w_arity(Plasm_Parser_Debug,"Plasm_Parser_Debug",1, 1),env);
	scheme_add_global("string->symbol-ci",scheme_make_prim_w_arity(string_to_symbol_ci,"string->symbol-ci",1, 1),env);
	
	return scheme_void;
}


Scheme_Object* Get_Plasm_Syntax_Table(int rule)
{
	
	if (!Plasm_Syntax_Table)
		Plasm_Syntax_Table=SCHEME_VEC_ELS(scheme_lookup_global(scheme_intern_symbol("Syntax-Rules"),The_Scheme_Env));
	return Plasm_Syntax_Table[rule];
}



/* Implementazione funzione conversione Case insensitive 
da Stringa Scheme -> Simbolo */ 
Scheme_Object* string_to_symbol_ci(int argc, Scheme_Object **argv)
{
	Scheme_Object* ret;
	ret=scheme_void; 

	if (SCHEME_BYTE_STRINGP(argv[0]))
		ret=scheme_intern_symbol(SCHEME_BYTE_STR_VAL(argv[0]));
	else if (SCHEME_CHAR_STRINGP(argv[0]))
		ret=scheme_intern_symbol(SCHEME_BYTE_STR_VAL(scheme_char_string_to_byte_string(argv[0])));


	return ret;
}

/* applica funzione scheme dal nome s_func (identif. Stringa C) 
alla stringa s_arg (conv. C->Schmeme */
Scheme_Object *Scheme_Str_Call (const char *s_func, const char *s_arg)
{
	Scheme_Object* ret;
	Scheme_Object *arg[1];
	Scheme_Object *func;

	func=scheme_lookup_global(scheme_intern_symbol(s_func),The_Scheme_Env);
	arg[0]=scheme_byte_string_to_char_string(scheme_make_byte_string(s_arg));
	ret=_scheme_apply(func,1,arg);
	return ret;
	
}

/* chiama la funzione numero rule nel vettore Syntax_Rules, 
a 1..n argomenti  */
Scheme_Object* Scheme_SR_Table_0 (int rule)
{
	Scheme_Object *arg[1];
	
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),0,arg);
}

Scheme_Object* Scheme_SR_Table_1 (int rule, Scheme_Object *arg1)
{
	Scheme_Object *arg[1];
	arg[0]=arg1;
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),1,arg);
}

Scheme_Object* Scheme_SR_Table_2 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2)
{
	Scheme_Object *arg[2];
	arg[0]=arg1;
	arg[1]=arg2;
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),2,arg);
}

Scheme_Object* Scheme_SR_Table_3 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2, 
											 Scheme_Object *arg3)
{
	Scheme_Object *arg[3];
	arg[0]=arg1;
	arg[1]=arg2;
	arg[2]=arg3;
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),3,arg);
}

Scheme_Object* Scheme_SR_Table_4 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2, 
											 Scheme_Object *arg3,
											 Scheme_Object *arg4)
{
	Scheme_Object *arg[4];
	arg[0]=arg1;
	arg[1]=arg2;
	arg[2]=arg3;
	arg[3]=arg4;
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),4,arg);
}

Scheme_Object* Scheme_SR_Table_5 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2, 
											 Scheme_Object *arg3,
											 Scheme_Object *arg4, 
											 Scheme_Object *arg5)
{
	Scheme_Object *arg[5];
	arg[0]=arg1;
	arg[1]=arg2;
	arg[2]=arg3;
	arg[3]=arg4;
	arg[4]=arg5;
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),5,arg);
}

Scheme_Object* Scheme_SR_Table_6 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2, 
											 Scheme_Object *arg3,
											 Scheme_Object *arg4, 
											 Scheme_Object *arg5, 
											 Scheme_Object *arg6)
{
	Scheme_Object *arg[6];
	arg[0]=arg1;
	arg[1]=arg2;
	arg[2]=arg3;
	arg[3]=arg4;
	arg[4]=arg5;
	arg[5]=arg6;  
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),6,arg);
}

Scheme_Object* Scheme_SR_Table_7 (int rule, Scheme_Object *arg1, 
											 Scheme_Object *arg2, 
											 Scheme_Object *arg3,
											 Scheme_Object *arg4, 
											 Scheme_Object *arg5, 
											 Scheme_Object *arg6, 
											 Scheme_Object *arg7)
{
	Scheme_Object *arg[7];
	arg[0]=arg1;
	arg[1]=arg2;
	arg[2]=arg3;
	arg[3]=arg4;
	arg[4]=arg5;
	arg[5]=arg6; 
	arg[6]=arg7; 
	return _scheme_apply(Get_Plasm_Syntax_Table(rule),7,arg);
}


