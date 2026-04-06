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

    // vdec chack if first value is var. returns true either way
    // check there are no duplicate vars
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
bool SyntaxAnalyzer::relexpr() {
    return true;
}
bool SyntaxAnalyzer::vars() {
    if(tokitr != tokens.end() && *tokitr == "t_id") {
        tokitr++; lexitr++;
        return true;
    }
    return false;
}
bool SyntaxAnalyzer::strterm() {
    return true;
}
bool SyntaxAnalyzer::arithexpr() {
    return true;
}
bool SyntaxAnalyzer::stmt() {
    if(tokitr != tokens.end() && *tokitr == "t_if") {
        tokitr++; lexitr++;
        return true;
    }
    return false;
}
bool SyntaxAnalyzer::vdec(){
    cout << "vdec called" << endl;
    if(tokitr != tokens.end() && *tokitr == "t_var") {
        cout << "t_var found" << endl;
        tokitr++; lexitr++;
        if(!vars()) {
            return false;
        }
        bool moreVars = true;
        while(moreVars) {
            if(!vars()) {
                moreVars = false;
            }
        }
    }
    return true;
}
// fix
bool SyntaxAnalyzer::stmtlist(){
    bool moreStmts = true;
    while(moreStmts) {
        if(!stmt()) {
            moreStmts = false;
        }
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
bool SyntaxAnalyzer::assignstmt(){
    string id;
    string idType;
    if(tokitr != tokens.end() && *tokitr == "t_id") {
        id = *lexitr;
        if(symboltable.count(id)) {
            idType = symboltable.at(id);
        }
        tokitr++; lexitr++;
        if(tokitr != tokens.end() && *tokitr == "s_assign") {
            tokitr++; lexitr++;
            if(idType == "t_integer") {
                if(!arithexpr()) {
                    return false;
                }
            }
            else if(idType == "t_string") {
                if(!strterm()) {
                    return false;
                }
            }
            if(tokitr != tokens.end() && *tokitr == "s_semicolon") {
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}
bool SyntaxAnalyzer::outputstmt() {
    if(tokitr != tokens.end() && *tokitr == "t_output") {
        tokitr++; lexitr++;
        if(tokitr != tokens.end() && *tokitr == "s_lparen") {
            tokitr++; lexitr++;
            if(!numterm() && !strterm()) {
                return false;
            }
            if(tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}
bool SyntaxAnalyzer::logexpr() {
    if(!relexpr()){
        return false;
    }
    bool moreLogicOps = true;
    bool moreRelExpers = true;
    while(moreLogicOps && moreRelExpers) {
        if(!logicop()) {
            moreLogicOps = false;
        }
        if(!relexpr()) {
            moreRelExpers = false;
        }
    }
    return true;
}
bool SyntaxAnalyzer::numterm() {
    if(tokitr != tokens.end() && *tokitr == "t_number") {
        tokitr++; lexitr++;
        return true;
    }
    if(tokitr != tokens.end() && *tokitr == "t_id") {
        string id = *lexitr;
        if(symboltable.count(id) && symboltable.at(id) == "t_integer") {
            tokitr++; lexitr++;
            return true;
        }
    }
    return false;
}
bool SyntaxAnalyzer::logicop() {
    if(tokitr != tokens.end() && *tokitr == "t_and") {
        tokitr++; lexitr++;
        return true;
    }
    if(tokitr != tokens.end() && *tokitr == "t_or") {
        tokitr++; lexitr++;
        return true;
    }
    return false;
}
// pre: none
// post: The lexemes/tokens have been parsed.
// If an error occurs, a message prints indicating the token/lexeme pair
// that caused the error.
// If no error, vectors contain syntactically correct source code
// PROG  -> VDEC main STMTLIST end 