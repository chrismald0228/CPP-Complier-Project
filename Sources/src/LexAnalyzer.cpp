
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
bool LexAnalyzer::isValidStrBoundary(char c) {
    return c== '\0' || isspace(c) || ispunct(c);
}

void LexAnalyzer::scanFile(istream &infile, ostream &outfile)
{
    vector<string> lexemes;
    string current = "";
    char ch;
    char prevChar = '\0';
    bool inString = false;

    bool error = false;
    string errorMsg = "";
    int errorIndex = -1;

    while (infile.get(ch)) {
        if (ch == '"') {
            if (!inString && !current.empty() && !isValidStrBoundary(prevChar) && !error) {
                error = true;
                errorMsg = "ERROR : invalid boundary before string";
                errorIndex = lexemes.size();
            }

            if (!current.empty() && !inString) {
                lexemes.push_back(current);
                current.clear();
            }

            current += ch;

            if (inString) {
                lexemes.push_back(current);
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
                lexemes.push_back(current);
                current.clear();
            }
        }
        else if (ispunct(ch)) {
            if (!current.empty()) {
                lexemes.push_back(current);
                current.clear();
            }

            string op(1, ch);

            if (!infile.eof()) {
                char next = infile.peek();
                string twoCharOp = op + next;

                if (tokenmap.count(twoCharOp)) {
                    infile.get();
                    op = twoCharOp;
                }
            }

            lexemes.push_back(op);
        }
        else {
            current += ch;
        }

        prevChar = ch;
    }

    if (!current.empty()) {
        lexemes.push_back(current);
    }

    // error if string was never closed
    if (inString && !error) {
        error = true;
        errorMsg = "ERROR : unterminated string";
        errorIndex = lexemes.size();
    }

    //throw error if any, print tokens/lexemes, assign id, number, strings
    for (size_t i = 0; i < lexemes.size(); i++) {

        if (error && i == (size_t)errorIndex) {
            outfile << errorMsg << endl;
            outfile << "Lexical analysis failed." << endl;
            cout << "Scan failed." << endl;
            return;
        }

        const string &lex = lexemes[i];

        if (tokenmap.count(lex)) {
            outfile << tokenmap[lex] << " " << lex << endl;
        }
        else if (lex.front() == '"' && lex.back() == '"') {
            outfile << "t_text " << lex << endl;
        }
        else if (isValidNumber(lex)) {
            outfile << "t_number " << lex << endl;
        }
        else if (isValidId(lex)) {
            outfile << "t_id " << lex << endl;
        }
        else {
            outfile << "ERROR : " << lex << endl;
            outfile << "Lexical analysis failed." << endl;
            cout << "Scan failed." << endl;
            return;
        }
    }

    cout << "Scan successful." << endl;
}
