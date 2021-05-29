//
//  main.cpp
//  simple_c_scanner
//
//  Created by 施熠民 on 2021/3/21.
//


#include "regex2nfa.h"
#include "nfa2dfa.h"
#include <utility>

using namespace std;

// Global variables: used to count the index of NFA/DFA state.
int state_index_counter=0;
int DFA_state_index_counter=0;

// CompareCharVector_and_String(), helper function,
// to check whether the char vector is same as the string.
bool CompareCharVector_and_String(string str, vector<char>* charv){
    bool res = true;
    if (charv->size() != str.length()) {
        res = false;
    }
    for (int i = 0; i<str.length(); i++) {
        if (str[i] != (*charv)[i]) {
            res = false;
            break;
        }
    }
    return res;
}

// checkReserved(), helper function,
// to check whether the content in the buffer is in the reserved words list.
string checkReserved(vector<string>*reservedWords, vector<char>* buffer){
    for (string resword: *reservedWords) {
        if (CompareCharVector_and_String(resword, buffer)) {
            if (resword == "scanf") {
                return "READ";
            }
            if (resword == "printf") {
                return "WRITE";
            }
            transform(resword.begin(), resword.end(), resword.begin(), ::toupper);
            return resword;
        }
    }
    return "";
}

// scanner(), function,
// (1) a DFA simulator, which will check the next char in input in a loop, and transfer the DFA state
// to the next, until it stops in a final state, then output its token, and set the DFA state to the
// start state, continue to read char. (2)What's more, it will keep the string value of the token in
// a char vector buffer, and pop when the token type is found.
FILE* scanner(DFA_Graph* graph, FILE* fp){
    char next_char = '\0';
    bool keep_last = false;
    vector<char> input_buffer;
    DFA_state* current_DFA_state = graph->DFA_Start;
    vector<string> reserved_words = {"int", "main", "void", "break", "do", "else", "if", "while", "return", "scanf", "printf"};
    while (true) {
        if (!keep_last) {
            next_char = fgetc(fp);
        }
        keep_last = false;
        // action is found
        bool found = false;
        if (!feof(fp)) {
            for (char k: current_DFA_state->DFA_actions) {
                if (k == next_char) {
                    found = true;
                    current_DFA_state = (graph->Dtran)[{current_DFA_state, k}];
                    break;
                }
            }
            if (!found) {
                if (current_DFA_state->is_final) {
                    string the_regName = current_DFA_state->final_regex_names[0];
                    if (the_regName != "BLANK") {
                        if (the_regName == "IDENTIFIER") {
                            string reserved = checkReserved(&reserved_words, &input_buffer);
                            if (reserved!="") {
                                the_regName = reserved;
                            }
                        }
                        string buffer_string(input_buffer.begin(), input_buffer.end());
//                        cout<<"[SCA]: "<<buffer_string<<"\t->\t"<<the_regName<<endl;
//                        cout<<the_regName<<" ";
                        cout<<the_regName<<':'<<buffer_string<<" ";
                    }
                    keep_last = true;
                    current_DFA_state = graph->DFA_Start;
                    input_buffer.clear();
                }
                else{
                    cout<<"[ERR]:Fucked"<<endl;
                }
            }
            else{
                input_buffer.push_back(next_char);
            }
        }
        else{
            break;
        }
    }
    return fp;
}

// RegEx2DFA(), function,
// which can convert and merge a whole list of regular expressison strings into a overall DFA, returns
// the DFA graph class object pointer.
DFA_Graph* RegEx2DFA(vector<string> regs, vector<string> regnames){
    vector<NFA_SubGraph*> NFA_subgraphs;
    for (int i = 0; i<regs.size(); i++) {
        NFA_SubGraph* an_NFA_subgraph = new NFA_SubGraph(regs[i], regnames[i]);
        NFA_subgraphs.push_back(an_NFA_subgraph);
    }
    NFA_SubGraph* merged = new NFA_SubGraph(&NFA_subgraphs);
    DFA_Graph* res = new DFA_Graph(merged);
    return res;
}

// main(), main function,
// (1) sets up the regular expressison strings and the coressponding token name strings; (2) build up a
// DFA graph object for regexs; (3) read an input source code file of simple c and call the scanner() to
// do the syntax analysis.
int main(int argc, const char * argv[]) {
    
    // (1) sets up the regular expressison strings and the coressponding token name strings;
    vector<string>regs = {"((\\+|\\-)?).(0-9)+", "(((a-z)|(A-Z))+).((0-9)|((a-z)|(A-Z))|_)*", "{", "}", "[", "]", "(", ")", ";", "\\+", "\\-", "\\*", "/", "&", "\\|", "!", "=", "<", ">", "<.<", ">.>", "=.=", "!.=", "<.=", ">.=", "&.&", "\\|.\\|", ",",  "( |\t|\n)"};
    vector<string>regnames = {"INT_NUM", "IDENTIFIER", "LBRACE", "RBRACE", "LSQUARE", "RSQUARE", "LPAR", "RPAR", "SEMI", "PLUS", "MINUS", "MUL_OP", "DIV_OP", "AND_OP", "OR_OP", "NOT_OP", "ASSIGN", "LT", "GT", "SHL_OP", "SHR_OP", "EQ", "NOTEQ", "LTEQ", "GTEQ", "ANDAND", "OROR", "COMMA", "BLANK"};
    
    // (2) build up a DFA graph object for regexs;
    DFA_Graph* dfa = RegEx2DFA(regs, regnames);
    
    // (3) read an input source code file of simple c and call the scanner() to do the syntax analysis.
    FILE* fp;
    
    // using the file path to open and read the file
//    if((fp = fopen("/Users/shiyimin/Desktop/CSC4180/HW2/src/simple_c_scanner/simple_c_scanner/test01.txt","r")) == NULL){
//        printf("File not exist");
//        exit(0);
//    }
    
    // using the user input arguments to open and read the file
    if((fp = fopen(argv[1],"r")) == NULL){
        cout<<"File not exist"<<endl;
        exit(0);
    }
//    cout<<"Result:"<<endl;
//    cout<<"\tValues:\t\tTokens:"<<endl;
    fp = scanner(dfa, fp);
    // finished, close and free the input file pointer.
    fclose(fp);
    fp=NULL;
//    cout<< "done.";
    cout<<"$";
    cout<<endl;
    
    return 0;
}
