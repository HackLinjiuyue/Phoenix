#include <iostream>
#include <map>
using namespace std;
string getcode(string path){
	FILE * pFile;
	string sss;
	long lSize;
	char * buffer;
	size_t result;
	pFile = fopen ( path.c_str() , "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);
	// allocate memory to contain the whole file:
	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	/* the whole file is now loaded in the memory buffer. */
	// terminate
	sss.assign(buffer,result);
	fclose (pFile);
	free (buffer);
	return sss;
}

typedef struct reg{
	double number=0;//指针本身为基地址，number存的是末地址或是指向地址
	string str="";
	bool isstatic=false,isstr=false;
}regs;

string getdata(regs &input){
	if(!input.isstr){
		if(input.number==int(input.number)){
			return to_string(int(input.number));
		}
		else{
			return to_string(input.number);
		}
	}
	else{
		return input.str;
	}
}

void VM(string input){
	map<int,int> stack;
	if(input[input.length()-1]!='\n'){
		input+='\n';
	}
	map<int,map<int,string> > code;
	map<int,string> on_code;
	map<int,regs> vars;
	reg regis[4]={reg(),reg(),reg(),reg()};
	reg &r1=regis[0],&r2=regis[1],&r3=regis[2],&r4=regis[3];
	map<int,regs>::iterator on_var;
	short c_count=0,on_c_count=0,s=0,v=0;
	string onstr;
	for(int i=0;i<input.length();i++){
		if(input[i]=='\n'){
			code[c_count][on_c_count]=onstr,onstr="";
			c_count++,on_c_count=0;
		}
		else if(input[i]==' '){
			code[c_count][on_c_count]=onstr,onstr="";
			on_c_count++;
		}
		else{
			onstr+=input[i];
		}
	}
	for(int i=0;i<c_count;i++){
		on_code=code.find(i)->second;
		onstr=on_code.find(0)->second;
		if(onstr=="add"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else if(!r1.isstr){
				r3.number=r1.number+r2.number;
				r3.isstr=false;
			}
			else{
				r3.str=r1.str+r2.str;
				r3.isstr=true;
			}
		}
		else if(onstr=="sub"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else if(!r1.isstr){
				r3.number=r1.number-r2.number;
				r3.isstr=false;
			}
			else{
				cout<<"错误：类型不匹配\n";
				break;
			}
		}
		else if(onstr=="mul"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else if(!r1.isstr){
				r3.number=r1.number*r2.number;
				r3.isstr=false;
			}
			else{
				cout<<"错误：类型不匹配\n";
				break;
			}
		}
		else if(onstr=="div"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else if(!r1.isstr){
				if(r2.number==0){
					cout<<"错误：除0\n";
					break;
				}
				r3.number=r1.number/r2.number;
				r3.isstr=false;
			}
			else{
				cout<<"错误：类型不匹配\n";
				break;
			}
		}
		else if(onstr=="mov"){
			regis[stoi(on_code.find(2)->second)-1]=regis[stoi(on_code.find(1)->second)-1];
		}
		else if(onstr=="mod"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else if(!r1.isstr){
				if(r2.number==0){
					cout<<"错误：除0\n";
					break;
				}
				r3.number=r1.number-int(r1.number/r2.number)*r2.number;
				r3.isstr=false;
			}
			else{
				cout<<"错误：类型不匹配\n";
				break;
			}
		}
		else if(onstr=="eq"){
			if(r1.isstr!=r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else{
				r3.number=getdata(r1)==getdata(r2);
			}
		}
		else if(onstr=="lt"){
			if(r1.isstr!=r2.isstr||r1.isstr||r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else{
				r3.number=r1.number<r2.number;
			}
		}
		else if(onstr=="gt"){
			if(r1.isstr!=r2.isstr||r1.isstr||r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else{
				r3.number=r1.number>r2.number;
			}
		}
		else if(onstr=="or"){
			if(r1.isstr!=r2.isstr||r1.isstr||r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else{
				r3.number=r1.number||r2.number;
			}
		}
		else if(onstr=="and"){
			if(r1.isstr!=r2.isstr||r1.isstr||r2.isstr){
				cout<<"错误：类型不匹配\n";
				break;
			}
			else{
				r3.number=r1.number&&r2.number;
			}
		}
		else if(onstr=="not"){
			r3.number=!r3.number;
		}
		else if(onstr=="go"){
			stack[s]=i,s++;
			i=stoi(on_code.find(1)->second);
			continue;
		}
		else if(onstr=="tjp"){
			if(r3.number){
				i=stoi(on_code.find(1)->second);
				continue;
			}
		}
		else if(onstr=="jp"){
			i=stoi(on_code.find(1)->second);
			continue;
		}
		else if(onstr=="fjp"){
			if(!r3.number){
				i=stoi(on_code.find(1)->second);
				continue;
			}
		}
		else if(onstr=="rs"){
			r1=reg(),r2=reg(),r3=reg(),r4=reg();
		}
		else if(onstr=="ldc"){
			onstr=on_code.find(1)->second;
			if('0'<=onstr[0]&&onstr[0]<='9'){
				regis[stoi(on_code.find(2)->second)-1].number=stod(onstr),r3.isstr=false;
			}
			else{
				regis[stoi(on_code.find(2)->second)-1].str=onstr.substr(1,onstr.length()-2),r3.isstr=true;
			}
		}
		else if(onstr=="ldv"){
			on_c_count=stoi(on_code.find(1)->second);
			on_var=vars.find(on_c_count);
			if(on_var==vars.end()){
				cout<<"错误：变量不存在\n";
				break;
			}
			else{
				regis[stoi(on_code.find(2)->second)-1]=on_var->second;
			}
		}
		else if(onstr=="st"){
			vars[r1.number]=r3;
		}
		else if(onstr=="rt"){
			if(s==0){
				cout<<"错误：返回地点为空\n";
				break;
			}
			s--,i=stack[s],stack.erase(s);
		}
		else if(onstr=="sv"){
			onstr=on_code.find(1)->second;
			switch(onstr[0]){
				case '0':cin>>regis[3].str;break;
				case '1':break;
				case '2':exit(0);
				case '3':cout<<getdata(r2);
			}
		}
		else{
			cout<<"错误：未知的指令"<<onstr<<"\n";
			break;
		}
	}
}
int main(int argc,char* argv[])
{
	cout<<fixed;
	VM(getcode(argv[1]));
    return 0;
}