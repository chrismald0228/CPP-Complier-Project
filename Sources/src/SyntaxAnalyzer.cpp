#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string token, lexeme;
    while(infile >> token >> lexeme) {
        tokens.push_back(token);
        lexemes.push_back(lexeme);
    }
}
// pre: 1st parameter consists of an open file containing a source code's
//	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
// post: the vectors have been populated

// bump up iterator after the final check in a function
bool SyntaxAnalyzer::parse(){
    tokitr = tokens.begin();
    lexitr = lexemes.begin();

    if(!vdec()) {
        cout << "error in line" << *tokitr << endl;
        return 0;
    }
    if(tokitr != tokens.end() && *tokitr == "t_main"){
        tokitr++; lexitr++;
        if(!stmtlist()) {
            cout << "error in line" << *tokitr << endl;
            return 0;
        }
        if(tokitr != tokens.end() && *tokitr == "t_end"){
            tokitr++; lexitr++;
            if(tokitr == tokens.end()){
                cout << "success" << endl;
            } else {
                cout << "tokens not empty" << endl;
            }
        }
    } else {
        cout << "error in line" << *tokitr << endl;
    }

}
// pre: none
// post: The lexemes/tokens have been parsed.
// If an error occurs, a message prints indicating the token/lexeme pair
// that caused the error.
// If no error, vectors contain syntactically correct source code
// PROG  -> VDEC main STMTLIST end 