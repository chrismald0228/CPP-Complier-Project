#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

class SyntaxAnalyzer{
private:
    vector<string> lexemes;
    vector<string> tokens;
    vector<string>::iterator lexitr;
    vector<string>::iterator tokitr;
    // map of variables and their datatype
    // i.e. sum t_integer
    map<string, string> symboltable; 

    // other private methods
    // these can be changed and added to
    bool vdec();
    // int vars();
    bool vars();
    bool stmtlist();
    // int stmt();
    bool stmt();
    bool ifstmt();
    bool elsepart();
    bool whilestmt();
    bool assignstmt();
    bool inputstmt();
    bool outputstmt();

    bool logexpr();
    bool arithexpr();
    bool relexpr();
    bool strterm();
    bool numterm();

    bool expr();
    bool simpleexpr();
    bool term();
    bool logicop();
    bool arithop();
    bool relop();

public:
    SyntaxAnalyzer(istream& infile);
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
    // post: the vectors have been populated

    bool parse();
    // pre: none
    // post: The lexemes/tokens have been parsed.
    // If an error occurs, a message prints indicating the token/lexeme pair
    // that caused the error.
    // If no error, vectors contain syntactically correct source code
	// PROG  -> VDEC main STMTLIST end 
};

#endif