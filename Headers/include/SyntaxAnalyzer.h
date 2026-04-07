#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
class SyntaxAnalyzer {
private:
    vector<string> lexemes;
    vector<string> tokens;
    vector<string>::iterator lexitr;
    vector<string>::iterator tokitr;

    map<string, string> symboltable;

    bool vdec();
    bool vars();
    bool type();
    bool stmtlist();
    bool stmt();
    bool ifstmt();
    bool elsepart();
    bool whilestmt();
    bool assignstmt();
    bool inputstmt();
    bool outputstmt();
    bool logexpr();
    bool relexpr();
    bool arithexpr();
    bool numterm();
    bool strterm();
    bool logicop();
    bool arithop();
    bool relop();

public:
    // pre:  infile is an open file containing valid scanner/lexer output
    //       in the form:  token lexeme  (one pair per line)
    // post: tokens and lexemes vectors have been populated
    SyntaxAnalyzer(istream& infile);

    // pre:  none
    // post: lexemes/tokens have been parsed.
    //       Prints error messages for any bad token/lexeme pair found.
    //       Returns true on success, false on failure.
    bool parse();
};
#endif