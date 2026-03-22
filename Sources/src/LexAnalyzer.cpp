
#include "LexAnalyzer.h"
#include <sstream>
#include <iterator>
// pre: parameter refers to open data file consisting of token and
// lexeme pairs i.e.  t_and and
// Each pair appears on its own input line.
// post: tokenmap has been populated - key: lexeme, value: token

LexAnalyzer::LexAnalyzer(istream& infile) {
    string line;
    while(getline(infile, line)) {
        vector<string> lex_pairs = split(line);

        string token = lex_pairs[0];
        string lexeme = lex_pairs[1];

        tokenmap[lexeme] = token;
    }
    for (pair<const string, string>& p: tokenmap){
        cout << p.first << ":==:" << p.second << endl;
    }
}

vector<string> LexAnalyzer::split(const string& line){
    stringstream stream(line);
    return vector<string>(
        istream_iterator<string>{stream},
        istream_iterator<string>{}
    );
}

// pre: 1st parameter refers to an open text file that contains source
// code in the language, 2nd parameter refers to an open empty output
// file
// post: If no error, the token and lexeme pairs for the given input
// file have been written to the output file (token : lexeme).
// If there is an error, the incomplete token/lexeme pairs, as well as
// an error message have been written to the output file.
// A success or fail message has printed to the console.
void LexAnalyzer::scanFile(istream& infile, ostream& outfile) {

}
