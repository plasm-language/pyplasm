/* -------------------------------------------------------------------------- *\
   Project..........:   eXtreme Geometric Environment for PLaSM
   Copyright........:   Universita' degli studi di ROMA TRE
                        Dipartimento di Informatica e Automazione 
                        Via Vasca Navale 84, 00146 ROMA
	Development by...:	Simone Portuesi
	Environment......:	Win32, UNIX
\* -------------------------------------------------------------------------- */


#ifndef _C_2_SCHEME_H__
#define _C_2_SCHEME_H__

#include <scheme.h>

Scheme_Object* string_to_symbol_ci(int argc, Scheme_Object **argv);

Scheme_Object* Scheme_Str_Call (const char *s_func, const char *s_arg);

Scheme_Object* Scheme_SR_Table_0 (int rule);
Scheme_Object* Scheme_SR_Table_1 (int rule, Scheme_Object *arg1);
Scheme_Object* Scheme_SR_Table_2 (int rule, Scheme_Object *arg1, Scheme_Object *arg2);
Scheme_Object* Scheme_SR_Table_3 (int rule, Scheme_Object *arg1, Scheme_Object *arg2, Scheme_Object *arg3);
Scheme_Object* Scheme_SR_Table_4 (int rule, Scheme_Object *arg1, Scheme_Object *arg2, Scheme_Object *arg3,Scheme_Object *arg4);
Scheme_Object* Scheme_SR_Table_5 (int rule, Scheme_Object *arg1, Scheme_Object *arg2, Scheme_Object *arg3,Scheme_Object *arg4, Scheme_Object *arg5);
Scheme_Object* Scheme_SR_Table_6 (int rule, Scheme_Object *arg1, Scheme_Object *arg2, Scheme_Object *arg3,Scheme_Object *arg4, Scheme_Object *arg5, Scheme_Object *arg6);
Scheme_Object* Scheme_SR_Table_7 (int rule, Scheme_Object *arg1, Scheme_Object *arg2, Scheme_Object *arg3,Scheme_Object *arg4, Scheme_Object *arg5, Scheme_Object *arg6, Scheme_Object *arg7);

#endif

