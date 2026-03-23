
#include "LexAnalyzer.h"
#include <sstream>
#include <iterator>
// pre: parameter refers to open data file consisting of token and
// lexeme pairs i.e.  t_and and
// Each pair appears on its own input line.
// post: tokenmap has been populated - key: lexeme, value: token

LexAnalyzer::LexAnalyzer(istream &infile)
{
    populateTokenmap(infile);
}

void LexAnalyzer::populateTokenmap(istream &infile)
{
    string line;
    while (getline(infile, line))
    {
        vector<string> lex_pairs = split(line);

        string token = lex_pairs[0];
        string lexeme = lex_pairs[1];

        tokenmap[lexeme] = token;
    }
}
vector<string> LexAnalyzer::split(const string &line)
{
    stringstream stream(line);
    return vector<string>(
        istream_iterator<string>{stream},
        istream_iterator<string>{});
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
    string current = "";
    char ch;
    bool inString = false;

    while (infile.get(ch)) {
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
        else if (ispunct(ch)) {
            if (!current.empty()) {
                pushToLexemes(current);
                current.clear();
            }

            string op(1, ch);

            char next = infile.peek();
            string twoCharOp = op + next;
            if (tokenmap.count(twoCharOp)) {
                infile.get();
                op = twoCharOp;
            }
            pushToLexemes(op);
        }
        else {
            current += ch;
        }
    }

    if (!current.empty()) {
        pushToLexemes(current);
    }

    writeToFile(outfile);
}
