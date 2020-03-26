#include "parser/json.hpp"


namespace JSON{

//text check function
bool check_char(char c,const std::string& text,size_t offset){
    if (offset >= 0 && offset < text.size()){
        if (text[offset] == c){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool check_str(const std::string& s,const std::string& text,size_t offset){
    if (offset >= 0 && offset+s.size() <= text.size()){
        for (size_t i = 0;i < s.size();i++){
            if (s[i] != text[offset+i]){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

bool check_digit(const std::string& text,size_t offset){
    if (offset >= 0 && offset < text.size()){
        if (text[offset] >= '0' && text[offset] <= '9'){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}





//skip function
bool skip_control_char(const std::string& text,size_t& offset){
    if (check_char('\f',text,offset) || check_char('\n',text,offset) ||
        check_char('\r',text,offset) || check_char('\t',text,offset) ||
        check_char(' ',text,offset))
    {
        offset++;
        return true;
    }else{
        return false;
    }
}





//tokenize function
bool read_punctuator(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    if (check_char('{',text,offset) || check_char('}',text,offset) ||
        check_char('[',text,offset) || check_char(']',text,offset) ||
        check_char(':',text,offset) || check_char(',',text,offset))
    {
        Token token;
        token.type = PUNCTUATOR_TOKEN;
        token.data = text[offset];
        tokens.push_back(token);
        offset++;
        return true;
    }else{
        return false;
    }
}

bool read_escape_sequence(char& buffer,const std::string& text,size_t& offset){
    if (!check_char('\\',text,offset)){
        return false;
    }
    if (check_char('\"',text,offset+1) ||
        check_char('\\',text,offset+1) ||
        check_char('/',text,offset+1))
    {
        buffer = text[offset+1];
        offset += 2;
        return true;
    }else if (check_char('b',text,offset+1)){
        buffer = '\b';
        offset += 2;
        return true;
    }else if (check_char('f',text,offset+1)){
        buffer = '\f';
        offset += 2;
        return true;
    }else if (check_char('n',text,offset+1)){
        buffer = '\n';
        offset += 2;
        return true;
    }else if (check_char('r',text,offset+1)){
        buffer = '\r';
        offset += 2;
        return true;
    }else if (check_char('t',text,offset+1)){
        buffer = '\t';
        offset += 2;
        return true;
    }else{
        return false;
    }
}

bool read_s_char(char& buffer,const std::string& text,size_t& offset){
    if (!check_char('\"',text,offset) && !check_char('\\',text,offset)){
        buffer = text[offset];
        offset++;
        return true;
    }else{
        return read_escape_sequence(buffer,text,offset);
    }
}

bool read_s_char_sequence(std::string& buffer,const std::string& text,size_t& offset){
    char c;
    if (read_s_char(c,text,offset)){
        buffer += c;
        while (read_s_char(c,text,offset)){
            buffer += c;
        }
        return true;
    }else{
        return false;
    }
}

bool read_string(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    //tmp data
    Token t_token;
    size_t t_offset = offset;

    //read "
    if (check_char('\"',text,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //read s_char
    read_s_char_sequence(t_token.data,text,t_offset);

    //read "
    if (check_char('\"',text,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //success
    t_token.type = STRING_TOKEN;
    tokens.push_back(t_token);
    offset = t_offset;
    return true;
}


bool read_number(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    //tmp data
    Token t_token;
    size_t t_offset = offset;

    //read sign part
    if (check_char('+',text,t_offset) || check_char('-',text,t_offset)){
        t_token.data += text[t_offset];
        t_offset++;
    }

    //read integer part
    if (check_digit(text,t_offset)){
        if (text[t_offset] == '0'){
            t_token.data += text[t_offset];
            t_offset++;
        }else{
            while (check_digit(text,t_offset)){
                t_token.data += text[t_offset];
                t_offset++;
            }
        }
    }else{
        return false;
    }

    //read decimal part
    if (check_char('.',text,t_offset)){
        t_token.data += text[t_offset];
        t_offset++;
        //read digit
        if (check_digit(text,t_offset)){
            while (check_digit(text,t_offset)){
                t_token.data += text[t_offset];
                t_offset++;
            }
        }else{
            return false;
        }
    }

    //read exponent part
    if (check_char('e',text,t_offset) || check_char('E',text,t_offset)){
        t_token.data += text[t_offset];
        t_offset++;

        //read sign
        if (check_char('+',text,t_offset) || check_char('-',text,t_offset)){
            t_token.data += text[t_offset];
            t_offset++;
        }

        //read digit
        if (check_digit(text,t_offset)){
            while (check_digit(text,t_offset)){
                t_token.data += text[t_offset];
                t_offset++;
            }
        }else{
            return false;
        }
    }

    //success
    t_token.type = NUMBER_TOKEN;
    tokens.push_back(t_token);
    offset = t_offset;
    return true;
}

bool read_boolean(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    //true
    if (check_str("true",text,offset)){
        Token token;
        token.type = BOOLEAN_TOKEN;
        token.data = "true";
        tokens.push_back(token);
        offset += 4;
        return true;
    }

    //false
    if (check_str("false",text,offset)){
        Token token;
        token.type = BOOLEAN_TOKEN;
        token.data = "false";
        tokens.push_back(token);
        offset += 5;
        return true;
    }

    //error
    return false;
}

bool read_null(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    if (check_str("null",text,offset)){
        Token token;
        token.type = NULL_TOKEN;
        token.data = "null";
        tokens.push_back(token);
        offset += 4;
        return true;
    }else{
        return false;
    }
}

void tokenize(std::vector<Token>& tokens,const std::string& text){
    //offset
    size_t offset = 0;

    //tokenize
    while (offset < text.size()){
        //skip control character
        if (skip_control_char(text,offset)){
            continue;
        }

        //read punctuator
        if (read_punctuator(tokens,text,offset)){
            continue;
        }

        //read number
        if (read_number(tokens,text,offset)){
            continue;
        }

        //read string
        if (read_string(tokens,text,offset)){
            continue;
        }

        //read boolean
        if (read_boolean(tokens,text,offset)){
            continue;
        }

        //read null
        if (read_null(tokens,text,offset)){
            continue;
        }

        //error
        std::cout << "json.cpp:syntax error/failed to tokenize json file" << "\n";
        std::terminate();
    }
}





//token check function
bool check_token_type(TokenType type,const std::vector<Token>& tokens,size_t offset){
    if (offset >= 0 && offset < tokens.size()){
        if (tokens[offset].type == type){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool check_token_data(const std::string& data,const std::vector<Token>& tokens,size_t offset){
    if (offset >= 0 && offset < tokens.size()){
        if (tokens[offset].data == data){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}





//parse function
bool read_value(Node* node,const std::vector<Token>& tokens,size_t& offset);
bool read_object(Node* node,const std::vector<Token>& tokens,size_t& offset);
bool read_array(Node* node,const std::vector<Token>& tokens,size_t& offset);

bool read_value(Node* node,const std::vector<Token>& tokens,size_t& offset){
    //null check
    if (node == nullptr) return false;

    //read object
    if (read_object(node,tokens,offset)){
        return true;
    }

    //read array
    if (read_array(node,tokens,offset)){
        return true;
    }

    //read number
    if (check_token_type(NUMBER_TOKEN,tokens,offset)){
        node->type = NUMBER_NODE;
        node->data = tokens[offset].data;
        offset++;
        return true;
    }

    //read string
    if (check_token_type(STRING_TOKEN,tokens,offset)){
        node->type = STRING_NODE;
        node->data = tokens[offset].data;
        offset++;
        return true;
    }

    //read boolean
    if (check_token_type(BOOLEAN_TOKEN,tokens,offset)){
        node->type = BOOLEAN_NODE;
        node->data = tokens[offset].data;
        offset++;
        return true;
    }

    //read null
    if (check_token_type(NULL_TOKEN,tokens,offset)){
        node->type = NULL_NODE;
        node->data = tokens[offset].data;
        offset++;
        return true;
    }

    //error
    return false;
}

bool read_object(Node* node,const std::vector<Token>& tokens,size_t& offset){
    //null check
    if (node == nullptr) return false;

    //tmp data
    Node* t_node = new Node();
    size_t t_offset = offset;

    //read "{"
    if (check_token_data("{",tokens,t_offset)){
        t_offset++;
    }else{
        delete t_node;
        return false;
    }

    //read pairs
    while (t_offset < tokens.size()){
        //read string token
        std::string key;
        if (check_token_type(STRING_TOKEN,tokens,t_offset)){
            key = tokens[t_offset].data;
            t_offset++;
        }else{
            break;
        }

        //read ":"
        if (check_token_data(":",tokens,t_offset)){
            t_offset++;
        }else{
            break;
        }

        //read value
        Node* child = new Node();
        if (read_value(child,tokens,t_offset)){
            t_node->members[key] = child;
        }else{
            delete child;
            break;
        }

        //read ","
        if (check_token_data(",",tokens,t_offset)){
            if (!check_token_data("}",tokens,t_offset+1)){
                t_offset++;
            }else{
                break;
            }
        }else{
            break;
        }
    }

    //read "}"
    if (check_token_data("}",tokens,t_offset)){
        t_offset++;
    }else{
        release(t_node);
        return false;
    }

    //success
    t_node->type = OBJECT_NODE;
    *node = *t_node;
    offset = t_offset;
    delete t_node;
    return true;
}

bool read_array(Node* node,const std::vector<Token>& tokens,size_t& offset){
    //null check
    if (node == nullptr) return false;

    //tmp data
    Node* t_node = new Node();
    size_t t_offset = offset;

    //read "["
    if (check_token_data("[",tokens,t_offset)){
        t_offset++;
    }else{
        delete t_node;
        return false;
    }

    //read values
    while (t_offset < tokens.size()){
        //read value
        Node* child = new Node();
        if (read_value(child,tokens,t_offset)){
            t_node->elements.push_back(child);
        }else{
            delete child;
            break;
        }

        //read ","
        if (check_token_data(",",tokens,t_offset)){
            if (!check_token_data("]",tokens,t_offset+1)){
                t_offset++;
            }else{
                break;
            }
        }else{
            break;
        }
    }

    //read "]"
    if (check_token_data("]",tokens,t_offset)){
        t_offset++;
    }else{
        release(t_node);
        return false;
    }

    //success
    t_node->type = ARRAY_NODE;
    *node = *t_node;
    offset = t_offset;
    delete t_node;
    return true;
}





//data writing function
void write_value(std::string& text,const Node* node,size_t tab_count){
    //null check
    if (node == nullptr) return;

    //write
    //object
    if (node->type == OBJECT_NODE){
        //create tabs
        std::string tabs;
        for (size_t i = 0;i < tab_count;i++){
            tabs += "\t";
        }

        text += "{\n";
        for (auto i = node->members.begin();i != node->members.end();++i){
            if (i != node->members.begin()){
                text += ",\n";
            }
            text += tabs+"\t";
            text += "\""+i->first+"\""+":";
            write_value(text,i->second,tab_count+1);
        }
        text += "\n"+tabs+"}";
        return;
    }

    //array
    if (node->type == ARRAY_NODE){
        text += "[";
        for (size_t i = 0;i < node->elements.size();i++){
            write_value(text,node->elements[i],tab_count+1);
            text += ",";
        }
        if (text.back() == ','){
            text.pop_back();
        }
        text += "]";
        return;
    }

    //string
    if (node->type == STRING_NODE){
        text += "\""+node->data+"\"";
        return;
    }

    //number,boolean
    if (node->type == NUMBER_NODE || node->type == BOOLEAN_NODE){
        text += node->data;
        return;
    }

    //null
    if (node->type == NULL_NODE){
        text += "null";
        return;
    }
}




//user call function
Node* read(const std::string& path){
    //read file
    std::string text;
    read_file(text,path);

    //tokenize
    std::vector<Token> tokens;
    tokenize(tokens,text);

    //parse
    Node* node = new Node();
    size_t offset = 0;
    if (!read_value(node,tokens,offset)){
        std::cout << "json.cpp:syntax error/failed to parse json file:" << path << "\n";
        std::terminate();
    }
    return node;
}

void release(Node* node){
    //null check
    if (node == nullptr) return;

    //delete child
    for (auto i = node->members.begin();i != node->members.end();++i){
        release(i->second);
    }
    for (size_t i = 0;i < node->elements.size();i++){
        release(node->elements[i]);
    }

    //delete parent
    delete node;
}

void print(const Node* node){
    std::string text;
    write_value(text,node,0);
    std::cout << text << "\n";
}


}// namespace JSON
