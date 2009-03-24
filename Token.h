#ifndef _TOKEN_H_
#define _TOKEN_H_

using namespace std;

#include <iostream>
#include <string>

#include <string.h>

#define MAX_STR_LEN 26

enum TokenType { ERROR, EOF_TOK, NUM_INT, NUM_REAL, ADDOP, MULOP, ID, RELOP, ASSIGNOP, LPAREN, RPAREN, SEMICOLON, LBRACK, RBRACK, COMMA, AND, OR, VOID, INTEGER, FLOAT, BEGIN, END, WHILE, IF, THEN, ELSE};

extern char *TokStr[];
// { "ERROR", "EOF_TOK", "STRING", "NUM_INT", "NUM_REAL", "ADDOP", "MULOP", "ID" ... } 

class Token
{
private:
  char *_value;
  TokenType _type;
  unsigned int _line_num;
public:
  Token():_value(NULL), _type(ERROR), _line_num(1) {}
  
    // accessors/modifiers
  TokenType Type() const {return _type;}
  TokenType& Type() {return _type;}

  char* Value() const {return _value;}
  char* &Value() {return _value;}

  unsigned int LineNumber() const {return _line_num;}
  unsigned int &LineNumber() {return _line_num;}

  //*********************************************************
  // gets the next token ... YOU MUST WRITE THIS!!
  //*********************************************************
  void Get(istream &is);

  // dump out this token
  ostream & print(ostream &os) const; 

  // some basic comparison operators. 
  short int operator==(const TokenType tt) {return _type==tt;}
  short int operator!=(const TokenType tt) {return !(*this==tt);}

  short int operator==(const char *s) {return !strcmp(_value, s);}
  short int operator!=(const char *s) {return !(*this==s);}
};

// overload << for Tokens. 
inline ostream &operator<<(ostream &os, const Token &t)
  { return t.print(os); }

#endif
