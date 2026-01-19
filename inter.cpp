#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <stack>

using namespace std;

bool fl_test = 1;

enum type_lex{LEX_NULL, LEX_AND, LEX_NOT, LEX_OR, LEX_STRING, LEX_ELSE, LEX_IF, LEX_INT, LEX_PROGRAM,
    LEX_READ, LEX_WHILE, LEX_WRITE, LEX_SEMICOLON, LEX_COMMA, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_MUL, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_LBRACK, LEX_RBRACK,  
    LEX_FIN,            
    LEX_NUM, LEX_STR,                                                                                   
    LEX_ID,                                                                                     
    POLIZ_LABEL,                                                                                
    POLIZ_ADDRESS,                                                                              
    POLIZ_GO,                                                                                   
    POLIZ_FGO
};

vector<string> TS = {"", "and", "not", "or", "string", "else", "if", "int", "program", "read", "while", "write", 
    ";", ",", "=", "(", ")", "==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", "{", "}"
};


class Lex{
    type_lex tlex;
    int nlex, vlex;
    static int count_lex;
public:
    Lex(type_lex t = LEX_NULL, int val = 0){
        tlex = t; 
        vlex = val;
        nlex = count_lex;
        count_lex++;
    }

    int get_num(){
        return nlex;
    }

    type_lex get_type(){
        return tlex;
    }

    int get_val(){
        return vlex;
    }

    void set_val(int a){
        vlex = a;
    }

    void set_type(type_lex tp){
        tlex = tp;
    }

    friend ostream& operator << (ostream &s, Lex l);
};

int Lex::count_lex = 0;

ostream& operator << (ostream &s, Lex l){
    if((l.tlex > 0) && (l.tlex < 29)){
        s << "( " << l.nlex << " , " << l.tlex << " , " << TS[l.vlex] << " )" << endl;
    }else{
        s << "( " << l.nlex << " , " << l.tlex << " , " << l.vlex << " )" << endl;
    }
    return s;
}


enum type_id{T_INT = 7, T_STR = 4, T_UND = 0};

class Ident{
    string name;
    type_id type_ident = T_UND;
    string val_str;
    int val_int;

public:
    Ident(string nm){
        name = nm;
    }

    string get_name(){
        return name;
    }

    void set_type(type_id t){
        if(type_ident != T_UND) throw "Error: identificator " + name + " must be declared only 1 time";
        type_ident = t;
    }

    type_id get_type(){
        if(type_ident == T_UND) throw "Error: undefined type of identificator " + name;
        return type_ident;
    }

    void set_val(const int val){
        if(type_ident == T_UND) throw "Error: undefined type of identificator " + name;
        val_int = val;
    }

    void set_val(const string& val){
        if(type_ident == T_UND) throw "Error: undefined type of identificator " + name;
        val_str = val;
    }

    string get_val(const string& ret){
        if(type_ident == T_UND) throw "Error: undefined type of identificator " + name;
        return val_str;
    }

    int get_val(const int& ret){
        if(type_ident == T_UND) throw "Error: undefined type of identificator " + name;
        return val_int;
    }

    bool operator==(const Ident& id) const {
        return name == id.name;
    }
};

vector<Ident> TID;

vector<string> TSTR;

int put_TID(const string& buf) {
    vector<Ident>::iterator k = find(TID.begin(), TID.end(), buf);
 
    if (k != TID.end()){
        return k - TID.begin();
    }

    TID.push_back (Ident(buf));
    return TID.size() - 1;
}


class Scanner{
    ifstream file;
    char getter;

public:
    Scanner(string name_file): file(name_file){
        if(!file.is_open())
            throw "Error: can't open file";
        
        file.unsetf(ios_base::skipws);
    }

    int gc(){
        if(!file.eof())
            file >> getter;
        else
            return -1;
        return getter;
    }

    Lex get_lex();
};

