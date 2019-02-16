#include<iostream>
#include<set>
#include<map>
#include<vector>
using namespace std;

string getstring(string inputstr)
{
string sout;
FILE * pFile;
long lSize;
char * buffer;
size_t result;
pFile = fopen ( inputstr.c_str() , "rb" );
if (pFile==NULL) {fputs ("找不到文件\n",stderr); exit (1);}
// obtain file size:
fseek (pFile , 0 , SEEK_END);
lSize = ftell (pFile);
rewind (pFile);
// allocate memory to contain the whole file:
buffer = (char*) malloc (sizeof(char)*lSize);
if (buffer == NULL) {fputs ("内存错误\n",stderr); exit (2);}
// copy the file into the buffer:
result = fread (buffer,1,lSize,pFile);
if (result != lSize) {fputs ("文件读取错误\n",stderr); exit (3);}
/* the whole file is now loaded in the memory buffer. */
// terminate
sout.assign(buffer,result);
fclose (pFile);
free (buffer);
return sout;}

typedef struct function{
	string name,return_type;
	map<int,map<string,string> > args;
}func;

class token{
public:
	string type,value;
	map<int,token> sub;
	int sub_count=0;
	void print(){
		cout<<this->type<<" "<<this->value<<"\n";
	};
	void add(token input){
		this->sub[this->sub_count]=input;
		this->sub_count++;
	}
};

token init_token(string type,string value){
	token temp;
	temp.type=type,temp.value=value;
	return temp;
}

int p=0,line=1,code_count=0;

string text,onstr,op="+-*/=<>!{}[]%()|.&,",num="0123456789.";

bool is_float;

map<int,string> code;

set<string> type={"int","string","float"};
set<string> keyword={"continue","break","return","else","if","func","while","define","endif","endloop","function","endfunc","print"};
set<string> two_op={"==","!=",">=","<=","||","&&"};

map<string,int> yxj;


map<string,string> vars;

bool isins(string input,wchar_t test_str){
	bool rt=false;
	for(int i=0;i<input.length();i++){
		if(test_str==input[i]){
			rt=true;
			break;
		}
	}
	return rt;
}

void test_var(map<string,string> &table,string var_type,string var_name){
	map<string,string>::iterator iter=table.find(var_name);
	if(iter!=table.end()){
		if(iter->second!=var_type){
			cout<<"第"<<line<<"行：\n错误：变量"<<var_name<<"不允许被二次定义\n";
			exit(1);
		}
	}
	else{
		cout<<"第"<<line<<"行：\n错误：变量"<<var_name<<"未定义\n";
		exit(1);
	}
}

map<int,token> parse_code(){
	int i=0,token_count=0;
	char on_sign;
	map<int,token> temp;
	map<int,token>::iterator iter;
	while(i<text.length()){
		if(text[i]=='\t'){
			i++;
			continue;
		}
		else if(text[i]=='\n'||text[i]==';'||text[i]==' '){
			if(onstr!=""){
				if(keyword.find(onstr)!=keyword.end()){
					temp[token_count]=init_token("keyword",onstr);
				}
				else if(type.find(onstr)!=type.end()){
					temp[token_count]=init_token("type",onstr);
				}
				else{
					temp[token_count]=init_token("var",onstr);
				}
				onstr="";
				token_count++;
			}
			if(text[i]=='\n'||text[i]==';'){
				temp[token_count]=init_token("sep","");
				token_count++;
			}
		}
		else if(isins(num,text[i])){
			if(onstr!=""){
				onstr+=text[i];
				i++;
				continue;
			}
			onstr=text[i];
			while(i<text.length()){
				i++;
				if(!isins(num,text[i])){
					if(is_float){
						is_float=false;
						temp[token_count]=init_token("float",onstr);
					}
					else{
						temp[token_count]=init_token("int",onstr);
					}
					token_count++;
					onstr="";
					break;
				}
				else if(text[i]=='.'){
					is_float=true;
				}
				onstr+=text[i];
			}
			continue;
		}
		else if(isins(op,text[i])){
			iter=temp.find(token_count-1);
			if(onstr!=""){
				if(keyword.find(onstr)!=keyword.end()){
					temp[token_count]=init_token("keyword",onstr);
				}
				else{
					temp[token_count]=init_token("var",onstr);
				}
				token_count++;
				onstr="";
			}
			if(text[i]=='('){
				iter=temp.find(token_count-1);
				if(iter->second.type=="var"){
					temp[token_count]=init_token("call","(");
				}
				else{
					temp[token_count]=init_token("op","(");
				}
				token_count++;
			}
			else if(two_op.find(iter->second.value+text[i])!=two_op.end()){
				temp[token_count-1]=init_token("op",iter->second.value+text[i]);
			}
			else{
				onstr=text[i];
				temp[token_count]=init_token("op",onstr);
				onstr="";
				token_count++;
			}
		}
		else if(text[i]=='"'||text[i]=='\''){
			on_sign=text[i];
			while(i<text.length()){
				i++;
				if(text[i]==on_sign&&text[i-1]!='\\'){
					break;
				}
				onstr+=text[i];
			}
			temp[token_count]=init_token("string",onstr);
			token_count++;
			onstr="";
		}
		else{
			onstr+=text[i];
		}
		i++;
	}
	return temp;
}

