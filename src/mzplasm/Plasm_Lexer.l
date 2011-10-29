

%{
  
#ifdef __MWERKS__
#include <cstdio>
#else
#include <stdio.h>
#endif   

#ifdef WIN32
#pragma warning (disable:4267 4101 4018 4013 4996 4244)
#endif
  
  /* Interfacciamento Scheme */
#include <scheme.h>
#include "c2scheme.h"  
  
  /* interfacciamento Analizzatore Sintattico */
  /* Plasm_Parser.h Plasm_Parser.tab.h 
   devono essere in questo ordine 
   per la definizione di YYSTYPE */
#include "Plasm_Parser.h"  
#include "Plasm_Parser.tab.h"
  
  /* azione da compiere per qualsiasi stringa */
#define YY_USER_ACTION Scheme_Str_Call("Syntax_User_Action",yytext);
  
struct YY_BUFFER_STATE_list
{
  YY_BUFFER_STATE buffer;
  struct YY_BUFFER_STATE_list* next;

};

struct YY_BUFFER_STATE_list* bufferStack=NULL;

int outer_text;


%}

/* non fornire il main del programma */
%option nomain


/* %option debug*/

%x OUTER_TEXT

/* non fornire la funzione di wrap */
/*%option noyywrap*/


DIGIT    [0-9] 
       
ALFA     [A-Z]        

SPECIAL [\?\!\^\_]
                                     
KEYWORD ("DEF"|"WHERE"|"DEFAULT"|"END"|"CLASS"|"ISA"|"FEATURE"|"CONTROL"|"ON"|"MOVE")

LITERAL_FUNC (\#|"##")

LITERAL_OP (".."|"&&"|\&|\||"||"|"XOR"|\$)

ADD_OPS (\+|\-)

MULT_OPS (\*|\/)

POWER_OP ("**")

COMP_OP (~)

TYPE_OP ("::")

APPL_OP (\:)

PARENTESIS (\(|\[|\<|\>|\]|\))

ASSIGN (\=)

LOCENV (\.)

SEPARATORS (\,|\;)


%%

   /* 
   Setting this global variable before calling yylex() will make lexer start in outer text mode 
   In this mode everithing is ignore until a line containing ---- is encountered.
   At this point lex will work normally until another line is encountered 
   */
   {
   //printf("In %s\n",outer_text?"outer text":"code");
   if (outer_text)  BEGIN(OUTER_TEXT);
   }
   /* Look for 
	  //blah
      ----------------- */
<OUTER_TEXT>^\/\/.*[\n\r\t]-+[\n\r\t] {
		//printf("Going to code\n");
		BEGIN(INITIAL);
		outer_text=0;
		}

   /* Ingore everything else */
<OUTER_TEXT>(.*)

<OUTER_TEXT>[ \n\r\t]+

^-+ {
		//printf("Going to outer\n");
		BEGIN(OUTER_TEXT);
		outer_text=1;
		}

  /* Spaces and other separators */ 
[ \n\r\t]+ /* ignore spaces */

  /* Comments */ 
\%[^\%]+\% /* ignore comments */


  /* Valori atomici */   
  /* Strings */     
\'([^\']|"''")+\' {
  yylval=Scheme_Str_Call("Syntax_string",yytext);
  return LEX_VALUE;} 

  /* Numbers */
(\-)?{DIGIT}+(\.{DIGIT}+)?(E(\+|\-)?{DIGIT}+)?  {
  yylval=Scheme_Str_Call("Syntax_number",yytext);
  return LEX_VALUE;} 


  /* Stringe che costituiscono unita' lessicali a se' stanti */
  /* Non so se separarli o tenerli uniti per classi di token */

  /* Keywords */
{KEYWORD}|{PARENTESIS}|{SEPARATORS}|{APPL_OP}|{ASSIGN}|{LOCENV} {
  return Str_2_Token(yytext);}
 
  /* Mappati in token identificatore generici */
{LITERAL_FUNC}|{LITERAL_OP} {
  yylval=Scheme_Str_Call("Syntax_identifier",yytext);
  return TOKENID;}

  /* Mappati in token letterali separati */
{ADD_OPS}|{MULT_OPS}|{POWER_OP}|{COMP_OP}|{TYPE_OP} {
  yylval=Scheme_Str_Call("Syntax_identifier",yytext);
  return Str_2_Token(yytext);}

  /* sono speciali, si deve ritornare l'equivalente di S:num e @:num*/
S{DIGIT}+ {
  yylval=Scheme_Str_Call("Syntax_selop",yytext);
  return LEX_VALUE;}
\@{DIGIT}+ {
  yylval=Scheme_Str_Call("Syntax_skelop",yytext);
  return LEX_VALUE;}


  /* Identificatori utente */
({ALFA}|{SPECIAL})(({ALFA}|{SPECIAL}|{DIGIT})+)?    {
  yylval=Scheme_Str_Call("Syntax_identifier",yytext);
  return TOKENID;}


  /* Fine File EOF */
<*><<EOF>> { 
  yylval=scheme_null;
  return '\0';}


  /* tutto il resto non fa' parte della grammatica */
. {return UNDEFINED;  }

%%