Lex Scanner::get_lex(){
    string buf;
    enum stat{H, IDENT, NUM, STR, ALE, NEQ} status = H;
    int num = 0, sign = 0, flag = 0;

    while(gc() != -1){
        switch(status){
            case H:{
                switch (getter){
                    case ' ': case '\n': case '\t': case '\r': break;
                    case '"': status = STR; break;
                    case '!': status = NEQ; buf.push_back(getter); break;
                    case '<': case '>': case '=': status = ALE; buf.push_back(getter); break;
                    case '+': status = NUM; sign = 1; break;
                    case '-': status = NUM; sign = -1; break;
                    case '*': return Lex(LEX_MUL, LEX_MUL);
                    case '/': return Lex(LEX_SLASH, LEX_SLASH);
                    case '{': return Lex(LEX_LBRACK, LEX_LBRACK);
                    case '}': return Lex(LEX_RBRACK, LEX_RBRACK);
                    case '(': return Lex(LEX_LPAREN, LEX_LPAREN);
                    case ')': return Lex(LEX_RPAREN, LEX_RPAREN);
                    case ',': return Lex(LEX_COMMA, LEX_COMMA);
                    case ';': return Lex(LEX_SEMICOLON, LEX_SEMICOLON);
                    default: {
                        if(isdigit(getter)){
                            status = NUM;
                            num = getter - '0';
                            flag = 1;
                        }else if(isalpha(getter) || (getter == '_')){
                            status = IDENT;
                            buf.push_back(getter);
                        }else{
                            throw getter;
                        }
                    }
                }
                break;
            }
            case IDENT:{
                if(isdigit(getter) || isalpha(getter) || (getter == '_')){
                    buf.push_back(getter);
                }else{
                    file.seekg(-1, ios::cur);
                    vector<string>::iterator k = find(TS.begin(), TS.end(), buf);
                    if(k != TS.end()){
                        return Lex((type_lex) distance(TS.begin(), k), distance(TS.begin(), k));
                    }

                    return Lex(LEX_ID, put_TID(buf));
                }

                break;
            }
            case NUM:{
                if(isdigit(getter)){
                    flag = 1;
                    num = num * 10 + getter - '0';
                }else{
                    file.seekg(-1, ios::cur);
                    if(flag){
                        if(sign){
                            return Lex(LEX_NUM, num * sign);
                        }else{
                            return Lex(LEX_NUM, num);
                        }
                    }else{
                        if(sign == 1){
                            return Lex(LEX_PLUS, LEX_PLUS);
                        }else{
                            return Lex(LEX_MINUS, LEX_MINUS);
                        }
                    }
                }
                break;
            }
            case STR:{
                if(getter != '"'){
                    buf.push_back(getter);
                }else{
                    vector<string>::iterator k = find(TSTR.begin(), TSTR.end(), buf);
                    if(k == TSTR.end()){
                        TSTR.push_back(buf);
                        return Lex(LEX_STR, TSTR.size() - 1);
                    }
                    return Lex(LEX_STR, distance(TSTR.begin(), k));
                }
                break;
            }
            case ALE:{
                if(getter == '='){
                    switch(buf[0]){
                        case '<': return Lex(LEX_LEQ, LEX_LEQ);
                        case '>': return Lex(LEX_GEQ, LEX_GEQ);
                        case '=': return Lex(LEX_EQ, LEX_EQ);
                    }
                }else{
                    file.seekg(-1, ios::cur);
                    switch(buf[0]){
                        case '<': return Lex(LEX_LSS, LEX_LSS);
                        case '>': return Lex(LEX_GTR, LEX_GTR);
                        case '=': return Lex(LEX_ASSIGN, LEX_ASSIGN);
                    }
                }
            }
            case NEQ:{
                if(getter != '='){
                    throw "Error: undefined symbol !";
                }else{
                    return Lex(LEX_NEQ, LEX_NEQ);
                }
            }
        }
    }

    if(!buf.empty()){
        throw "Wrong ending of program";
    }

    return Lex(LEX_FIN);
} 


vector <Lex> poliz;

class Parser{
    Lex cur_lex, buf_lex;
    type_lex cur_type, seman_type;
    int cur_val;
    bool flag;
    Scanner scan;

    stack<type_lex> st_type_lex;

