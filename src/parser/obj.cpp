#include "parser/obj.hpp"

namespace OBJ{

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

bool check_nondigit(const std::string& text,size_t offset){
    if (offset >= 0 && offset < text.size()){
        if ((text[offset] >= 'a' && text[offset] <= 'z') ||
            (text[offset] >= 'A' && text[offset] <= 'Z') ||
             text[offset] == '_')
        {
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

//skip comment
bool skip_comment(const std::string& text,size_t& offset){
    //read '#'
    if (check_char('#',text,offset)){
        offset++;
    }else{
        return false;
    }

    //read character until '\n'
    while (offset < text.size()){
        if (!check_char('\n',text,offset)){
            offset++;
        }else{
            offset++;
            break;
        }
    }

    //success
    return true;
}





//tokenize function
bool read_punctuator(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    if (check_char('/',text,offset)){
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

bool read_identifier(std::vector<Token>& tokens,const std::string& text,size_t& offset){
    //read nondigit
    Token token;
    if (check_nondigit(text,offset)){
        token.data += text[offset];
        offset++;
    }else{
        return false;
    }

    //read digit or nondigit
    while (check_digit(text,offset) || check_nondigit(text,offset)){
        token.data += text[offset];
        offset++;
    }

    //success
    token.type = IDENTIFIER_TOKEN;
    tokens.push_back(token);
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

void tokenize(std::vector<Token>& tokens,const std::string& text){
    //offset
    size_t offset = 0;

    //tokenize
    while (offset < text.size()){
        //skip control character
        if (skip_control_char(text,offset)){
            continue;
        }

        //skip comment
        if (skip_comment(text,offset)){
            continue;
        }

        //read punctuator
        if (read_punctuator(tokens,text,offset)){
            continue;
        }

        //read identifier
        if (read_identifier(tokens,text,offset)){
            continue;
        }

        //read number
        if (read_number(tokens,text,offset)){
            continue;
        }

        //error
        //skip this character
        offset++;
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
bool read_line_v(Data& data,const std::vector<Token>& tokens,size_t& offset){
    //tmp data
    std::array<FLOAT,3> t_array;
    size_t t_offset = offset;

    //read "v"
    if (check_token_data("v",tokens,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //read x,y,z
    for (uint8_t i = 0;i < 3;i++){
        if (check_token_type(NUMBER_TOKEN,tokens,t_offset)){
            t_array[i] = (FLOAT)std::stod(tokens[t_offset].data);
            t_offset++;
        }else{
            return false;
        }
    }

    //success
    data.v.push_back(t_array);
    offset = t_offset;
    return true;
}

bool read_line_vt(Data& data,const std::vector<Token>& tokens,size_t& offset){
    //tmp data
    std::array<FLOAT,2> t_array;
    size_t t_offset = offset;

    //read "vt"
    if (check_token_data("vt",tokens,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //read x,y
    for (uint8_t i = 0;i < 2;i++){
        if (check_token_type(NUMBER_TOKEN,tokens,t_offset)){
            t_array[i] = (FLOAT)std::stod(tokens[t_offset].data);
            t_offset++;
        }else{
            return false;
        }
    }

    //success
    data.vt.push_back(t_array);
    offset = t_offset;
    return true;
}

bool read_line_vn(Data& data,const std::vector<Token>& tokens,size_t& offset){
    //tmp data
    std::array<FLOAT,3> t_array;
    size_t t_offset = offset;

    //read "vn"
    if (check_token_data("vn",tokens,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //read x,y,z
    for (uint8_t i = 0;i < 3;i++){
        if (check_token_type(NUMBER_TOKEN,tokens,t_offset)){
            t_array[i] = (FLOAT)std::stod(tokens[t_offset].data);
            t_offset++;
        }else{
            return false;
        }
    }

    //success
    data.vn.push_back(t_array);
    offset = t_offset;
    return true;
}

bool check_line_f_case1(const std::vector<Token>& tokens,size_t offset){
    //case1:n n...n
    return check_token_type(NUMBER_TOKEN,tokens,offset) &&
           !check_token_data("/",tokens,offset+1);
}

bool check_line_f_case2(const std::vector<Token>& tokens,size_t offset){
    //case2:n/n n/n...n/n
    return check_token_type(NUMBER_TOKEN,tokens,offset) &&
           check_token_data("/",tokens,offset+1) &&
           check_token_type(NUMBER_TOKEN,tokens,offset+2);
}

bool check_line_f_case3(const std::vector<Token>& tokens,size_t offset){
    //case3:n//n n//n...n//n
    return check_token_type(NUMBER_TOKEN,tokens,offset) &&
           check_token_data("/",tokens,offset+1) &&
           check_token_data("/",tokens,offset+2) &&
           check_token_type(NUMBER_TOKEN,tokens,offset+3);
}

bool check_line_f_case4(const std::vector<Token>& tokens,size_t offset){
    //case4:n/n/n n/n/n...n/n/n
    return check_token_type(NUMBER_TOKEN,tokens,offset) &&
           check_token_data("/",tokens,offset+1) &&
           check_token_type(NUMBER_TOKEN,tokens,offset+2) &&
           check_token_data("/",tokens,offset+3) &&
           check_token_type(NUMBER_TOKEN,tokens,offset+4);
}

bool read_line_f(Data& data,const std::vector<Token>& tokens,size_t& offset){
    //tmp data
    Face t_face;
    size_t t_offset = offset;

    //read "f"
    if (check_token_data("f",tokens,t_offset)){
        t_offset++;
    }else{
        return false;
    }

    //read idx
    if (check_line_f_case1(tokens,t_offset)){
        while (check_line_f_case1(tokens,t_offset)){
            t_face.v_idx.push_back((size_t)std::stoul(tokens[t_offset].data)-1);
            //t_face.vt_idx.push_back(-1);
            //t_face.vn_idx.push_back(-1);
            t_offset += 1;
        }
    }else if (check_line_f_case2(tokens,t_offset)){
        while (check_line_f_case2(tokens,t_offset)){
            t_face.v_idx.push_back((size_t)std::stoul(tokens[t_offset].data)-1);
            t_face.vt_idx.push_back((size_t)std::stoul(tokens[t_offset+2].data)-1);
            //t_face.vn_idx.push_back(-1);
            t_offset += 3;
        }
    }else if (check_line_f_case3(tokens,t_offset)){
        while (check_line_f_case3(tokens,t_offset)){
            t_face.v_idx.push_back((size_t)std::stoul(tokens[t_offset].data)-1);
            //t_face.vt_idx.push_back(-1);
            t_face.vn_idx.push_back((size_t)std::stoul(tokens[t_offset+3].data)-1);
            t_offset += 4;
        }
    }else if (check_line_f_case4(tokens,t_offset)){
        while (check_line_f_case4(tokens,t_offset)){
            t_face.v_idx.push_back((size_t)std::stoul(tokens[t_offset].data)-1);
            t_face.vt_idx.push_back((size_t)std::stoul(tokens[t_offset+2].data)-1);
            t_face.vn_idx.push_back((size_t)std::stoul(tokens[t_offset+4].data)-1);
            t_offset += 5;
        }
    }else{
        return false;
    }

    //count idx size
    if (t_face.v_idx.size() < 3){
        return false;
    }

    //success
    data.f.push_back(t_face);
    offset = t_offset;
    return true;
}

void parse(Data& data,const std::vector<Token>& tokens){
    //offset
    size_t offset = 0;

    //parse
    while (offset < tokens.size()){
        //read line v
        if (read_line_v(data,tokens,offset)){
            continue;
        }

        //read line vt
        if (read_line_vt(data,tokens,offset)){
            continue;
        }

        //read line vn
        if (read_line_vn(data,tokens,offset)){
            continue;
        }

        //read line f
        if (read_line_f(data,tokens,offset)){
            continue;
        }

        //error
        //skip this token
        offset++;
    }
}





//data writing function
void write_data(std::string& text,const Data& data){
    //write v
    for (size_t i = 0;i < data.v.size();i++){
        text += "v";
        text += " ";
        text += std::to_string(data.v[i][0]);
        text += " ";
        text += std::to_string(data.v[i][1]);
        text += " ";
        text += std::to_string(data.v[i][2]);
        text += "\n";
    }

    //wtite vt
    for (size_t i = 0;i < data.vt.size();i++){
        text += "vt";
        text += " ";
        text += std::to_string(data.vt[i][0]);
        text += " ";
        text += std::to_string(data.vt[i][1]);
        text += "\n";
    }

    //write vn
    for (size_t i = 0;i < data.vn.size();i++){
        text += "vn";
        text += " ";
        text += std::to_string(data.vn[i][0]);
        text += " ";
        text += std::to_string(data.vn[i][1]);
        text += " ";
        text += std::to_string(data.vn[i][2]);
        text += "\n";
    }

    //write f
    //case1:f n n...n
    //case2:f n/n n/n...n/n
    //case3:f n//n n//n...n//n
    //case4:f n/n/n n/n/n...n/n/n
    bool case1 = (data.vt.size() == 0)&&(data.vn.size() == 0);
    bool case2 = (data.vt.size() != 0)&&(data.vn.size() == 0);
    bool case3 = (data.vt.size() == 0)&&(data.vn.size() != 0);
    //bool case4 = (data.vt.size() != 0)&&(data.vn.size() != 0);

    if (case1){
        for (size_t i = 0;i < data.f.size();i++){
            text += "f";
            for (size_t j = 0;j < data.f[i].v_idx.size();j++){
                text += " ";
                text += std::to_string(data.f[i].v_idx[j]+1);
            }
            text += "\n";
        }
    }else if (case2){
        for (size_t i = 0;i < data.f.size();i++){
            text += "f";
            for (size_t j = 0;j < data.f[i].v_idx.size();j++){
                text += " ";
                text += std::to_string(data.f[i].v_idx[j]+1);
                text += "/";
                text += std::to_string(data.f[i].vt_idx[j]+1);
            }
            text += "\n";
        }
    }else if (case3){
        for (size_t i = 0;i < data.f.size();i++){
            text += "f";
            for (size_t j = 0;j < data.f[i].v_idx.size();j++){
                text += " ";
                text += std::to_string(data.f[i].v_idx[j]+1);
                text += "//";
                text += std::to_string(data.f[i].vn_idx[j]+1);
            }
            text += "\n";
        }
    }else{
        for (size_t i = 0;i < data.f.size();i++){
            text += "f";
            for (size_t j = 0;j < data.f[i].v_idx.size();j++){
                text += " ";
                text += std::to_string(data.f[i].v_idx[j]+1);
                text += "/";
                text += std::to_string(data.f[i].vt_idx[j]+1);
                text += "/";
                text += std::to_string(data.f[i].vn_idx[j]+1);
            }
            text += "\n";
        }
    }
}





//function for creating triangles
std::array<FLOAT,3> get_v(const Data& data,size_t idx){
    if (idx >= 0 && idx < data.v.size()){
        return data.v[idx];
    }else{
        return std::array<FLOAT,3>();
    }
}
std::array<FLOAT,2> get_vt(const Data& data,size_t idx){
    if (idx >= 0 && idx < data.vt.size()){
        return data.vt[idx];
    }else{
        return std::array<FLOAT,2>();
    }
}
void create_model(Model& model,const Data& data){
    for (size_t i = 0;i < data.f.size();i++){
        const Face& f = data.f[i];
        if (f.v_idx.size() == f.vt_idx.size()){
            for (size_t j = 1;j <= f.v_idx.size()-2;j++){
                Triangle tri;
                tri.xyz1 = get_v(data,f.v_idx[0]);
                tri.xyz2 = get_v(data,f.v_idx[j]);
                tri.xyz3 = get_v(data,f.v_idx[j+1]);
                tri.uv1 = get_vt(data,f.vt_idx[0]);
                tri.uv2 = get_vt(data,f.vt_idx[j]);
                tri.uv3 = get_vt(data,f.vt_idx[j+1]);
                model.triangles.push_back(tri);
            }
        }else{
            for (size_t j = 1;j <= f.v_idx.size()-2;j++){
                Triangle tri;
                tri.xyz1 = get_v(data,f.v_idx[0]);
                tri.xyz2 = get_v(data,f.v_idx[j]);
                tri.xyz3 = get_v(data,f.v_idx[j+1]);
                tri.uv1 = std::array<FLOAT,2>();
                tri.uv2 = std::array<FLOAT,2>();
                tri.uv3 = std::array<FLOAT,2>();
                model.triangles.push_back(tri);
            }
        }
    }
}





//data check function
void check_data(const Data& data){
    //check v size
    if (data.v.size() == 0){
        std::cout << "obj.cpp:v size must be > 0" << "\n";
        std::terminate();
    }

    //check vt,vn size
    bool vt_exist = (data.vt.size() > 0)? true:false;
    bool vn_exist = (data.vn.size() > 0)? true:false;

    //check f size
    if (data.f.size() == 0){
        std::cout << "obj.cpp:f size must not be zero" << "\n";
        std::terminate();
    }

    //check f index
    for (size_t i = 0;i < data.f.size();i++){
        const Face& face = data.f[i];

        //v_idx
        if (face.v_idx.size() < 3){
            std::cout << "obj.cpp:f's v index size must be >= 3" << "\n";
            std::terminate();
        }
        for (size_t j = 0;j < face.v_idx.size();j++){
            if (!(face.v_idx[j] >= 0 && face.v_idx[j] < data.v.size())){
                std::cout << "obj.cpp:f's v index is out of range" << "\n";
                std::terminate();
            }
        }

        //vt_idx
        if (vt_exist){
            if (face.v_idx.size() != face.vt_idx.size()){
                std::cout << "obj.cpp:f's v,vt index size must be equal" << "\n";
                std::terminate();
            }
            for (size_t j = 0;j < face.vt_idx.size();j++){
                if (!(face.vt_idx[j] >= 0 && face.vt_idx[j] < data.vt.size())){
                    std::cout << "obj.cpp:f's vt index is out of range" << "\n";
                    std::terminate();
                }
            }
        }else{
            if (face.vt_idx.size() != 0){
                std::cout << "obj.cpp:f's vt index is invalid when vt does not exist" << "\n";
                std::terminate();
            }
        }

        //vn_idx
        if (vn_exist){
            if (face.v_idx.size() != face.vn_idx.size()){
                std::cout << "obj.cpp:f's v,vn index size must be equal" << "\n";
                std::terminate();
            }
            for (size_t j = 0;j < face.vn_idx.size();j++){
                if (!(face.vn_idx[j] >= 0 && face.vn_idx[j] < data.vn.size())){
                    std::cout << "obj.cpp:f's vn index is out of range" << "\n";
                    std::terminate();
                }
            }
        }else{
            if (face.vn_idx.size() != 0){
                std::cout << "obj.cpp:f's vn index is invalid when vn does not exist" << "\n";
                std::terminate();
            }
        }
    }
}





//user call function
void read(Data& data,const std::string& path){
    //read file
    std::string text;
    read_file(text,path);

    //end of text must be "\n"
    if (text.back() != '\n'){
        text += '\n';
    }

    //tokenize
    std::vector<Token> tokens;
    tokenize(tokens,text);

    //parse
    parse(data,tokens);

    //check data
    check_data(data);
}

void read(Model& model,const std::string& path){
    //read file
    std::string text;
    read_file(text,path);

    //end of text must be "\n"
    if (text.back() != '\n'){
        text += '\n';
    }

    //tokenize
    std::vector<Token> tokens;
    tokenize(tokens,text);

    //parse
    Data data;
    parse(data,tokens);

    //check data
    check_data(data);

    //create model
    create_model(model,data);
}

void write(const Data& data,const std::string& path){
    //write data
    std::string text;
    write_data(text,data);

    //check data
    check_data(data);

    //write file
    write_file(text,path);
}

void print(const Data& data){
    std::string text;
    write_data(text,data);
    std::cout << text << "\n";
}



}//namespace OBJ
