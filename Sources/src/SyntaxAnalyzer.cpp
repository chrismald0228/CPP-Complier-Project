// Alex and Chris 4/8/26
#include "SyntaxAnalyzer.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

SyntaxAnalyzer::SyntaxAnalyzer(istream& infile) {
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

bool SyntaxAnalyzer::parse() {
    tokitr = tokens.begin();
    lexitr = lexemes.begin();

    if (tokitr != tokens.end()) {
        vdec();
        if (tokitr != tokens.end() && *tokitr == "t_main") {
            tokitr++; lexitr++;
            if (!stmtlist()) {
                cout << "error in source: invalid statement list" << endl;
                return false;
            }
            cout << *tokitr << " : " << *lexitr << endl;
            if (tokitr != tokens.end() && *tokitr == "t_end") {
                tokitr++; lexitr++;
                cout << "Success" << endl;
                return true;
            }
            cout << "error in source: expected end" << endl;
            return false;
        }
    }
    cout << "error in source: expected main" << endl;
    return false;
}


bool SyntaxAnalyzer::vdec(){
    if(tokitr != tokens.end() && *tokitr == "t_var") {
        tokitr++; lexitr++;
        if(!vars()) {

            return false;
        }
        // bool moreVars = true;
        // while(moreVars) {
        //     if(!vars()) {
        //         moreVars = false;
        //     }
        // }
        while(tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string")) {
            if(!vars()) break;
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
    return false;
}
bool SyntaxAnalyzer::elsepart(){
    if(tokitr != tokens.end() && *tokitr == "t_else") {
        cout << "in else" << endl;
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
        if(!symboltable.count(id)) {

            return false;
        }
        idType = symboltable.at(id);
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
            if(tokitr != tokens.end() && *tokitr == "s_semi") {
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
    // bool moreLogicOps = true;
    // bool moreRelExpers = true;
    // while(moreLogicOps && moreRelExpers) {
    //     if(!logicop()) {
    //         moreLogicOps = false;
    //     }
    //     if(!relexpr()) {
    //         moreRelExpers = false;
    //     }
    // }
    while(tokitr != tokens.end()) {
        if(!logicop()) break;
        if(!relexpr()) {
            return false;
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


bool SyntaxAnalyzer::vars() {
    if (tokitr == tokens.end()){ return false;}
    string varType = *tokitr;
    if (!type()) {
        cout << "error in vars: expected type" << endl;
        return false;
    }
    if (tokitr != tokens.end() && *tokitr == "t_id") {
        if (symboltable.contains(*lexitr)) {
            cout << "error in vars: duplicate variable '" << *lexitr << "'" << endl;
            return false;
        }
        symboltable[*lexitr] = varType;
        tokitr++; lexitr++;
        while (tokitr != tokens.end() && *tokitr == "s_comma") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "t_id") {
                if (symboltable.contains(*lexitr)) {
                    cout << "error in vars: duplicate variable '" << *lexitr << "'" << endl;
                    return false;
                }
                symboltable[*lexitr] = varType;
                tokitr++; lexitr++;
            } else {
                cout << "error in vars: expected id after comma" << endl;
                return false;
            }
        }
        if (tokitr != tokens.end() && *tokitr == "s_semi") {
            tokitr++; lexitr++;
            return true;
        }
        cout << "error in vars: expected semicolon" << endl;
        return false;
    }
    cout << "error in vars: expected id" << endl;
    return false;
}

bool SyntaxAnalyzer::type() {
    if (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string")) {
        tokitr++; lexitr++;
        return true;
    }
    tokitr++; lexitr++;
    return false;
}


bool SyntaxAnalyzer::stmt() {
    if (tokitr == tokens.end()) { return false;}
    if (*tokitr == "t_if") {     return ifstmt();}
    if (*tokitr == "t_while") {  return whilestmt();}
    if (*tokitr == "t_id") {    return assignstmt();}
    if (*tokitr == "t_input") { return inputstmt();}
    if (*tokitr == "t_output") { return outputstmt();}
    return false;
}

bool SyntaxAnalyzer::whilestmt() {
    if (tokitr != tokens.end() && *tokitr == "t_while") {
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr == "s_lparen") {
            tokitr++; lexitr++;
            if (!logexpr()) {
                cout << "error in whilestmt: invalid logical expression" << endl;
                return false;
            }
            if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *tokitr == "t_loop") {
                    tokitr++; lexitr++;
                    if (!stmtlist()) {
                        cout << "error in whilestmt: invalid statement list" << endl;
                        return false;
                    }
                    if (tokitr != tokens.end() && *tokitr == "t_end") {
                        tokitr++; lexitr++;
                        if (tokitr != tokens.end() && *tokitr == "t_loop") {
                            tokitr++; lexitr++;
                            return true;
                        }
                    }
                }
            }
        }
    }
    cout << "error in whitestmt" << endl;
    return false;
}

bool SyntaxAnalyzer::inputstmt() {
    tokitr++; lexitr++;
    if (tokitr != tokens.end() && *tokitr == "s_lparen") {
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr == "t_id") {
            if (symboltable.contains(*lexitr)) {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                    tokitr++; lexitr++;
                    return true;
                }
            }
        }
    }
    cout << "error in inputstmt" << endl;
    return false;
}

bool SyntaxAnalyzer::relexpr() {
    if (!arithexpr()) {
        cout << "error in relexpr" << endl;
        return false;
    }
    if (!relop()) {
        cout << "error in relexpr" << endl;
        return false;
    }
    if (!arithexpr()) {
        cout << "error in relexpr" << endl;
        return false;
    }
    return true;
}

bool SyntaxAnalyzer::arithexpr() {
    if (!numterm()) {
        cout << "error in arithexpr: expected number or integer id" << endl;
        return false;
    }
    while (tokitr != tokens.end() &&
           (*tokitr == "s_plus" || *tokitr == "s_minus" || *tokitr == "s_mult" ||
            *tokitr == "s_div"  || *tokitr == "s_mod")) {
        arithop();
        if (!numterm()) {
            cout << "error in arithexpr: expected numterm after operator" << endl;
            return false;
        }
    }
    return true;
}

bool SyntaxAnalyzer::strterm() {
    if (tokitr != tokens.end() && *tokitr == "t_text") {
        tokitr++; lexitr++;
        return true;
    }
    if (tokitr != tokens.end() && *tokitr == "t_id") {
        if (!symboltable.contains(*lexitr)) {
            cout << "error in strterm: undeclared variable '" << *lexitr << "'" << endl;
            return false;
        }
        if (symboltable[*lexitr] != "t_string") {
            cout << "error in strterm: type mismatch" << endl;
            return false;
        }
        tokitr++; lexitr++;
        return true;
    }
    return false;
}

bool SyntaxAnalyzer::relop() {
    if (tokitr != tokens.end() &&
        (*tokitr == "s_lt" || *tokitr == "s_le" || *tokitr == "s_gt" ||
         *tokitr == "s_ge" || *tokitr == "s_eq" || *tokitr == "s_ne")) {
        tokitr++; lexitr++;
        return true;
    }
    tokitr++; lexitr++;
    return false;
}

bool SyntaxAnalyzer::arithop() {
    if (tokitr != tokens.end() &&
        (*tokitr == "s_plus"  || *tokitr == "s_minus" || *tokitr == "s_mult" ||
         *tokitr == "s_div"   || *tokitr == "s_mod")) {
        tokitr++; lexitr++;
        return true;
    }
    tokitr++; lexitr++;
    return false;
}