    void check_two_op(){
        //cout << "check" << endl;
        type_lex op1, oper, op2;
        op2 = st_type_lex.top();
        st_type_lex.pop();
        oper = st_type_lex.top();
        st_type_lex.pop();
        op1 = st_type_lex.top();
        st_type_lex.pop();

        //cout << "check_op" << op1 << " " << oper << " " << op2 << endl;

        if(op1 != op2){
            throw "Error: Bad types of operands";
        }

        switch(oper){
            case LEX_MINUS: case LEX_SLASH: case LEX_MUL: case LEX_AND: case LEX_OR:{
                if(op1 != LEX_INT){
                    throw "Error: Bad types of operands";
                } 
                st_type_lex.push(LEX_INT); 
                break;
            }

            case LEX_GEQ: case LEX_GTR: case LEX_LEQ: case LEX_LSS: case LEX_EQ:
            case LEX_NEQ: st_type_lex.push(LEX_INT);  break;

            case LEX_PLUS: case LEX_ASSIGN: st_type_lex.push(op1); break;
        }

        poliz.push_back(Lex(oper));
    }

    void check_one_op(){
        type_lex op1, oper;
        op1 = st_type_lex.top();
        st_type_lex.pop();
        oper = st_type_lex.top();
        st_type_lex.pop();

        if(op1 != LEX_INT){
            throw "Error: Bad types of operand";
        }
        st_type_lex.push(LEX_INT);
        poliz.push_back(Lex(LEX_NOT));
    }

    void PR();
    void D1();
    void O1();
    void D();
    void P();
    void O();
    void O2();
    void OE();
    void EX();
    void E1();
    void E2();
    void E3();
    void E4();
    void T();
    void F();

    type_lex gl(){
        if(!flag){
            cur_lex = scan.get_lex();
        }else{
            flag = false;
            cur_lex = buf_lex;
        }
        cur_type = cur_lex.get_type();
        cur_val = cur_lex.get_val();
        if(fl_test) cout << cur_lex;
        return cur_type;
    } 

public:
    Parser(const string name_prog):scan(name_prog){
        flag = false;
    }

    void analyze(){
        gl();
        PR();
        if(fl_test) cout << cur_type << endl;
        if(cur_type == LEX_FIN){
            cout << "Compiled program" << endl;
        }else{
            throw "You can't place anything after program";
        }
        poliz.push_back(Lex(LEX_FIN));

        /*cout << endl;
        while(!st_type_lex.empty()){
            cout << st_type_lex.top() << endl;
            st_type_lex.pop();
        }*/
    }


};

void Parser::PR(){
    if(fl_test) cout << "PR ";
    if(cur_type != LEX_PROGRAM){
        throw "Error: wrong begin of program";
    }
    if(gl() != LEX_LBRACK){
        throw cur_lex;
    }
    gl();
    D1();
    O1();
    if(cur_type != LEX_RBRACK){
        throw "Error: wrong end of program";
    }
    gl();
    gl();
}

void Parser::D1(){
    if(fl_test) cout << "D1 ";
    while((cur_type == LEX_INT) || (cur_type == LEX_STRING)){
        D();
        if(cur_type != LEX_SEMICOLON){
            throw cur_lex;
        }
        gl();
    }
}

void Parser::D(){
    if(fl_test) cout << "D ";
    if((cur_type != LEX_INT) && (cur_type != LEX_STRING)){
        throw cur_lex;
    }
    
    seman_type = cur_type;

    gl();
    P();
    while(cur_type == LEX_COMMA){
        gl();
        P();
    }
}

void Parser::P(){
    if(fl_test) cout << "P ";
    if(cur_type != LEX_ID){
        throw cur_lex;
    }

    TID[cur_val].set_type((type_id) seman_type);

    if(gl() == LEX_ASSIGN){
        gl();
        if((cur_type != LEX_STR) && (cur_type != LEX_NUM)){
            throw cur_lex;
        }
        if((cur_type != LEX_STRING && seman_type == LEX_STR) || (cur_type != LEX_NUM && seman_type == LEX_INT)){
            throw "Error: bad type of const in initialization";
        }
        if(cur_type == LEX_NUM){
            TID[TID.size() - 1].set_val(cur_val);
        }else{
            TID[TID.size() - 1].set_val(TSTR[cur_val]);
        }
        
        gl();
    }
}

