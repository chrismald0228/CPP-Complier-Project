
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
bool LexAnalyzer::isValidSymbol(const char& ch) {
    for(pair<string, string> p: tokenmap) {
        if(p.second.rfind("s_", 0) == 0) {
            if(p.first[0] == ch) {
                return true;
            }
        }
    }
    return false;
}

void LexAnalyzer::pushToLexemes(string& lexeme, bool& spaceBefore, bool tEndValid) {
    bool tValid = tEndValid && (spaceBefore || (!tokens.empty() && tokens.back()[0] == 's'));
    if(tokenmap.count(lexeme)){
        if (tokenmap[lexeme][0] == 't') {
            if (tValid) {
                lexemes.push_back(lexeme);
                tokens.push_back(tokenmap[lexeme]);
            } else {
                lexemes.push_back("error");
                tokens.push_back(lexeme + ": invalid t_ lexeme");
            }
        } else {
            lexemes.push_back(lexeme);
            tokens.push_back(tokenmap[lexeme]);
        }
    } 
    else if(lexeme.front()=='"') {

        if(lexeme.back() != '"' || lexeme.length() == 1) {
            lexemes.push_back("error");
            tokens.push_back(lexeme + ": unclosed string");
        }
        else if (tValid) {
            lexemes.push_back(lexeme.substr(1, lexeme.length() - 2));
            tokens.push_back("t_text");
        } else {
            lexemes.push_back("error");
            tokens.push_back(lexeme + ": invalid t_ token ");
        }
    }
    else if(isValidId(lexeme)) {
        if (tValid) {
            lexemes.push_back(lexeme);
            tokens.push_back("t_id");
        } else {
            lexemes.push_back("error");
            tokens.push_back(lexeme + ": invalid t_ token");
        }
    }
    else if(isValidNumber(lexeme)) {
        if (tValid) {
            lexemes.push_back(lexeme);
            tokens.push_back("t_number");
        } else {
            lexemes.push_back("error");
            tokens.push_back(lexeme + ": invalid t_ token");
        }
    }
    else {
        lexemes.push_back("error");
        tokens.push_back(lexeme + ": unknown lexeme");
    }
    spaceBefore = false;
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
    cout << "success lex" << endl;
}
void LexAnalyzer::scanFile(istream &infile, ostream &outfile) {
    string currentLine;
    string current;
    bool inString = false;
    bool spaceBefore = true;

    while (getline(infile, currentLine)) {
        if (!currentLine.empty() && currentLine.back() == '\r') {
            currentLine.pop_back();
        }
        spaceBefore = true;
        for(int i=0;i<currentLine.length();i++){
            char ch = currentLine[i];
            
            if (ch == '"') {
                if (!current.empty() && !inString) {
                    pushToLexemes(current, spaceBefore, false);
                    current.clear();
                }
                current += ch;
                if (inString) {
                    bool tEndValid = (i == currentLine.size() - 1) || (isValidSymbol(currentLine[i+1]) || isspace(currentLine[i+1]));
                    pushToLexemes(current, spaceBefore, tEndValid);
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
                    pushToLexemes(current, spaceBefore, true);
                    current.clear();
                }
                spaceBefore = true;
            }
            else if (isValidSymbol(ch)) {
                if (!current.empty()) {
                    pushToLexemes(current, spaceBefore, true);
                    current.clear();
                }

                string op(1, ch);

                char next = currentLine[i+1];
                string twoCharOp = op + next;
                if (tokenmap.count(twoCharOp)) {
                    i++;
                    op = twoCharOp;
                }
                pushToLexemes(op, spaceBefore, false); 
            }
            else {
                current += ch;
            }
        }
    }

    if (!current.empty()) {
        pushToLexemes(current, spaceBefore, true);
    }

    writeToFile(outfile);
}