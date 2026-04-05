#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string token, lexeme;
    string line;
    cout << "SyntaxAnalyzer constructor called" << endl;
    while(getline(infile, line)) {
        int spacePos = line.find(' ');
        string token = line.substr(0, spacePos);
        string lexeme = line.substr(spacePos + 1);
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
        if(tokitr != tokens.end())
            cout << "error in line " << *tokitr << endl;
        return 0;
    }
    if(tokitr != tokens.end() && *tokitr == "t_main"){
        cout << "vdec pass" << endl;
        tokitr++; lexitr++;
        if(!stmtlist()) {
            if(tokitr != tokens.end())
                cout << "error in line " << *tokitr << endl;
            return 0;
        }
        cout << "stmtlist pass" << endl;
        if(tokitr != tokens.end() && *tokitr == "t_end"){
            cout << "t_end found" << endl;
            tokitr++; lexitr++;
            if(tokitr == tokens.end()){
                cout << "success parse" << endl;
            } else {
                cout << "tokens not empty" << endl;
            }
        }
    } else {
        if(tokitr != tokens.end()) {
            cout << "error in line" << *tokitr << endl;
        }
    }
    return 0;
}

bool SyntaxAnalyzer::vdec(){
    cout << "vdec called" << endl;
    if(tokitr != tokens.end() && *tokitr == "t_var") {
        cout << "t_var found" << endl;
        tokitr++; lexitr++;
        if(!vars()) {
            return false;
        }
    }
    return true;
}
int SyntaxAnalyzer::vars(){
    return true;
}
// fix
bool SyntaxAnalyzer::stmtlist(){
    if(!stmt()) {
        if(tokitr != tokens.end())
            cout << "error in line " << *tokitr << endl;
        return 0;
    }
    return true;
}
bool SyntaxAnalyzer::ifstmt(){
    if(tokitr != tokens.end() && *tokitr == "t_if") {
        tokitr++; lexitr++;
        if(tokitr != tokens.end() && *tokitr =="s_lparen") {
            tokitr++; lexitr++;
            if(!logexpr()) {
                return false;
            }
            if(tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++; lexitr++;
                if(tokitr != tokens.end() && *tokitr == "t_then") {
                    tokitr++; lexitr++;
                    if(!stmtlist()) {
                        return false;
                    }
                    if(!elsepart()) {
                        return false;
                    }
                    if(tokitr != tokens.end() && *tokitr == "t_end") {
                        tokitr++; lexitr++;
                        if(tokitr != tokens.end() && *tokitr == "t_if") {
                            tokitr++; lexitr++;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    // return true;
    return false;
}
bool SyntaxAnalyzer::elsepart(){
    if(tokitr != tokens.end() && *tokitr == "t_else") {
        tokitr++; lexitr++;
        if(!stmtlist()) {
            return false;
        }
    }
    return true;
}
// pre: none
// post: The lexemes/tokens have been parsed.
// If an error occurs, a message prints indicating the token/lexeme pair
// that caused the error.
// If no error, vectors contain syntactically correct source code
// PROG  -> VDEC main STMTLIST end 