int yywrap()
{
  struct YY_BUFFER_STATE_list* tmpBufferStack;

  //printf("Wrap 1\n");
  //printf("Current bufferStack (%p) points to buffer %p\n",
  //	 bufferStack,
  // 	 bufferStack?bufferStack->buffer:NULL);
  //printf("YY_CURRENT_BUFFER=%p\n",YY_CURRENT_BUFFER);

  /* if stack is null then we are in serious trouble */
  if(bufferStack==NULL)
    yyterminate();

  /* delete current buffer */
  yy_delete_buffer(YY_CURRENT_BUFFER);
  
 
  //printf("Wrap 2\n");
  
  /* pop from stack */
  tmpBufferStack=bufferStack->next;
  free(bufferStack);
  bufferStack=tmpBufferStack;
  //printf("Current bufferStack (%p) points to buffer %p\n",
  //	 bufferStack,
  //	 bufferStack?bufferStack->buffer:NULL);
  
  /* if net top is null then lexing is finished */

  if (bufferStack!=NULL)
    {
      yy_switch_to_buffer(bufferStack->buffer);
      //printf("YY_CURRENT_BUFFER=%p\n",YY_CURRENT_BUFFER);
      return 0;
    }
  else return 1;
}

/* Funzione che realizza la funzione scheme di inizializzazione 
   dell'analizzatore, può essere chiamata con due parametri opzionali
   il primo è o una stringa o un file che si vuole analizzare
   il secondo è un flag per attivare la modalità interattiva */



Scheme_Object *Plasm_Lex_Init (int argc, Scheme_Object **argv)
{
  
  struct YY_BUFFER_STATE_list* tmpBufferStack;
  Scheme_Object *tmpStr;
  char *tmpCStr;

  if (argc>3) return(scheme_false);
  
  if (argc==0) 
  {
    yyin=stdin;
    return(scheme_true); 
  }
  //printf("Init 1\n");
  //printf("Current bufferStack (%p) points to buffer %p\n",
  //	 bufferStack,
  //	 bufferStack?bufferStack->buffer:NULL);
  //printf("YY_CURRENT_BUFFER=%p\n",YY_CURRENT_BUFFER);
  
  /*yy_delete_buffer(YY_CURRENT_BUFFER);*/

  BEGIN(0);
  
  if (SCHEME_CHAR_STRINGP(argv[0])) {
    tmpCStr=scheme_strdup(SCHEME_BYTE_STR_VAL(scheme_char_string_to_byte_string(argv[0])));
    if(yy_flex_debug) fprintf(stderr,"Buffer is a Scheme String:\n%s\n",tmpCStr);
    yy_scan_string(tmpCStr);
  } else if  (SCHEME_BYTE_STRINGP(argv[0])) {
    tmpCStr=scheme_strdup(SCHEME_BYTE_STR_VAL(argv[0]));
    if(yy_flex_debug) fprintf(stderr,"Buffer is a Scheme Byte Array:\n%s\n",tmpCStr);
    yy_scan_string(tmpCStr);
  } else if (SCHEME_INPORTP(argv[0])) {
    if(yy_flex_debug) fprintf(stderr,"Buffer is a Scheme Port %p\n",SCHEME_INPORT_VAL(argv[0]));
    yyin=SCHEME_INPORT_VAL(argv[0]);
  } else {
    return(scheme_false);
  }


  /*if (SCHEME_STRINGP(argv[0]))
     yy_scan_string(scheme_strdup(SCHEME_STR_VAL(argv[0])));
  else if (SCHEME_INPORTP(argv[0]))
    yyin=SCHEME_INPORT_VAL(argv[0]);
  else return(scheme_false);*/
  
  //printf("Init 2\n");
  //printf("Current bufferStack (%p) points to buffer %p\n",
  //	 bufferStack,
  //	 bufferStack?bufferStack->buffer:NULL);
  //printf("YY_CURRENT_BUFFER=%p\n",YY_CURRENT_BUFFER);
  tmpBufferStack=malloc(sizeof(struct YY_BUFFER_STATE_list));
  tmpBufferStack->buffer=YY_CURRENT_BUFFER;
  tmpBufferStack->next=bufferStack;
  bufferStack=tmpBufferStack;
  //printf("Init 3\n");
  //printf("Current bufferStack (%p) points to buffer %p\n",
  //	 bufferStack,
  //	 bufferStack?bufferStack->buffer:NULL);

  if (argc>1)
    {
       if (SCHEME_TRUEP(argv[1])) 
	 yy_set_interactive(1)
       else if (SCHEME_FALSEP(argv[1])) 
	 yy_set_interactive(0);
    }
 
  if (argc>2)
    {
       if (SCHEME_TRUEP(argv[2])) outer_text=1;
       else if (SCHEME_FALSEP(argv[2])) outer_text=0;
    }
  else outer_text=0;
 
  // To test lexer by itself
  //while(yylex());

  return(scheme_true);  
}

void Plasm_Lex_Reset()
{
  struct YY_BUFFER_STATE_list* tmpBufferStack;

  /* pop from stack */
  while(bufferStack)
    {
      yy_delete_buffer(YY_CURRENT_BUFFER);
      tmpBufferStack=bufferStack->next;
      free(bufferStack);
      bufferStack=tmpBufferStack;
      if(bufferStack)
          yy_switch_to_buffer(bufferStack->buffer);
    }
}
