%{
/* Elk Plasm: Plasm_Parser.y
   File sorgente per yacc per costruire l'albero sintattico espresso in scheme in maniera simile a quella del plasm originale. Se si vuole in maniera copia-conforme vedi altro file.
*/
#ifdef WIN32
#pragma warning (disable: 4013 4018 4090)
#endif

#include <scheme.h>
#include "Plasm_Parser.h"
#include "Plasm_Lex.h"
#include "c2scheme.h"


static void print_token_value (FILE *, int, YYSTYPE);
#define YYPRINT(file, type, value) {} //print_token_value (file, type, value)

  

/* nell'include Plasm_Parser.h */ 
/*#define YYSTYPE Object*/
  %}

/* costruisci tabella token */
%token_table

/* token used by lex function if a string is matched but no token for it   */
/* it indicates  a SERIOUS incosistency between the lex and parser         */
/*    (ie: some lexers match a string and then searches in the parser's    */
/*     yytname the corisponding litteral token)                            */
%token UNDEFINED

/* identifier, separated from LEX_VALUE for syntax checking on definisions */
%token TOKENID

/* Value Given by Lex: 
         numbers, strings special operators such as S|@{numbers} */
%token LEX_VALUE

 

/* Keywords */
/*%token DEF "DEF"
 %token WHERE "WHERE"
 %token DEFAULT "DEFAULT"
%token END "END"*/


/* Token con sinonimi di stringa */
/* %token "**" POWER 
   %token "::" TYPESEP */

/*
%token RAISE "^'"
%token EQ "=="
%token LHEADTAILPRED "|->"
%token RHEADTAILPRED "<-|"
%token LHEADTAILPAT  "|=>"
%token RHEADTAILPAT  "<=|"
%token LIFTOR "\/" 
%token LIFTAND "/\"
*/



/* fake token for other infix operations */
/* resolves the conficts brought by infix rules */
%nonassoc SINGLE_OP



/*  '>' ']' ')' */
%nonassoc '(' '[' '<'  TOKENID LEX_VALUE


%left INFIXTOKEN 

   
/* %left '='   */ /* assegnazione , serve o implicito? */

/* %left LAMBDA */       /* fake token for lambda */
/* %right CONDITIONAL */ /* fake token p -> f § g serve esprimere priorita e right assoc? (FL)*/

/* left "/\" "\/"    */ /* FL Raised stessa priorita?OR & AND */
/* left "|=>" "<=|!  */ /* FL tail list cond or pat */
/* left "|->" "<-|"  */ /* FL tail list cond or pat */
/* left "=="    */ /*Equality (FL =) */

/* pol complex operators, priorita'? */
/* ".." priorita'? */
/*%left "&&" '&' '|' "||" "XOR" ".." */

%left '+' '-'  /* Add & Subtract */
%left '*' '/'  /* Mul & div */

/*%right UMINUS */  /* fake token for - precedence, in genere e right assoc ?! */

%left "**"     /* Power , in genere e right assoc ?! */
%left '~'      /* Composition (FL °,@ )*/
%left "::"     /* separatore di tipi */
%left ':'      /* Application */
%left '.'

/* move */
%nonassoc "MOVE"


/*%left '^'   */ /* Constant, not present in Plasm, in FL ~ */
/* %left "`^"   */ /* Raise , not present neither in Plasm nor FL */
/* %left '`'    */ /* Prime==Lift, not present in Plasm, in FL \' */


%%

input: 
  /* empty */ 
/*{ 
  $$=Scheme_SR_Table_0 (yyn); 
  Parsed_Value=$$; 
}*/
 script 
{
  $$=Scheme_SR_Table_1 (yyn,$1);
  Parsed_Value=$$;
}
| script script_elem
{
  $$=Scheme_SR_Table_2 (yyn,$1,$2);
  Parsed_Value=$$;
}
| script  error
{
  $$=Scheme_SR_Table_2 (yyn,$1,$2);
  Parsed_Value=$$;
};

script:
/* empty */ 
{
  $$=Scheme_SR_Table_0 (yyn);
}
|
 script script_elem ';'
{
  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);
}
| script error ';'
{
  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);
};


script_elem: 
  expr 
{
  $$=Scheme_SR_Table_1 (yyn,$1);
}
| defin
{ 
  $$=Scheme_SR_Table_1 (yyn,$1);
};


/* an experssion is either a value or an application */
/* a value is : 
      something received from the lexer: num, strings, names, etc ...
      a sequence
      a parenesized expression
*/
expr:
  TOKENID 
{
  $$=Scheme_SR_Table_1 (yyn,$1);
}
| LEX_VALUE
{

  $$=Scheme_SR_Table_1 (yyn,$1);
}
/*| '.' %prec SINGLE_OP
  {
   $$=Scheme_SR_Table_1 (yyn,$1);
}*/
| '~' %prec SINGLE_OP
{
  $$=Scheme_SR_Table_1 (yyn,$1);
 
}
| "**" %prec  SINGLE_OP
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| '*' %prec SINGLE_OP
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| '/' %prec SINGLE_OP
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| '+' %prec SINGLE_OP
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| '-' %prec SINGLE_OP
{

   $$=Scheme_SR_Table_1 (yyn,$1);

}
| seq
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| '(' expr ')' 
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr '.' expr
{
  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);
}
/*| appl
{
  $$=Scheme_SR_Table_1 (yyn,$1);
 };*/