map<int,token> fix(map<int,token> &tokens){
	int i=0,count=0;
	map<int,token>::iterator iter;
	map<int,token> temp;
	while(i<tokens.size()){
		iter=tokens.find(i);
		if(iter->second.type=="keyword"){
			temp[count]=tokens[i];
			while(i<tokens.size()){
				i++;
				iter=tokens.find(i);
				if(iter->second.type=="sep"){
					break;
				}
				temp[count].add(iter->second);
			}
			count++;
		}
		else{
			temp[count]=tokens[i];
			count++;
		}
		i++;
	}
	return temp;
}

vector<token> transexp(map<int,token> &tokens){
	vector<token> stack,list;
	int i=0,s=0,l=0;
	while(i<tokens.size()){
		if(tokens[i].type=="op"){
			if(s==0){
				stack.push_back(tokens[i]);
			}
			else if(tokens[i].value=="("||tokens[i].value=="{"){
				stack.push_back(tokens[i]);
			}
			else if(tokens[i].value==")"){
				while(s>0){
					if(stack.back().value=="("){
						if(stack.back().type=="call"){
							list.push_back(init_token("op","@"));
						}
						stack.pop_back();
						break;
					}
					s--;
					list.push_back(stack.back());
					stack.pop_back();
				}
			}
			else if(yxj[tokens[i].value]>yxj[stack.back().value]){
				stack.push_back(tokens[i]);
			}
			else{
				while(s>0){
					if(yxj[stack.back().value]>=yxj[tokens[i].value]){
						stack.push_back(tokens[i]);
						break;
					}
					s--;
					list.push_back(stack.back());
					stack.pop_back();
				}
			}
		}
		else{
			list.push_back(tokens[i]);
		}
		i++;
	}
	while(s>0){
		s--;
		list.push_back(stack.back());
		stack.pop_back();
	}
	for(int i=0;i<list.size();i++){
		list[i].print();
	}
	return list;
}

string compiler(map<int,token> &tokens){
	string temp="";
	int i=0;
	map<int,token>::iterator iter;
	while(i<tokens.size()){
		iter=tokens.find(i);
		if(iter->second.type=="keyword"){
			if(iter->second.value=="print"){

			}
			else if(iter->second.value=="if"){
				transexp(iter->second.sub);
			}
		}
		i++;
	}
	return temp;
}

int main(int argc,char* argv[]){
	yxj["+"]=5;
	yxj["-"]=5;
	yxj["*"]=6;
	yxj["/"]=6;
	yxj["%"]=6;
	yxj["=="]=4;
	yxj["!="]=4;
	yxj["<="]=4;
	yxj[">="]=4;
	yxj["<"]=4;
	yxj[">"]=4;
	yxj["&&"]=3;
	yxj["||"]=2;
	yxj["="]=0;
	text=getstring(argv[1]);
	if(text[-1]!='\n'){
		text+='\n';
	}
	map<int,token> tokens=parse_code();
	tokens=fix(tokens);
	string temp=compiler(tokens);
}