void Parser::O1(){
    if(fl_test) cout << "O1 ";
    while((cur_type == LEX_IF) || (cur_type == LEX_WHILE) || (cur_type == LEX_READ) || (cur_type == LEX_WRITE) || 
    (cur_type == LEX_LBRACK) || (cur_type == LEX_LPAREN) || (cur_type == LEX_ID) || (cur_type == LEX_INT) || 
    (cur_type == LEX_STR) || (cur_type == LEX_NOT) || (cur_type == LEX_SEMICOLON)){
        O();
    }
}

void Parser::O(){
    if(fl_test) cout << "O ";
    switch(cur_type){
        case LEX_IF:{
            if(gl() != LEX_LPAREN){
                throw cur_lex;
            }
            gl();
            EX();
            if(st_type_lex.top() != LEX_INT){
                throw "Bad type in operator if";
            }
            st_type_lex.pop();
            int label_false = poliz.size();
            poliz.push_back(Lex(POLIZ_LABEL));
            poliz.push_back(Lex(POLIZ_FGO));
            if(cur_type != LEX_RPAREN){
                throw cur_lex;
            }
            gl();
            O();
            if(cur_type == LEX_ELSE){
                int label_true = poliz.size();
                poliz.push_back(Lex(POLIZ_LABEL));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[label_false].set_val(poliz.size());
                gl();
                O();
                poliz[label_true].set_val(poliz.size());
            }else{
                poliz[label_false].set_val(poliz.size());
            }
            break;
        }

        case LEX_WHILE:{
            int label_beg = poliz.size();
            if(gl() != LEX_LPAREN){
                throw cur_lex;
            }
            gl();
            EX();
            if(st_type_lex.top() != LEX_INT){
                throw "Bad type in operator while";
            }
            st_type_lex.pop();
            int label_false = poliz.size();
            poliz.push_back(Lex(POLIZ_LABEL));
            poliz.push_back(Lex(POLIZ_FGO));
            if(cur_type != LEX_RPAREN){
                throw cur_lex;
            }
            gl();
            O();
            poliz.push_back(Lex(POLIZ_LABEL, label_beg));
            poliz.push_back(Lex(POLIZ_GO));
            poliz[label_false].set_val(poliz.size());
            break;
        }

        case LEX_READ:{
            if(gl() != LEX_LPAREN){
                throw cur_lex;
            }
            if(gl() != LEX_ID){
                throw cur_lex;
            }
            poliz.push_back(Lex(POLIZ_ADDRESS, cur_val));
            if(gl() != LEX_RPAREN){
                throw cur_lex;
            }
            if(gl() != LEX_SEMICOLON){
                throw cur_lex;
            }
            poliz.push_back(Lex(LEX_READ));
            gl();
            break;
        }

        case LEX_WRITE:{
            if(gl() != LEX_LPAREN){
                throw cur_lex;
            }
            gl();
            EX();
            st_type_lex.pop();
            while(cur_type == LEX_COMMA){
                gl();
                EX();
                st_type_lex.pop();
            }
            poliz.push_back(Lex(LEX_WRITE));
            if(cur_type != LEX_RPAREN){
                throw cur_lex;
            }
            if(gl() != LEX_SEMICOLON){
                throw cur_lex;
            }
            gl();
            break;
        }

        case LEX_LBRACK:{
            O2();
            break;
        }

        case LEX_NOT: case LEX_LPAREN: case LEX_ID:
        case LEX_STR: case LEX_INT: case LEX_SEMICOLON: {
            OE();
            break;
        }

        default: throw cur_lex;
    }
}

void Parser::O2(){
    if(fl_test) cout << "O2 ";
    if(cur_type != LEX_LBRACK){
        throw cur_lex;
    }
    gl();
    O1();
    if(cur_type != LEX_RBRACK){
        throw cur_lex;
    }
    gl();
}

void Parser::OE(){
    if(fl_test) cout << "OE ";
    if(cur_type != LEX_SEMICOLON){
        EX();
        if(cur_type != LEX_SEMICOLON){
            throw cur_lex;
        }
        st_type_lex.pop();
        poliz.push_back(Lex(LEX_SEMICOLON));
    }
    gl();
}

