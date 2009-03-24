// File:    Token.cpp
// Author:  S. A. Blythe
// Date:    1/19/09
// Purpose: Implements the Token class. Primarilly, the Get(istream&) member.
#include "Token.h"

#include <fstream>

#include <ctype.h>

// there are 20 states (0..19) in the specified automata
#define MAXSTATES 20

// keeps current line number; static ensures scoped to this file only 
static int __linenum=1;

// strings corresponding to token types for output
char *TokStr[]=
{ "ERROR", "EOF_TOK", "NUM_INT", "NUM_REAL", "ADDOP", "MULOP", "ID", "RELOP", 
  "ASSIGNOP", "LPAREN", "RPAREN",  "SEMICOLON",  "LBRACK", "RBRACK", "COMMA",
  "AND", "OR", "VOID", "INTEGER", "FLOAT", "BEGIN", "END", "WHILE", 
  "IF", "THEN", "ELSE"};


// strings cprresponding to the keywords. 
static char *reserved[]={ "void", "int", "float", "begin", "end", "while", 
			  "if", "then", "else" };

// given a state number, plug into the array (map), and you get the
// corresponding token type.
static TokenType state2type[]={ERROR, ID, NUM_INT, ERROR, NUM_REAL, 
			       ADDOP, MULOP, RELOP, RELOP, ASSIGNOP, 
			       LPAREN, RPAREN, ERROR, AND, ERROR, OR, 
			       SEMICOLON, LBRACK, RBRACK, COMMA};

// function to strip out white space in front of next token. Modifies
// the provided stream accordingly and returns 1 if whitespace was
// skipped, 0 otherwise
int skip_ws(istream & is)
{
  char ch;
  int flag=0;

  // isspace gets all possible whitespace
  while( isspace(ch=is.get()) )
    {
      // remember we got whitespace
      flag=1;

      // if it is a newline, we'd be on the next line
      if (ch=='\n')
	__linenum++;
    }

  // sneakily, the last get "ate up" the first non-WS character. The
  // token will want that back!
  is.unget();

  return flag;
}

// function to strip out (a sequence of) comments in front of next
// token. Modifies the provided stream accordingly and returns 1 if at
// least one comment was skipped, 0 otherwise
int skip_comm(istream &is)
{
  char ch;

  ch = is.get();

  // does the next character indicate the start of another comment?
  if (ch != '#')
    {
      // if not ... we need to give that character back; it could well
      // be the beginning of the next token
      is.unget();
      return 0;
    }

  // ... but if it is a comment, just keep going to the end of line. 
  while ( (ch=is.get())!='\n' );

  // since the comment ends at the end of a line, the line number goes up!
  __linenum++;

  return 1;
}

// dumps a token (including type) in the expected fromat. 
ostream&
Token::print(ostream& os) const
{
  os << "Type:" << TokStr[_type] 
     << "\t Value:" << _value
     << "\t Line Number:" << _line_num;
  return os;
}


// Gets the next token in the input stream (is)
void
Token::Get(istream &is)
{
  // remember if we have built the table for lexical analysis yet ...
  static short table_built=0;

  // the lexical analysis table; static makes sure it remains even
  // after the current call completes.
  static int DFA[MAXSTATES][256];

  // if we have not built the table yet, do so ...
  if (!table_built)
    {
      int snum; // "iterator" for state number
      int inp;  // "iterator" for input characters.

      // default for all states is ERROR (-1)
      for (snum=0; snum<MAXSTATES; snum++)
	for(inp=0; inp<256; inp++)
	  DFA[snum][inp]=-1;

      // transitions into state 1
      for(inp='A'; inp<='Z'; inp++)
	DFA[0][inp]=DFA[1][inp]=1;
      for(inp='a'; inp<='z'; inp++)
	DFA[0][inp]=DFA[1][inp]=1;
      for(inp='0'; inp<='9'; inp++)
        DFA[1][inp]=1;

      // transitions into state 2
      for(inp='0'; inp<='9'; inp++)
	DFA[0][inp]=DFA[2][inp]=2;


      // transitions into state 3
      DFA[2][(int) '.']=3;
      

      // transitions into state 4
      for(inp='0'; inp<='9'; inp++)
	DFA[3][inp]=DFA[4][inp]=4;


      /* transitions into state 5 */
      DFA[0][(int)'+']=DFA[0][(int)'-']=5;

      /* transitions into state 6 */
      DFA[0][(int)'*']=DFA[0][(int)'/']=6;

      /* transitions into state 7 */
      DFA[0][(int)'<']=DFA[0][(int)'>']=7;

      /* transitions into state 8 */
      DFA[7][(int)'=']=DFA[9][(int)'=']=8;

      /* transitions into state 9 */
      DFA[0][(int)'=']=9;

      /* transitions into state 10 */
      DFA[0][(int)'(']=10;

      /* transitions into state 11 */
      DFA[0][(int)')']=11;
      
      /* transitions into state 12 */
      DFA[0][(int)'&']=12;
 
      /* transitions into state 13 */
      DFA[12][(int)'&']=13;

      /* transitions into state 14 */
      DFA[0][(int)'|']=14;

      /* transitions into state 15 */
      DFA[14][(int)'|']=15;

      /* transitions into state 16 */
      DFA[0][(int)';']=16;

      /* transitions into state 17 */
      DFA[0][(int)'[']=17;

      /* transitions into state 18 */
      DFA[0][(int)']']=18;

      /* transitions into state 19 */
      DFA[0][(int)',']=19;

      // nopw that we've built the tabel, make sure the next call
      // doesn't waste time rebuilding it!
      table_built=1;
    }

  // if the string for tiken value alread has storage, delete it; it
  // might not be enough for this token, or it might be too much for
  // this token.
  if (_value) 
    delete _value;

  // certainly won't need token of more than 75 chars (76 covers
  // terminating '\0')
  _value=new char[76];

  // keep skipping whitepace and/or comments as long as you can
  while(skip_comm(is) || (skip_ws(is)) ); /* note "Tricky" semicolon here */

  // if we are at the end of the stream, return EOF. 
  if (!is)
    {
      _type=EOF_TOK;
      return;
    }

  int prev_state, // state we were in before current iteration
    curr_state;   // current state
  char ch;        // next character in input

  prev_state=curr_state=0; // before we start, we are in state 0

  int index=0; // position within value string we are currently at

  // as long as the next state is not an error state ...
  while(curr_state!=-1)
    {
      // get next character
      ch=is.get();

      // put it in the token (may be removed later)
      _value[index++]=ch;

      // back up current (valid) state
      prev_state=curr_state;

      // get the next state
      curr_state=DFA[curr_state][(int)ch];
    }

  // getting here means that the current state is INVALID as a result
  // of reading the next character. So we need to: 
  //
  // (1) remove the character from the token value string ...
  _value[--index]=0;

  // ... and (2) place it back in the input stream (as it could be the
  // beginning of something else
  is.unget();

  // update the token's line number
  _line_num=__linenum;

  // get the type of token, based on the state ...
  _type=state2type[prev_state];
  
  // ... one catch; if it is an ID, it could really be a reserved word
  if (_type==ID)
    // go through all the reserverd words (see reserved[] above)
    for (int tt=0; tt<=ELSE-VOID; tt++)
      {
	// if it matches a reserved word, then that is its type!
	if (!strcmp(reserved[tt], _value) )
	  {
	    _type=(TokenType) (tt+VOID);
	    break;
	  }
      }
}









