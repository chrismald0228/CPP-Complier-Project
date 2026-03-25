
#include "LexAnalyzer.h"
// pre: parameter refers to open data file consisting of token and
// lexeme pairs i.e.  t_and and
// Each pair appears on its own input line.
// post: tokenmap has been populated - key: lexeme, value: token

LexAnalyzer::LexAnalyzer(istream &infile)
{
    string token, lexeme;
    while(infile >> token >> lexeme) {
        tokenmap[lexeme] = token;
    }
}

// pre: 1st parameter refers to an open text file that contains source
// code in the language, 2nd parameter refers to an open empty output
// file
// post: If no error, the token and lexeme pairs for the given input
// file have been written to the output file (token : lexeme).
// If there is an error, the incomplete token/lexeme pairs, as well as
// an error message have been written to the output file.
// A success or fail message has printed to the console.
bool LexAnalyzer::isValidId(const string& lexeme) {
    if(!isalpha(lexeme[0]))
        return false;
    for(char ch : lexeme) {
        if(!isalnum(ch) && ch != '_')
            return false;
    }
    return true;
}
bool LexAnalyzer::isValidNumber(const string& lexeme) {
    for(char ch : lexeme) {
        if(!isdigit(ch))
            return false;
    }
    return true;
}
//searches through tokenmap and compares the symbols(s_ tokens) to the char passed in
bool LexAnalyzer::isSymbol(char ch) {
    for(pair<string, string> p: tokenmap) {
        if(p.second.rfind("s_", 0) == 0) {
            if(p.first[0] == ch) {
                return true;
            }
        }
    }
    return false;
}
void LexAnalyzer::pushToLexemes(string& lexeme) {
    if(tokenmap.count(lexeme)){
        lexemes.push_back(lexeme);
        tokens.push_back(tokenmap[lexeme]);
    } 
    else if(lexeme.front()=='"') {
        if(lexeme.back() != '"') {
            lexemes.push_back("error");
            tokens.push_back(lexeme + ": unclosed string");
        }
        else{
            //lexemes.push_back(lexeme.substr(1,lexeme.length() -2));
            lexemes.push_back(lexeme);
            tokens.push_back("t_text");
        }
    }
    else if(isValidId(lexeme)) {
        lexemes.push_back(lexeme);
        tokens.push_back("t_id");
    }
    else if(isValidNumber(lexeme)) {
        lexemes.push_back(lexeme);
        tokens.push_back("t_number");
    }
    else {
        lexemes.push_back("error");
        tokens.push_back(lexeme + ": unknown lexeme");
    }
}
//error for string delimiter is found during line parsing
//this function pushes the error to the lexeme/token parallel array
void LexAnalyzer::pushStringDelimiterError(string& errString) {
    lexemes.push_back("error");
    tokens.push_back(errString + ": string delimiter error");
}
void LexAnalyzer::writeToFile(ostream &outfile) {
    for(int i=0;i<lexemes.size();i++) {
        if(lexemes[i] == "error") {
            outfile << tokens[i] << " " << lexemes[i] << endl;
            cout << "fail" << endl;
            return;
        }
        outfile << tokens[i] << " " << lexemes[i] << endl;
    }
    cout << "success" << endl;
}
void LexAnalyzer::scanFile(istream &infile, ostream &outfile) {
    string currentLine;
    string current = "";
    bool inString = false;

    while (getline(infile, currentLine)) {
        for(int i=0;i<currentLine.length();i++){
            char ch = currentLine[i];
            
            if (ch == '"') {
                //first quote, add current lex to array and clear current
                if (!current.empty() && !inString) {
                    pushToLexemes(current);
                    current.clear();
                }
                //push " to current, needed for final checks
                current += ch;
                //inString, so the " we hit is closing
                if (inString) {
                    pushToLexemes(current);
                    current.clear();
                    inString = false;
                } else {
                    inString = true;
                }
            }
            else if (inString) {
                current += ch;
            }
            else if (isspace(ch)) {
                if (!current.empty()) {
                    pushToLexemes(current);
                    current.clear();
                }
            }
            else if (isSymbol(ch)) {
                if (!current.empty()) {
                    pushToLexemes(current);
                    current.clear();
                }

                string op(1, ch);

                char next = currentLine[i+1];
                string twoCharOp = op + next;
                if (tokenmap.count(twoCharOp)) {
                    i++;
                    op = twoCharOp;
                }
                pushToLexemes(op);
            }
            else {
                current += ch;
                //checks if the current char (letter/num) has a " to its right, which creates error
                if(currentLine[i+1] == '"') {
                    string errString = current + currentLine[i+1];
                    pushStringDelimiterError(errString);
                    current.clear();
                }
                //checks if the current char (letter/num) has a " to its left, which creates error
                if(i>0 && currentLine[i-1] == '"') {
                    string errString = current;
                    pushStringDelimiterError(errString);
                    current.clear();
                }
            }
        }
    }

    if (!current.empty()) {
        pushToLexemes(current);
    }
    writeToFile(outfile);
}
