%{
#include "builder/generic_builder.h"
#include "misc/scalar.h"
#include "misc/exception.h"
#include <stdio.h>

#define YYDEBUG 1

// Builder will be passed to yyparse as argument
#define YYPARSE_PARAM param
#define builder (static_cast<Builders::GenericBuilder*>(param))

using Builders::pbuilding_obj;

int trp_yylex(void);
void trp_yyerror(char *);

FILE* trp_yyinsave = (FILE*) 0;
extern FILE* trp_yyin;
bool  orderFileGiven = false;

pbuilding_obj trp_parse_result;

%}

%union {
	pbuilding_obj object;
        char* string;
}

%token <string> TOK_IDENT
%token TOK_LPAR
%token TOK_RPAR
%token TOK_LBRACKET
%token TOK_RBRACKET
%token TOK_NOT
%token TOK_CONJ
%token TOK_DISJ
%token TOK_COMMA
%token TOK_DOT
%token TOK_ALWAYS
%token TOK_SOMETIME
%token TOK_NEXT
%token TOK_ORDER
%type <object> formula
%type <object> atom
%type <object> list_of_snf_clauses
%type <object> snf_clause
%type <object> temporal_clause
%type <object> initial_clause
%type <object> universal_clause
%type <object> step_clause
%type <object> sometime_clause
%type <object> literal_list
%type <object> next_literal_list
%type <object> literal


%left TOK_DISJ
%left TOK_CONJ
%left TOK_NOT 

%%


start:  order formula
           {
               trp_parse_result = $2;
           }

order: /*no order given*/
         {
         }
     | TOK_ORDER TOK_LPAR literal_list TOK_RPAR TOK_DOT
         {
         }
     ;

formula: TOK_CONJ TOK_LPAR TOK_LBRACKET list_of_snf_clauses TOK_RBRACKET TOK_RPAR TOK_DOT
           {
	       if(orderFileGiven)
	       {
	          throw("Order file may not contain anything but \"order\" sentance\n");
	       }
	       $$ = $4;
	   }
       ;

list_of_snf_clauses:  /* empty list */
                       {
		           $$ = builder->create_clause_list();
		       }
		   | snf_clause
		       {
		           $$ = builder->create_clause_list();
			   $$ = builder->add_clause_to_list($$, $1);
		       }
                   | list_of_snf_clauses TOK_COMMA snf_clause
		       {
 		           $$ = builder->add_clause_to_list($1, $3);
		       }
                   ;

snf_clause: TOK_ALWAYS TOK_LPAR temporal_clause TOK_RPAR
	      {
	          $$ = $3;
	      }
          | initial_clause
	      {
	          $$ = $1;
	      }
          ;

temporal_clause: universal_clause
                   {
		       $$ = $1;
		   }
               | step_clause
                   {
		       $$ = $1;
		   }
	       | sometime_clause
                   {
		       $$ = $1;
		   }
	       ;

initial_clause: TOK_DISJ TOK_LPAR TOK_LBRACKET literal_list TOK_RBRACKET TOK_RPAR
                  {
		      $$ = builder->build_initial($4);
		  }
              ;

universal_clause: TOK_DISJ TOK_LPAR TOK_LBRACKET literal_list TOK_RBRACKET TOK_RPAR
                    {
		        $$ = builder->build_universal($4);
		    }
                ;

step_clause: TOK_DISJ TOK_LPAR TOK_LBRACKET literal_list next_literal_list TOK_RBRACKET TOK_RPAR 
               {
	           $$ = builder->build_step($4, $5);
	       }
           ;

sometime_clause: TOK_DISJ TOK_LPAR TOK_LBRACKET literal_list TOK_SOMETIME TOK_LPAR literal TOK_RPAR TOK_RBRACKET TOK_RPAR
                   {
		       $$ = builder->build_sometime($4, $7);
		   }
               ;

literal_list: 
                {
		    $$ = builder->create_literal_list();
		}
            | literal
	        {
		    $$ = builder->create_literal_list();
		    $$ = builder->add_literal_to_list($$, $1);
		}
            | literal TOK_COMMA literal_list
	        {
		    $$ = $3;
		    $$ = builder->add_literal_to_list($$, $1);
		}
	    ;

next_literal_list: TOK_NEXT TOK_LPAR literal TOK_RPAR  /* must not be empty */
                     {
		         $$ = builder->create_literal_list();
			 $$ = builder->add_literal_to_list($$, $3);
		     }
                 | TOK_NEXT TOK_LPAR literal TOK_RPAR TOK_COMMA next_literal_list
		     {
		         $$ = $6;
			 $$ = builder->add_literal_to_list($$, $3);
		     }
		 ;

literal: atom
           {
	       $$ = builder->build_literal($1);
	   }
       | TOK_NOT TOK_LPAR atom TOK_RPAR
           {
	       $$ = builder->build_literal($3);
	       $$ = builder->negate($$);
	   }
       | TOK_NOT  atom
           {
	       $$ = builder->build_literal($2);
	       $$ = builder->negate($$);
	   }
       ;

atom: TOK_IDENT
        {
            $$ = builder->build_atom($1);
   	    free($1); // seems that I do not create any other object 
	                 // self-deleting
	}
    ;

%%

extern int trp_errlineno;
extern int trp_yylineno;

extern "C" {
    int
    trp_yywrap( void )
    {
        if(orderFileGiven)
	{
	    trp_yyin = trp_yyinsave;
	    orderFileGiven = false;
	    return 0;
	}
	else
	{
  	    return 1;
   	}
    }
}

void
trp_yyerror(char *s)
{
    if (trp_errlineno == -1)
        trp_errlineno = trp_yylineno;
    std::cerr << "line " << trp_errlineno << ": " << s << std::endl;;
}