void Parser::EX(){
    if(fl_test) cout << "EX ";
    if(cur_type == LEX_ID){
        Lex Id = cur_lex;
        if(gl() == LEX_ASSIGN){
            st_type_lex.push((type_lex) TID[Id.get_val()].get_type());
            st_type_lex.push(LEX_ASSIGN);
            poliz.push_back(Lex(POLIZ_ADDRESS, Id.get_val()));
            gl();
            EX();
            check_two_op();
            return;
        }
        flag = true;
        buf_lex = cur_lex;
        cur_lex = Id;
        cur_type = Id.get_type();
        cur_val = Id.get_val();
    }
    E1();
    while(cur_type == LEX_OR){
        st_type_lex.push(cur_type);
        gl();
        E1();
        check_two_op();
    }
}

void Parser::E1(){
    if(fl_test) cout << "E1 ";
    E2();
    while(cur_type == LEX_AND){
        st_type_lex.push(LEX_AND);
        gl();
        E2();
        check_two_op();
    }
}

void Parser::E2(){
    if(fl_test) cout << "E2 ";
    E3();
    while((cur_type == LEX_EQ) || (cur_type == LEX_NEQ)){
        st_type_lex.push(cur_type);
        gl();
        E3();
        check_two_op();
    }
}

void Parser::E3(){
    if(fl_test) cout << "E3 ";
    E4();
    while((cur_type == LEX_LEQ) || (cur_type == LEX_GEQ) || (cur_type == LEX_GTR) || (cur_type == LEX_LSS)){
        st_type_lex.push(cur_type);
        gl();
        E4();
        check_two_op();
    }
}

void Parser::E4(){
    if(fl_test) cout << "E4 ";
    T();
    while((cur_type == LEX_PLUS) || (cur_type == LEX_MINUS)){
        st_type_lex.push(cur_type);
        gl();
        T();
        check_two_op();
    }
}

void Parser::T(){
    if(fl_test) cout << "T ";
    F();
    while((cur_type == LEX_MUL) || (cur_type == LEX_SLASH)){
        st_type_lex.push(cur_type);
        gl();
        F();
        check_two_op();
    }
}

void Parser::F(){
    if(fl_test) cout << "F ";
    switch(cur_type){
        case LEX_ID: {
            st_type_lex.push((type_lex) TID[cur_val].get_type()); 
            poliz.push_back(cur_lex);
            gl(); 
            return;
        }
        case LEX_NUM: {
            st_type_lex.push(LEX_INT); 
            poliz.push_back(cur_lex);
            gl(); 
            return;
        }
        case LEX_STR: {
            st_type_lex.push(LEX_STRING); 
            poliz.push_back(cur_lex);
            gl(); 
            return;
        }
        case LEX_NOT: {
            st_type_lex.push(cur_type);
            gl(); 
            F(); 
            check_one_op(); 
            return;
        }
        case LEX_LPAREN:{
            gl();
            EX();
            if(cur_type != LEX_RPAREN){
                throw cur_lex;
            }
            gl();
            return;
        }
        default: throw cur_lex;
    }
}


struct perem{
    type_id type;
    int val_int;
    string val_str;
};

void from_st(stack<Lex>& st, perem& per){
    if(st.top().get_type() == LEX_NUM || st.top().get_type() == POLIZ_LABEL){
        per.type = T_INT;
        per.val_int = st.top().get_val();
    }else if(st.top().get_type() == LEX_ID){
        per.type = TID[st.top().get_val()].get_type();
        if(per.type == T_INT){
            per.val_int = TID[st.top().get_val()].get_val(0);
        }else{
            per.val_str = TID[st.top().get_val()].get_val(string());
        }
    }else{
        per.type = T_STR;
        per.val_str = TSTR[st.top().get_val()];
    }
    st.pop();
}

