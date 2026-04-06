#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string token, lexeme;
    string line;
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

void SyntaxAnalyzer::printError() {
    if(tokitr == tokens.end()){
        cout << "hit end of file" << endl;
    }
    cout << "Error in" << *tokitr << " : " << *lexitr << endl;
}

bool SyntaxAnalyzer::vdec(){
    if(tokitr != tokens.end() && *tokitr == "t_var") {
        tokitr++; lexitr++;
        if(!vars()) {
            printError();
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
                printError();
                return false;
            }
            if(tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++; lexitr++;
                if(tokitr != tokens.end() && *tokitr == "t_then") {
                    tokitr++; lexitr++;
                    if(!stmtlist()) {
                        printError();
                        return false;
                    }
                    if(!elsepart()) {
                        printError();
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
        printError();
        return false;
    }
    printError();
    return false;
}
bool SyntaxAnalyzer::elsepart(){
    if(tokitr != tokens.end() && *tokitr == "t_else") {
        cout << "in else" << endl;
        tokitr++; lexitr++;
        if(!stmtlist()) {
            printError();
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
        if(!symboltable.count(id)) {
            printError();
            return false;
        }
        idType = symboltable.at(id);
        tokitr++; lexitr++;
        if(tokitr != tokens.end() && *tokitr == "s_assign") {
            tokitr++; lexitr++;
            if(idType == "t_integer") {
                if(!arithexpr()) {
                    printError();
                    return false;
                }
            }
            else if(idType == "t_string") {
                if(!strterm()) {
                    printError();
                    return false;
                }
            }
            if(tokitr != tokens.end() && *tokitr == "s_semi") {
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    printError();
    return false;
}
bool SyntaxAnalyzer::outputstmt() {
    if(tokitr != tokens.end() && *tokitr == "t_output") {
        tokitr++; lexitr++;
        if(tokitr != tokens.end() && *tokitr == "s_lparen") {
            tokitr++; lexitr++;
            if(!numterm() && !strterm()) {
                printError();
                return false;
            }
            if(tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    printError();
    return false;
}
bool SyntaxAnalyzer::logexpr() {
    if(!relexpr()){
        printError();
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
    printError();
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
    printError();
    return false;
}
// pre: none
// post: The lexemes/tokens have been parsed.
// If an error occurs, a message prints indicating the token/lexeme pair
// that caused the error.
// If no error, vectors contain syntactically correct source code
// PROG  -> VDEC main STMTLIST end 