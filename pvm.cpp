#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include<string>
#include<iostream>

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
#define Not 0x0f
#define Or 0x10
#define And 0x11
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
#define itof 0x26
#define ftoi 0x27
#define itos 0x28
#define ftos 0x29
#define pop 0x2a
#define change_var 0x2b

using namespace std;

FILE *fp=NULL;

wchar_t onget=0,type;

int wcount(wchar_t **s1){
	int temp=0;
	while(*s1[temp]!=0){
		temp++;
	}
	return temp;
}

int htoi(string *s1,int start,int end){
	int temp=0,sign=0;
	while(start<end){
		switch((*s1)[start]){
			case 'a':sign=10;break;
			case 'b':sign=11;break;
			case 'c':sign=12;break;
			case 'd':sign=13;break;
			case 'e':sign=14;break;
			case 'f':sign=15;break;
			default:
			sign=(*s1)[start]-'0';
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
	string str;
	if(onstr=='0'){
		if(isfile){
			str=get_from_file(3);
			return htoi(&str,0,3)*4;
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

void* parse_const(string *s1){
	void *temp;
	string *onstr=NULL;
	switch(type){
		case '0':
		onstr=new string();
		for(int i=0;(*s1)[i]!=0;i+=2){
			*onstr+=htoi(s1,i,i+2);
		}
		temp=onstr;
		break;
		case '1':
		temp=new int(htoi(s1,0,7));
		break;
		case '2':
		temp=new int(-htoi(s1,0,7));
		break;
		case '3':
		temp=new double(pow(0.1,htoi(s1,0,1))*htoi(s1,1,7));
		break;
		case '4':
		temp=new double(-pow(0.1,htoi(s1,0,1))*htoi(s1,1,7));
		break;
	}
	return temp;
}

void Iadd(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)+*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Isub(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Imul(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)**(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Idiv(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)/ *(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Imod(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-floor(*(int*)*(stack->end()-2)/ *(int*)stack->back())**(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Iequal(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)==*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Igreater(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)>*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Iless(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)<*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Fadd(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)+*(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fsub(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-*(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fmul(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)**(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fdiv(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)/ *(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fmod(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-floor(*(double*)*(stack->end()-2)/ *(double*)stack->back())**(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fequal(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)== *(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fgreater(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)>*(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Fless(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)<*(double*)stack->back();
	delete((double*)*(stack->end()-1));
	stack->pop_back();
}

void Sadd(vector<void*> *stack){
	*(string*)*(stack->end()-2)=*(string*)*(stack->end()-2)+*(string*)stack->back();
	delete((string*)*(stack->end()-1));
	stack->pop_back();
}

void Sequal(vector<void*> *stack){
	*(string*)*(stack->end()-2)=*(string*)*(stack->end()-2)==*(string*)stack->back();
	delete((string*)*(stack->end()-1));
	stack->pop_back();
}

void NOT(vector<void*> *stack){
	*(int*)(*(stack->end()-1))=!*(int*)(*(stack->end()-1));
}

void OR(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)||*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void AND(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)&&*(int*)stack->back();
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Iprint(vector<void*> *stack){
	printf("%i\n",*(int*)stack->back());
	delete((int*)*(stack->end()-1));
	stack->pop_back();
}

void Fprint(vector<void*> *stack){
	printf("%f\n",*(double*)stack->back());
	delete((double*)*(stack->end()-1));
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

void Push_fast(vector<void*> *stack,int isfile){
	string onstr;
	if(isfile){
		onstr=get_from_file(1);
	}
	isfile=getlen(onstr[0],isfile);
	type=onstr[0];
	onstr=get_from_file(isfile);
	stack->push_back(parse_const(&onstr));
}

void vm(int isfile){
	string code;
	int sign=0;
	vector<void*> stack,pool,var;
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
		switch(htoi(&code,2,4)){
			case null:
			break;
			case iadd:
			Iadd(&stack);
			break;
			case isub:
			Isub(&stack);
			break;
			case imul:
			Imul(&stack);
			break;
			case idiv:
			Idiv(&stack);
			break;
			case imod:
			Imod(&stack);
			break;
			case fadd:
			Fadd(&stack);
			break;
			case fsub:
			Fsub(&stack);
			break;
			case fmul:
			Fmul(&stack);
			break;
			case fdiv:
			Fdiv(&stack);
			break;
			case fmod:
			Fmod(&stack);
			break;
			case sadd:
			Sadd(&stack);
			break;
			case iequal:
			Iequal(&stack);
			break;
			case fequal:
			Fequal(&stack);
			break;
			case sequal:
			Sequal(&stack);
			break;
			case Not:
			NOT(&stack);
			break;
			case Or:
			OR(&stack);
			break;
			case And:
			AND(&stack);
			break;
			case i_greater:
			Igreater(&stack);
			break;
			case i_less:
			Iless(&stack);
			break;
			case f_greater:
			Fgreater(&stack);
			break;
			case f_less:
			Fless(&stack);
			break;
			case push_fast:
			Push_fast(&stack,isfile);
			break;
			case push_const:
			Push_fast(&stack,isfile);
			sign=*(int*)stack.back();
			delete((int*)*(stack.end()-1));
			stack.pop_back();
			stack.push_back(pool[sign]);
			break;
			case push_var:
			Push_fast(&stack,isfile);
			sign=*(int*)stack.back();
			delete((int*)*(stack.end()-1));
			stack.pop_back();
			stack.push_back(var[sign]);
			break;
			case add_const:
			Push_fast(&pool,isfile);
			break;
			case add_var:
			var.push_back(stack.back());
			stack.pop_back();
			break;
			case iprint:
			Iprint(&stack);
			break;
			case fprint:
			Fprint(&stack);
			break;
			case sprint:
			Sprint(&stack);
			break;
			case pop:
			stack.pop_back();//待添加类型信息记录以及释放void*的内存
			break;
			case change_var:
			sign=*(int*)stack.back();
			delete((int*)*(stack.end()-1));
			stack.pop_back();
			var[sign]=stack.back();
			stack.pop_back();
			break;
		}
	}
}

int main(int argc,char* argv[]){
	fp=fopen(argv[1],"r");
	vm(true);
	fclose(fp);
	return 0;
}