void translator(){
    int cur_pos = 0;
    stack <Lex> st_per;
    while(poliz[cur_pos].get_type() != LEX_FIN){
        switch(poliz[cur_pos].get_type()){
            case LEX_ID: case LEX_STR: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:{
                st_per.push(poliz[cur_pos]);
                break;
            }
            case LEX_NOT:{
                perem cur;
                from_st(st_per, cur);
                st_per.push(Lex(LEX_NUM, !cur.val_int));
                break;
            }
            case LEX_AND:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                st_per.push(Lex(LEX_NUM, cur1.val_int && cur2.val_int));
                break;
            }
            case LEX_OR:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                st_per.push(Lex(LEX_NUM, cur1.val_int || cur2.val_int));
                break;
            }
            case LEX_MINUS:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                st_per.push(Lex(LEX_NUM, cur1.val_int - cur2.val_int));
                break;
            }
            case LEX_MUL:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                st_per.push(Lex(LEX_NUM, cur1.val_int * cur2.val_int));
                break;
            }
            case LEX_SLASH:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                st_per.push(Lex(LEX_NUM, cur1.val_int / cur2.val_int));
                break;
            }
            case LEX_ASSIGN:{
                perem cur2;
                from_st(st_per, cur2);
                if(cur2.type == T_INT){
                    TID[st_per.top().get_val()].set_val(cur2.val_int);
                }else{
                    TID[st_per.top().get_val()].set_val(cur2.val_str);
                }
                st_per.top().set_type(LEX_ID);
                break;
            }
            case LEX_SEMICOLON:{
                st_per.pop();
                break;
            }
            case LEX_WRITE:{
                vector<perem> mas;
                perem cur;
                while(!st_per.empty()){
                    from_st(st_per, cur);
                    mas.push_back(cur);
                }
                for(int i = mas.size() - 1; i >= 0; i--){
                    if(mas[i].type == T_INT){
                        cout << mas[i].val_int << " ";
                    }else{
                        cout << mas[i].val_str << " ";
                    }
                }
                cout << endl;
                break;
            }
            case LEX_READ:{
                if(TID[st_per.top().get_val()].get_type() == T_STR){
                    string cur;
                    cin >> cur;
                    TID[st_per.top().get_val()].set_val(cur);
                }else{
                    int cur;
                    cin >> cur;
                    TID[st_per.top().get_val()].set_val(cur);
                }
                st_per.pop();
                break;
            }
            case LEX_PLUS:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                
                if(cur1.type == T_STR){
                    cur1.val_str = cur1.val_str + cur2.val_str;
                    vector<string>::iterator k = find(TSTR.begin(), TSTR.end(), cur1.val_str);
                    if(k == TSTR.end()){
                        TSTR.push_back(cur1.val_str);
                        st_per.push(Lex(LEX_STR, TSTR.size() - 1));
                    }else{
                        st_per.push(Lex(LEX_STR, distance(TSTR.begin(), k)));
                    }
                }else{
                    st_per.push(Lex(LEX_NUM, cur1.val_int + cur2.val_int));
                }
                break;
            }
            case LEX_LSS: case LEX_EQ: case LEX_NEQ: case LEX_LEQ: 
            case LEX_GEQ: case LEX_GTR:{
                perem cur1, cur2;
                from_st(st_per, cur2);
                from_st(st_per, cur1);
                if(cur1.type == T_STR){                 
                    switch(poliz[cur_pos].get_type()){
                        case LEX_LSS: st_per.push(Lex(LEX_NUM, cur1.val_str < cur2.val_str)); break;
                        case LEX_LEQ: st_per.push(Lex(LEX_NUM, cur1.val_str <= cur2.val_str)); break;
                        case LEX_GTR: st_per.push(Lex(LEX_NUM, cur1.val_str > cur2.val_str)); break;
                        case LEX_GEQ: st_per.push(Lex(LEX_NUM, cur1.val_str >= cur2.val_str)); break;
                        case LEX_EQ: st_per.push(Lex(LEX_NUM, cur1.val_str == cur2.val_str)); break;
                        case LEX_NEQ: st_per.push(Lex(LEX_NUM, cur1.val_str != cur2.val_str));
                    }
                }else{
                    switch(poliz[cur_pos].get_type()){
                        case LEX_LSS: st_per.push(Lex(LEX_NUM, cur1.val_int < cur2.val_int)); break;
                        case LEX_LEQ: st_per.push(Lex(LEX_NUM, cur1.val_int <= cur2.val_int)); break;
                        case LEX_GTR: st_per.push(Lex(LEX_NUM, cur1.val_int > cur2.val_int)); break;
                        case LEX_GEQ: st_per.push(Lex(LEX_NUM, cur1.val_int >= cur2.val_int)); break;
                        case LEX_EQ: st_per.push(Lex(LEX_NUM, cur1.val_int == cur2.val_int)); break;
                        case LEX_NEQ: st_per.push(Lex(LEX_NUM, cur1.val_int != cur2.val_int));
                    }
                }
                break;
            }
            case POLIZ_GO:{
                perem label;
                from_st(st_per, label);
                cur_pos = label.val_int - 1;
                break;
            }
            case POLIZ_FGO:{
                perem label, cur;
                from_st(st_per, label);
                from_st(st_per, cur);
                if(!cur.val_int){
                    cur_pos = label.val_int - 1;
                }
                break;
            }
            default: cout << "ЗАБЫЛ ЛЕКСЕМУ " << poliz[cur_pos];
        }
        //cout << poliz[cur_pos].get_type() << "       " << st_per.size() << "    " << (!st_per.empty()?st_per.top():(Lex(LEX_NULL)));
        cur_pos++;
        
    }
}


