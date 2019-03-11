#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include<string>

#define true 1
#define false 0

#define null 0x00
#define iadd 0x01
#define isub 0x02
#define imul 0x03
#define idiv 0x04
#define imod 0x05
#define fadd 0x06
#define fsub 0x07
#define fmul 0x08
#define fdiv 0x09
#define fmod 0x0a
#define sadd 0x0b
#define iequal 0x0c
#define fequal 0x0d
#define sequal 0x0e
#define NOT 0x0f
#define OR 0x10
#define AND 0x11
#define i_greater 0x12
#define i_less 0x13
#define f_greater 0x14
#define f_less 0x15
#define call 0x16
#define call_method 0x17
#define loop 0x18//先弹出代码块后条件
#define make_block 0x19
#define end_block 0x1a
#define make_func 0x1b//先弹出代码块后弹出参数个数
#define push_fast 0x1c
#define push_const 0x1d
#define push_var 0x1e
#define add_const 0x1f
#define add_var 0x20
#define push_array 0x21
#define get_subscript 0x22
#define iprint 0x23
#define fprint 0x24
#define sprint 0x25

using namespace std;

FILE *fp=NULL;

wchar_t onget=0,type;

class Pobject{
public:
	int type;
	virtual Pobject* operator+(Pobject *that)=0;
	virtual Pobject* operator-(Pobject *that)=0;
	virtual Pobject* operator*(Pobject *that)=0;
	virtual Pobject* operator/(Pobject *that)=0;
	virtual Pobject* operator%(Pobject *that)=0;
	virtual void print()=0;
	virtual ~Pobject();
};

class PArray:public Pobject{
public:
	~PArray();
	PArray(){
		this->type=0;
	}
	void add(){

	}
};

int wcount(wchar_t **s1){
	int temp=0;
	while(*s1[temp]!=0){
		temp++;
	}
	return temp;
}

int htoi(string s1,int start,int end){
	int temp=0,sign=0;
	while(start<end){
		switch(s1[start]){
			case 'a':sign=10;break;
			case 'b':sign=11;break;
			case 'c':sign=12;break;
			case 'd':sign=13;break;
			case 'e':sign=14;break;
			case 'f':sign=15;break;
			default:
			sign=s1[start]-'0';
		}
		temp+=sign*pow(16,end-start-1);
		start++;
	}
	return temp;
}

string get_from_file(int len){
	int i=0;
	string temp;
	while(i<len){
		onget=fgetc(fp);
		if(onget==' '||onget=='\n'){
			onget=fgetc(fp);
		}
		temp+=onget;
		i++;
	}
	return temp;
}


wchar_t* get_from_code(string code,int start,int len);

int getlen(wchar_t onstr,int isfile){
	if(onstr=='0'){
		if(isfile){
			return htoi(get_from_file(3),0,3)*4;
		}
		else{

		}
	}
	else if(onstr=='1'||onstr=='2'||onstr=='3'||onstr=='4'){
		return 7;
	}
	else{
		return 3;
	}
	return 0;
}

string getconst(int isfile){
	string temp;
	int count=0;
	if(isfile){
		temp=get_from_file(1);
	}
	type=temp[0];
	count=getlen(temp[0],isfile);
	if(isfile){
		temp=get_from_file(count);
	}
	else{

	}
	return temp;
}

void* parse_const(string s1){
	void *temp;
	double *count=(double*)malloc(sizeof(double));
	*count=0;
	string *onstr=new string();
	switch(type){
		case '0':
		free(count);
		for(int i=0;s1[i]!=0;i+=2){
			*onstr+=htoi(s1,i,i+2);
		}
		temp=onstr;
		break;
		case '1':
		*count=htoi(s1,0,7);
		delete(onstr);
		temp=count;
		break;
		case '2':
		*count=-htoi(s1,0,7);
		delete(onstr);
		temp=count;
		break;
		case '3':
		*count=pow(0.1,htoi(s1,0,1))*htoi(s1,1,7);
		delete(onstr);
		temp=count;
		break;
		case '4':
		*count=-pow(0.1,htoi(s1,0,1))*htoi(s1,1,7);
		delete(onstr);
		temp=count;
		break;
		case '5':
		*count=htoi(s1,0,3);
		delete(onstr);
		temp=count;
		break;
		case '6':
		*count=-htoi(s1,0,3);
		delete(onstr);
		temp=count;
		break;
	}
	return temp;
}

void Iadd(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)+*(int*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Isub(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-*(int*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Imul(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)**(int*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Idiv(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)/ *(int*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Imod(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-floor(*(int*)*(stack->end()-2)/ *(int*)stack->back())**(int*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fadd(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)+*(double*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fsub(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-*(double*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fmul(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)**(double*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fdiv(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)/ *(double*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fmod(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-floor(*(double*)*(stack->end()-2)/ *(double*)stack->back())**(double*)stack->back();
	free(*(stack->end()-1));
	stack->pop_back();
}

void Sadd(vector<void*> *stack){
	*(string*)*(stack->end()-2)=*(string*)*(stack->end()-2)+*(string*)stack->back();
	delete((string*)*(stack->end()-1));
	stack->pop_back();
}

void Iprint(vector<void*> *stack){
	printf("%d\n",*(int*)stack->back());
	free(*(stack->end()-1));
	stack->pop_back();
}

void Fprint(vector<void*> *stack){
	printf("%f\n",*(double*)stack->back());
	free(*(stack->end()-1));
	stack->pop_back();
}

void Sprint(vector<void*> *stack){
	string *onstr=(string*)stack->back();
	for(int i=0;i<onstr->size();i++){
		printf("%c",(*onstr)[i]);
	}
	printf("\n" );
	delete((string*)*(stack->end()-1));
	stack->pop_back();
}

void vm(int isfile){
	string code;
	int sign=0;
	vector<void*> stack;
	while(!feof(fp)){
		if(isfile){
			code=get_from_file(4);
		}
		else{
			//code=get_from_code();
		}
		if(feof(fp)){
			break;
		}
		switch(htoi(code,2,4)){

		}
	}
}

int main(int argc,char* argv[]){
	fp=fopen(argv[1],"r");
	vm(true);
	fclose(fp);
	return 0;
}