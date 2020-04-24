//Lexical Analyzer

/* Name :       final state                     {intermediate state}    (for charaters)
Identifier:     1                               {}                      ( _, a-zA-Z0-9 ) // combination of those without 0-9 alone
Number:         2,4,7                           {3,5,6}                 (0-9,.,e,+,-)
Comment:        12                              {10,11}                 (// - \n, /* [Any] * / )
Character:      15                              {14,16,17}              (',')
String:         19                              {18,20}                 (",")
Spaces:         21                              {}                      (' ')
Separator:      22                              {}                      ( {, }, ; )
Operator:       8,9,23,24,25,26,27,28,29,30     {}                      ( [, ], (, ), ~, ., ',', !, !=, 
                                                                            &, &=, |, |=, ^, ^=, +, ++, 
                                                                            +=, -, --, -=, ->, *, **, *=, 
                                                                            %, %=, =, ==, >, >=, >>, >>=, 
                                                                            <, <=, <<, <<=)
Newline         31                              {}                      (\n,\r)
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define numOfStates 32
#define numOfCharTypes 24

struct Token{
   int type;
   int value;
};

struct DFA{
    int currPos;
    int TransitionMtx[numOfStates][numOfCharTypes];
    int finalStates[numOfStates] ;
    vector<string> UniqueValueVec;
    
};

// Token Types
const int Error = -1;
const int Identifier = 0;
const int Number = 1;      
const int Comment = 2;    
const int Character = 3;   
const int String = 4;      
const int Spaces = 5;       
const int Separator = 6;
const int Operator = 7;
const int EORF = 8;

// Chr Types
const int Letter = 0;
const int UnderScore = 1;
const int Digit = 2;
const int Dot = 3;
const int E = 4;
const int Slash = 5;
const int BackSlash = 6;
const int Star = 7;
const int Equal = 8;
const int NewLine = 9;
const int Quote = 10;
const int DoubleQuote = 11;
const int Space = 12;
const int Sep = 13; //Separator {};
const int Op = 14; //Operator ()[]~.,
const int Not = 15;
const int Ampersand = 16;
const int Pipe = 17;
const int Caret = 18;
const int Plus = 19;
const int Minus = 20;
const int Percent = 21;
const int GreaterThan = 22;
const int LessThan = 23;



void initTransMtx(DFA &dfa){
    for(int i=0; i < numOfStates; i++){
        for(int j=0; j < numOfCharTypes; j++){
            dfa.TransitionMtx[i][j] = -1;
        }
    }

    dfa.TransitionMtx[0][Letter] = 1;
    dfa.TransitionMtx[0][E] = 1;
    dfa.TransitionMtx[0][UnderScore] = 1;
    dfa.TransitionMtx[0][Digit] = 2;
    dfa.TransitionMtx[0][Slash] = 8;
    dfa.TransitionMtx[0][Op] = 9;
    dfa.TransitionMtx[0][Dot] = 9;
    dfa.TransitionMtx[0][Quote] = 14;
    dfa.TransitionMtx[0][DoubleQuote] = 18;
    dfa.TransitionMtx[0][Space] = 21;
    dfa.TransitionMtx[0][Sep] = 22;
    dfa.TransitionMtx[0][Not] = 23;
    dfa.TransitionMtx[0][Caret] = 23;
    dfa.TransitionMtx[0][Percent] = 23;
    dfa.TransitionMtx[0][Equal] = 23;
    dfa.TransitionMtx[0][Ampersand] = 24;
    dfa.TransitionMtx[0][Pipe] = 25;    
    dfa.TransitionMtx[0][Plus] = 26;
    dfa.TransitionMtx[0][Minus] = 27;
    dfa.TransitionMtx[0][Star] = 28;
    dfa.TransitionMtx[0][GreaterThan] = 29;
    dfa.TransitionMtx[0][LessThan] = 30;
    dfa.TransitionMtx[0][NewLine] = 31;
    
    dfa.TransitionMtx[1][E] = 1;
    dfa.TransitionMtx[1][Letter] = 1;
    dfa.TransitionMtx[1][UnderScore] = 1;
    dfa.TransitionMtx[1][Digit] = 1;
    
    dfa.TransitionMtx[2][Digit] = 2;
    dfa.TransitionMtx[2][Dot] = 3;
    dfa.TransitionMtx[2][E] = 5;
    
    dfa.TransitionMtx[3][Digit] = 4;
    
    dfa.TransitionMtx[4][Digit] = 4;
    dfa.TransitionMtx[4][E] = 5;
    
    dfa.TransitionMtx[5][Plus] = 6;
    dfa.TransitionMtx[5][Minus] = 6;
    dfa.TransitionMtx[5][Digit] = 7;

    dfa.TransitionMtx[6][Digit] = 7;
    
    dfa.TransitionMtx[7][Digit] = 7;
    
    dfa.TransitionMtx[8][Equal] = 9;
    dfa.TransitionMtx[8][Star] = 10;
    dfa.TransitionMtx[8][Slash] = 13;

    for(int i = 0; i < numOfCharTypes; i++){
        if( i == Star)
            dfa.TransitionMtx[10][i] = 11;
        else
            dfa.TransitionMtx[10][i] = 10;
    }
    
    for(int i = 0; i < numOfCharTypes; i++){
        if( i == Star)
            dfa.TransitionMtx[11][i] = 11;
        else if (i == Slash)
            dfa.TransitionMtx[11][i] = 12;
        else
            dfa.TransitionMtx[11][i] = 10;
    }
    
    for(int i = 0; i < numOfCharTypes; i++){
        if( i == NewLine)
            dfa.TransitionMtx[13][i] = 12;
        else
            dfa.TransitionMtx[13][i] = 13;
    }
    
    for(int i = 0; i < numOfCharTypes; i++){
        if( i == Quote)
            dfa.TransitionMtx[14][i] = 15;
        else if (i == BackSlash)
            dfa.TransitionMtx[14][i] = 17;
        else
            dfa.TransitionMtx[14][i] = 16;
    }

    dfa.TransitionMtx[16][Quote] = 15;
    
    for(int i = 0; i < numOfCharTypes; i++){
        dfa.TransitionMtx[17][i] = 16;
    }
    
    for(int i = 0; i < numOfCharTypes; i++){
        if( i == DoubleQuote)
            dfa.TransitionMtx[18][i] = 19;
        else if (i == BackSlash)
            dfa.TransitionMtx[18][i] = 20;
        else
            dfa.TransitionMtx[18][i] = 18;
    }
    
    for(int i = 0; i < numOfCharTypes; i++){
        dfa.TransitionMtx[20][i] = 18;
    }

    dfa.TransitionMtx[21][Space] = 21;

    dfa.TransitionMtx[23][Equal] = 9;

    dfa.TransitionMtx[24][Ampersand] = 9;
    dfa.TransitionMtx[24][Equal] = 9;
    
    dfa.TransitionMtx[25][Pipe] = 9;
    dfa.TransitionMtx[25][Equal] = 9;
    
    dfa.TransitionMtx[26][Plus] = 9;
    dfa.TransitionMtx[26][Equal] = 9;
    
    dfa.TransitionMtx[27][Minus] = 9;
    dfa.TransitionMtx[27][Equal] = 9;
    dfa.TransitionMtx[27][GreaterThan] = 9;
    
    dfa.TransitionMtx[28][Equal] = 9;
    
    dfa.TransitionMtx[29][GreaterThan] = 23;
    dfa.TransitionMtx[29][Equal] = 9;
    
    dfa.TransitionMtx[30][LessThan] = 23;
    dfa.TransitionMtx[30][Equal] = 9;

    dfa.TransitionMtx[31][NewLine] = 31;
}

void initFinalStates(DFA &dfa){

    for(int i = 0; i < numOfStates; i++ )
        dfa.finalStates[i] = Error;
    
    dfa.finalStates[1] = Identifier;                             
    dfa.finalStates[2] = Number;
    dfa.finalStates[4] = Number;
    dfa.finalStates[7] = Number;                          
    dfa.finalStates[12] = Comment;                              
    dfa.finalStates[15] = Character;                              
    dfa.finalStates[19] = String;                     
    dfa.finalStates[21] = Spaces;                              
    dfa.finalStates[22] = Separator;                              
    dfa.finalStates[8] = Operator;                              
    dfa.finalStates[9] = Operator;                              
    dfa.finalStates[23] = Operator;                            
    dfa.finalStates[24] = Operator;                              
    dfa.finalStates[25] = Operator;                              
    dfa.finalStates[26] = Operator;                             
    dfa.finalStates[27] = Operator;                              
    dfa.finalStates[28] = Operator;                              
    dfa.finalStates[29] = Operator;                              
    dfa.finalStates[30] = Operator;                              
    dfa.finalStates[31] = NewLine;


}

void initDfa(DFA &dfa){

    initTransMtx(dfa);
    initFinalStates(dfa);

}

string getTokenTypeName(int tokenType){
    switch (tokenType){

    case Error :        return "Error";
    case Identifier :   return "Identifier";
    case Number :       return "Number";
    case Comment  :     return "Comment";
    case Character :    return "Character";
    case String :       return "String";
    case Spaces :       return "Spaces";
    case Separator :    return "Separator";
    case Operator :     return "Operator";
    case EORF :         return "EORF";

    }
}

int getCharType(char chr){
    
    if(isalnum(chr)){
        if(isdigit(chr)){
            return Digit;

        }else if(tolower(chr) == 'e'){
            return E;
        }

        return Letter;
    }

    switch (chr){

        case '_':   return UnderScore;
        case '.':   return Dot;
        case '/':   return Slash;
        case '\\':  return BackSlash;
        case '*':   return Star;
        case '=':   return Equal;
        case '\n':  
        case '\r':  return NewLine;
        case '\'':  return Quote;
        case '"':   return DoubleQuote;
        case ' ':   return Space;
        case '{':
        case '}':
        case ';':   return Sep; 
        case '(':
        case ')':
        case '[':
        case ']':
        case ',':
        case '~':   return Op;
        case '!':   return Not;
        case '&':   return Ampersand;
        case '|':   return Pipe;
        case '^':   return Caret;
        case '+':   return Plus;
        case '-':   return Minus;
        case '%':   return Percent;
        case '>':   return GreaterThan;
        case '<':   return LessThan;
        
        default:    return Error;
    }
}

int getValueIndex(string val, DFA &dfa){

    int index;
    vector<string>::iterator foundAt;

    foundAt = find(dfa.UniqueValueVec.begin(),dfa.UniqueValueVec.end(),val);        
    
    if(foundAt != dfa.UniqueValueVec.end())   
        index = distance(dfa.UniqueValueVec.begin(),foundAt);

    else{   
        dfa.UniqueValueVec.push_back(val);
        index = dfa.UniqueValueVec.size()-1;

    }

    return index;
}


Token getToken( string code, DFA &dfa){

    Token token;
    char currChar;
    int currState = 0,
        nextState,
        currCharType;
    string currVal = "";

    while(dfa.currPos < code.length()){
        currChar = code[dfa.currPos];
        currCharType = getCharType(currChar);
        nextState = dfa.TransitionMtx[currState][currCharType];

        if(nextState != Error){
            currState = nextState;
            dfa.currPos++;
            currVal += currChar;

        }else{
            token.type = dfa.finalStates[currState];
            switch (token.type){

                case Comment: 
                case Spaces: 
                case NewLine:   currVal = ""; 
                                currState = 0; 
                                break;
                 
                case Error:     token.value = dfa.currPos; 
                                return token;
                
                default:        token.value = getValueIndex(currVal, dfa);
                                return token;
            }
        }
    }

    if((token.type = dfa.finalStates[currState]) == Error)
        token.value = dfa.currPos;

    else{
        token.type = EORF;
        token.value = -1;

    }
        
    return token;
        
}


int main(int argc, char** argv){

    ifstream fin(argv[1]);
    string code;
    DFA dfa;
    Token token;
    vector<Token> foundTokens;

    code.assign( (istreambuf_iterator<char>(fin)), (istreambuf_iterator<char>()) );
    fin.close();
    
    dfa.currPos = 0;
    initDfa(dfa);

    while(dfa.currPos < code.length()){
        token = getToken(code,dfa);

        switch (token.type){
        case Error: cout<<"\nError at position "<<token.value<<endl;
                    return 0;
        
        default:    foundTokens.push_back(token);
                    break;
        }
    }

    for(int i = 0; i < foundTokens.size() - 1; i++)
        cout<< getTokenTypeName(foundTokens[i].type) <<": "<<dfa.UniqueValueVec[foundTokens[i].value]<<endl;

    return 0;
}