int main(int argc, char** argv){
    try{
        Parser par(argv[1]);
        par.analyze();

        if(fl_test){
            cout << endl << "TID" << endl;
            for(int i = 0; i < TID.size(); i++){
                int j;
                string s;
                if(TID[i].get_type() == T_STR){
                    cout << TID[i].get_name() << "     " << TID[i].get_val(s) << endl;
                }else{
                    cout << TID[i].get_name() << "     " << TID[i].get_val(0) << endl;
                }
            }
            cout << endl << "TSTR" << endl;
            for(int i = 0; i < TSTR.size(); i++){
                cout << TSTR[i] << endl;
            }
            cout << endl << "poliz" << endl;
            for(int i = 0; i < poliz.size(); i++){
                cout << i << ": ";
                if((poliz[i].get_type() > 0) && (poliz[i].get_type() < 29)){
                    cout << TS[poliz[i].get_type()] << "   ";
                }else if(poliz[i].get_type() == LEX_NUM || poliz[i].get_type() == POLIZ_LABEL){
                    cout << poliz[i].get_val() << "   ";
                }else if(poliz[i].get_type() == LEX_STR){
                    cout << '\"' << TSTR[poliz[i].get_val()] << "\"   ";
                }else if(poliz[i].get_type() == LEX_ID){
                    cout << TID[poliz[i].get_val()].get_name() << "   ";
                }else if(poliz[i].get_type() == POLIZ_ADDRESS){
                    cout << '&' << TID[poliz[i].get_val()].get_name() << "   ";
                }else if(poliz[i].get_type() == POLIZ_GO){
                    cout << "!   ";
                }else if(poliz[i].get_type() == POLIZ_FGO){
                    cout << "!F   ";
                }
            }
            cout << endl << endl << endl;
        }

        translator();

        if(fl_test){
            cout << endl << "TSTR" << endl;
            for(int i = 0; i < TSTR.size(); i++){
                cout << TSTR[i] << endl;
            }

            cout << endl << "TID" << endl;
            for(int i = 0; i < TID.size(); i++){
                int j;
                string s;
                if(TID[i].get_type() == T_STR){
                    cout << TID[i].get_name() << "     " << TID[i].get_val(s) << endl;
                }else{
                    cout << TID[i].get_name() << "     " << TID[i].get_val(0) << endl;
                }
            }
        }
    }
    catch(const char* s){
        cout << s << endl;
        return 1;
    }
    catch(string s){
        cout << s << endl;
        return 1;
    }
    catch(char c){
        cout << "Error: undefined symbol " << c << endl;
        return 1;
    }
    catch(Lex& l){
        cout << "Error: unexpected lexeme " << l << endl;
        return 1;
    }
    return 0;
}