/*appl: */
| expr ':' expr
{
  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);
}
| expr '~' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| '[' exprseq ']' 
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr "**" expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr '*' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr '/' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr '+' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| expr '-' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
/* crea grossi problemi: -~[...] -> parse error; -*ID ->parse error */
/*| '-' expr %prec UMINUS
{

  $$=Scheme_SR_Table_2 (yyn,$1,$2);

}*/
| expr expr expr  %prec INFIXTOKEN  
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
/* finto per la move di espressioni */
| "MOVE" ':' expr %prec "MOVE"
{
  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);
};



seq: 
'<' exprseq '>'
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

}
| '<' '>'
{

  $$=Scheme_SR_Table_2 (yyn,$1,$2);

};

exprseq: 
  expr
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| exprseq ',' expr
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};


defin: 
"DEF" TOKENID params '=' expr locals defaults
{

  $$=Scheme_SR_Table_7(yyn,$1,$2,$3,$4,$5,$6,$7);

}
| "CLASS"  TOKENID params isa '=' expr locals
{

  $$=Scheme_SR_Table_7(yyn,$1,$2,$3,$4,$5,$6,$7);

}
| "FEATURE" TOKENID params on '=' expr locals
{
  $$=Scheme_SR_Table_7(yyn,$1,$2,$3,$4,$5,$6,$7);
}
| "CONTROL" TOKENID params on '=' expr locals
{
  $$=Scheme_SR_Table_7(yyn,$1,$2,$3,$4,$5,$6,$7);
};

on:
"ON" TOKENID
{
  $$=Scheme_SR_Table_2(yyn,$1,$2);
}

isa:
/* empty */
{

  $$=Scheme_SR_Table_0 (yyn);

}
| "ISA" TOKENID
{
  $$=Scheme_SR_Table_2(yyn,$1,$2);
}

params:
/* empty */
{

  $$=Scheme_SR_Table_0 (yyn);

}
| params '(' param_list ')'
{

  $$=Scheme_SR_Table_4 (yyn,$1,$2,$3,$4);

};




param_list:
  param 
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| param_list ';' param
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};

param:
  idents "::" type
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};


type:
  expr
{

  $$=Scheme_SR_Table_1 (yyn,$1);

};

idents:
  TOKENID
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| idents ',' TOKENID
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};


locals:
/*empty */
{

  $$=Scheme_SR_Table_0 (yyn);

}
| "WHERE" loc_def_list "END"
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};

defaults:
/*empty */
{

  $$=Scheme_SR_Table_0 (yyn);

}
| "DEFAULT" loc_def_list "END"
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};


loc_def_list:
  loc_def
{

  $$=Scheme_SR_Table_1 (yyn,$1);

}
| loc_def_list ',' loc_def 
{

  $$=Scheme_SR_Table_3 (yyn,$1,$2,$3);

};

loc_def:
TOKENID params '=' expr locals
{

  $$=Scheme_SR_Table_5 (yyn,$1,$2,$3,$4,$5);

};


%%

#if (defined (__MWERKS__) || defined (WIN32))
int strncasecmp (const char *S1, const char *S2, size_t N)
{
  int res=0;
  int i=0;
  for (i=0;i<N;i++)
    {
      res=toupper(S2[i])-toupper(S1[i]);
      if (res) return res;
    }
  return 0;     
}
#endif   



int Str_2_Token (const char* str)
{
  int i;
  if (strlen (str)==1) return (int) str[0];
  for (i = 0; i < YYNTOKENS; i++)
    {
      /*printf ("yytnamed[%d]: %s\n",i,yytname[i]);*/
      if (yytname[i] != 0
	   && yytname[i][0] == '"'
	   && ! (strncasecmp (yytname[i] + 1, str,
			      strlen (str)))
	   && ( ( yytname[i][strlen (str) + 1] == '"' 
		  && yytname[i][strlen (str) + 2] == 0) 
		||
		( yytname[i][strlen (str) + 1] == '\\' 
		  && yytname[i][strlen (str) + 2] == '"' 
		  && yytname[i][strlen (str) + 3] == 0 ))) 
	break;  
    }
   /*printf ("%s -> yytnamed[%d]: %s\n",str,i,yytname[i]); */
  if (i<YYNTOKENS) return yytoknum[i]; 
   /* this shouldn't happend */
  else return UNDEFINED;
  
}

char* Token_2_String (int token)
{
  int i;
  for (i = 0; i<YYNTOKENS; i++)
    if (token==yytoknum[i]) break;
  if (i<YYNTOKENS) 
    return yytname[i];
  /* this shouldn't happend */
  else return "UNDEFINED";
}


yyerror (s)
          char *s;
     {
       //Plasm_Lex_Reset();
       Scheme_Str_Call("Syntax_Error",s);
       #ifdef WIN32
       return 0;
       #endif
     }

/* Printer per debugging, necessario per avere le yytoknum */
static void print_token_value (FILE *file, int type, YYSTYPE value) {
  //char *tmpCStr;
  //Scheme_Object* PrintVal;
  //Scheme_Object *PrintFunc;
  //fprintf(file,"\nYYPRINT: Token = %s\n",Token_2_String(type));
  //tmpCStr=scheme_strdup(SCHEME_BYTE_STR_VAL(scheme_char_string_to_byte_string(value)));

  //fprintf(file,"\n         Value = %s\n",tmpCStr);
         
}

YYSTYPE Parsed_Value;

