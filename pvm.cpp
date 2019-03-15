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
#define Not 0x0f
#define Or 0x10
#define And 0x11
#define i_greater 0x12
#define i_less 0x13
#define f_greater 0x14
#define f_less 0x15
#define call 0x16//读取编号获取对象
#define call_method 0x17
#define loop 0x18//先弹出条件后代码块
#define make_block_for_loop 0x19
#define end_loop 0x1a
#define make_func 0x1b//先弹出代码块后弹出参数个数
#define push_fast 0x1c
#define push_const 0x1d
#define push_var 0x1e
#define add_const 0x1f
#define add_var 0x20
#define print 0x21
#define itof 0x22
#define ftoi 0x23
#define itos 0x24
#define ftos 0x25
#define pop 0x26
#define change_var 0x27//先弹出索引再弹出值
#define push_int_arr 0x28//静态 1s
#define push_float_arr 0x29//静态 1s
#define push_Array 0x2a//动态
#define get_subscript 0x2b//先下标再数组
#define push_item 0x2c//先值再数组
#define pop_item 0x2d
#define make_block 0x2e
#define end_block 0x2f
#define Return 0x30
#define change_arr_var 0x31//先数组再索引再值

#define beginblock_loop "0x19"
#define endloop "0x1a"
#define beginblock_func "0x2e"
#define endfunc "0x2f"

using namespace std;

FILE *fp=NULL;

wchar_t onget=0;

int type,func_count=0;

void Print(vector<void*> *stack,vector<int> *s_type);
void Sprint(void *value);

class Array{
public:
	void **value;
	int length=0,type;
	Array(int type,int length=0){
		this->type=type;
		this->length=length;
		this->value=new void*[length];
		for(int i=0;i<length;i++){
			switch(type){
				case '3':
				this->value[i]=new int(0);
				break;
				case '4':
				this->value[i]=new double(0.0);
				break;
			}
		}
	};
	~Array(){
		for(int i=0;i<this->length;i++){
			switch(this->type){
				case '3':
				delete((int*)this->value[i]);
				break;
				case '4':
				delete((double*)this->value[i]);
				break;
			}
		}
	}
};

class List{
public:
	vector<void*> value;
	vector<int> type;
	int length=0;
	void push_back(void* value,int type){
		this->value.push_back(value);
		this->type.push_back(type);
		this->length++;
	};
};

class func{
public:
	string *code;
	int argc,no;
	func(string *code,int argc){
		this->code=new string(*code);
		this->argc=argc;
		this->no=func_count;
		func_count++;
	}
	~func(){
		delete(this->code);
		delete(this);
	}
};

void release(List *temp);

vector<List*> path;

void delete_void(vector<void*> *stack,vector<int> *s_type,int index){
	switch((*s_type)[index]){
		case '0'://str
		delete((string*)*(stack->begin()+index));
		break;
		case '1'://int
		delete((int*)*(stack->begin()+index));
		break;
		case '2'://double
		delete((double*)*(stack->begin()+index));
		break;
		case '3'://int_arr
		delete((Array*)*(stack->begin()+index));
		break;
		case '4'://double_arr
		delete((Array*)*(stack->begin()+index));
		break;
		case '5'://char
		delete((char*)*(stack->begin()+index));
		break;
		case '6'://list
		release((List*)*(stack->begin()+index));
		break;
		case '7'://func
		break;
	}
}

void Pop(vector<void*> *stack,vector<int> *s_type){
	delete_void(stack,s_type,stack->size()-1);
	s_type->pop_back();
	stack->pop_back();
}

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

int getdec(double num){
	int temp=0;
	while((int)num!=num){
		num*=10;
		temp++;
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


string get_from_code(string *code,int *start,int len){
	string onstr="";
	for(int i=0;i<len;i++){
		onstr+=(*code)[*start];
		(*start)++;
	}
	return onstr;
};

string getcode(int len,string *code,int *start,int isfile){
	if(isfile){
		return get_from_file(len);
	}
	else{
		return get_from_code(code,start,len);
	}
}

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
}

void Isub(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-*(int*)stack->back();
}

void Imul(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)**(int*)stack->back();
}

void Idiv(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)/ *(int*)stack->back();
}

void Imod(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)-floor(*(int*)*(stack->end()-2)/ *(int*)stack->back())**(int*)stack->back();
}

void Iequal(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)==*(int*)stack->back();
}

void Igreater(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)>*(int*)stack->back();
}

void Iless(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)<*(int*)stack->back();
}

void Fadd(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)+*(double*)stack->back();
}

void Fsub(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-*(double*)stack->back();
}

void Fmul(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)**(double*)stack->back();
}

void Fdiv(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)/ *(double*)stack->back();
}

void Fmod(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)-floor(*(double*)*(stack->end()-2)/ *(double*)stack->back())**(double*)stack->back();
}

void Fequal(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)== *(double*)stack->back();
}

void Fgreater(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)>*(double*)stack->back();
}

void Fless(vector<void*> *stack){
	*(double*)*(stack->end()-2)=*(double*)*(stack->end()-2)<*(double*)stack->back();
}

void Sadd(vector<void*> *stack){
	*(string*)*(stack->end()-2)=*(string*)*(stack->end()-2)+*(string*)stack->back();
}

void Sequal(vector<void*> *stack){
	*(string*)*(stack->end()-2)=*(string*)*(stack->end()-2)==*(string*)stack->back();
}

void NOT(vector<void*> *stack){
	*(int*)(*(stack->end()-1))=!*(int*)(*(stack->end()-1));
}

void OR(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)||*(int*)stack->back();
}

void AND(vector<void*> *stack){
	*(int*)*(stack->end()-2)=*(int*)*(stack->end()-2)&&*(int*)stack->back();
}

void Iprint(void *value){
	printf("%i",*(int*)value);
}

void Fprint(void *value){
	printf("%f",*(double*)value);
}

void Sprint(void *value){
	string *onstr=(string*)value;
	int max=onstr->size();
	printf("\"");
	for(int i=0;i<max;i++){
		printf("%c",(*onstr)[i]);
	}
	printf("\"");
}

void Cprint(void *value){
	printf("%c",*(char*)value);
}

void IAprint(void *value){
	Array *temp=(Array*)value;
	int i=0,max=temp->length;
	printf("[");
	while(i<max-1){
		printf("%d,",*(int*)(temp->value)[i]);
		i++;
	}
	printf("%d]",*(int*)(temp->value)[i]);
}

void FAprint(void *value){
	Array *temp=(Array*)value;
	int i=0,max=temp->length;
	printf("[");
	while(i<max-1){
		printf("%f,",*(double*)(temp->value)[i]);
		i++;
	}
	printf("%f]",*(double*)(temp->value)[i]);
}

void Lprint(List* temp){
	int i=0,max=temp->length;
	printf("[");
	while(i<max){
		switch(temp->type[i]){
			case '0':
			Sprint(temp->value[i]);
			break;
			case '1':
			Iprint(temp->value[i]);
			break;
			case '2':
			Fprint(temp->value[i]);
			break;
			case '3':
			IAprint(temp->value[i]);
			break;
			case '4':
			FAprint(temp->value[i]);
			break;
			case '5':
			Cprint(temp->value[i]);
			break;
			case '6':
			Lprint((List*)temp->value[i]);
			break;
		}
		if(i!=max-1){
			printf(",");
		}
		i++;
	}
	printf("]");
}

void Print(vector<void*> *stack,vector<int> *s_type){
	switch(s_type->back()){
		case '0':
		Sprint(stack->back());
		break;
		case '1':
		Iprint(stack->back());
		break;
		case '2':
		Fprint(stack->back());
		break;
		case '3':
		IAprint(stack->back());
		break;
		case '4':
		FAprint(stack->back());
		break;
		case '5':
		Cprint(stack->back());
		break;
		case '6':
		Lprint((List*)stack->back());
		break;
	}
	Pop(stack,s_type);
	printf("\n");
}

void Itof(vector<void*> *stack,vector<int> *s_type){
	double *linshi=new double(*(int*)stack->back());
	Pop(stack,s_type);
	stack->push_back(linshi);
	s_type->push_back(50);
}

void Ftoi(vector<void*> *stack,vector<int> *s_type){
	int *linshi=new int(*(double*)stack->back());
	Pop(stack,s_type);
	stack->push_back(linshi);
	s_type->push_back(49);
}

void Itos(vector<void*> *stack,vector<int> *s_type){
	string *onstr=new string(to_string(*(int*)stack->back()));
	Pop(stack,s_type);
	stack->push_back(onstr);
	s_type->push_back(48);
};

void Ftos(vector<void*> *stack,vector<int> *s_type){
	double linshi=*(double*)stack->back();
	string *onstr=new string(to_string(*(double*)stack->back()));
	Pop(stack,s_type);
	stack->push_back(onstr);
	s_type->push_back(48);
};

void Loop(vector<void*> *stack,vector<int> *s_type,vector<string*> *loop_s,vector<int> *loop_p,string **onstr,int *on_loop,int *isfile,int *on_p){
	int sign=*(int*)stack->back();
	Pop(stack,s_type);
	if(sign){
		loop_s->push_back(*onstr);
		loop_p->push_back(*on_p);
		*onstr=(string*)stack->back();
		stack->pop_back();
		s_type->pop_back();
		*isfile=0;
		*on_p=0;
		(*on_loop)++;
	}
	else{
		Pop(stack,s_type);
	}
}

void End_loop(vector<void*> *stack,vector<int> *s_type,vector<string*> *loop_s,vector<int> *loop_p,int *on_loop,int *on_p,string **onstr){
	if(*(int*)stack->back()){
		(*on_p)=0;
	}
	else{
		delete(*onstr);
		*onstr=loop_s->back();
		loop_s->pop_back();
		*on_p=loop_p->back();
		loop_p->pop_back();
		(*on_loop)--;
	}
	Pop(stack,s_type);
}

void Make_func(vector<void*> *stack,vector<int> *s_type,vector<void*> *pool,vector<int> *p_type){
	int index=*(int*)stack->back(),no;
	Pop(stack,s_type);
	string *onstr=(string*)stack->back();
	func *temp=new func(onstr,index);
	Pop(stack,s_type);
	pool->push_back(temp);
	p_type->push_back('7');
}

void Make_block_for_loop(vector<void*> *stack,vector<int> *s_type,int isfile,string *code,int *start){
	string onstr,temp="";
	int kh=1;
	while(true){
		onstr=getcode(4,code,start,isfile);
		if(onstr==beginblock_loop){
			kh++;
		}
		else if(onstr==endloop){
			kh--;
		}
		if(kh==0){
			temp+=endloop;
			break;
		}
		temp+=onstr;
	}
	stack->push_back(new string(temp));
	s_type->push_back(48);
}

void Make_block_for_func(vector<void*> *stack,vector<int> *s_type,int isfile,string *code,int *start){
	string onstr,temp="";
	int kh=1;
	while(true){
		onstr=getcode(4,code,start,isfile);
		if(onstr==beginblock_func){
			kh++;
		}
		else if(onstr==endfunc){
			kh--;
		}
		if(kh==0){
			break;
		}
		temp+=onstr;
	}
	stack->push_back(new string(temp));
	s_type->push_back(48);
}

void Push_fast(vector<void*> *stack,vector<int> *s_type,int isfile,string *code,int *start){
	string onstr;
	int on_p;
	onstr=getcode(1,code,start,isfile);
	on_p=getlen(onstr[0],isfile);
	type=onstr[0];
	onstr=getcode(on_p,code,start,isfile);
	stack->push_back(parse_const(&onstr));
	s_type->push_back(type);
}

void Push_var(vector<void*> *stack,vector<void*> *var,vector<int> *s_type,vector<int> *v_type,int isfile,int *start,string *code){
	Push_fast(stack,s_type,isfile,code,start);
	int sign=*(int*)stack->back();
	Pop(stack,s_type);
	switch((*v_type)[sign]){
		case '0':
		stack->push_back(new string(*(string*)(*var)[sign]));
		break;
		case '1':
		stack->push_back(new int(*(int*)(*var)[sign]));
		break;
		case '2':
		stack->push_back(new double(*(double*)(*var)[sign]));
		break;
		case '3':
		stack->push_back(new Array(*(Array*)(*var)[sign]));
		break;
		case '4':
		stack->push_back(new Array(*(Array*)(*var)[sign]));
		break;
		case '5':
		stack->push_back(new char(*(char*)(*var)[sign]));
		break;
		case '6':
		stack->push_back(new List(*(List*)(*var)[sign]));
		break;
	}
	s_type->push_back((*v_type)[sign]);
}

void Change_var(vector<void*> *stack,vector<void*> *var,vector<int> *s_type,vector<int> *v_type){
	int index=*(int*)stack->back();
	Pop(stack,s_type);
	delete_void(var,v_type,index);
	switch(s_type->back()){
		case '0':
		*(var->begin()+index)=new string(*(string*)stack->back());
		break;
		case '1':
		*(var->begin()+index)=new int(*(int*)stack->back());
		break;
		case '2':
		*(var->begin()+index)=new double(*(double*)stack->back());
		break;
		case '3':
		*(var->begin()+index)=new Array(*(Array*)stack->back());
		break;
		case '4':
		*(var->begin()+index)=new Array(*(Array*)stack->back());
		break;
		case '5':
		*(var->begin()+index)=new char(*(char*)stack->back());
		break;
		case '6':
		*(var->begin()+index)=new List(*(List*)stack->back());
		break;
	}
	Pop(stack,s_type);
}

void Add_var(vector<void*> *stack,vector<void*> *var,vector<int> *s_type,vector<int> *v_type){
	switch(s_type->back()){
		case '0':
		var->push_back(new string(*(string*)stack->back()));
		break;
		case '1':
		var->push_back(new int(*(int*)stack->back()));
		break;
		case '2':
		var->push_back(new double(*(double*)stack->back()));
		break;
		case '3':
		var->push_back(new Array(*(Array*)stack->back()));
		break;
		case '4':
		var->push_back(new Array(*(Array*)stack->back()));
		break;
		case '5':
		var->push_back(new char(*(char*)stack->back()));
		break;
		case '6':
		var->push_back(new List(*(List*)stack->back()));
		break;
	}
	v_type->push_back(s_type->back());
	Pop(stack,s_type);
}

void Change_arr_var(vector<void*> *stack,vector<int> *s_type){
	int index;
	Array *temp;
	List *linshi;
	switch(s_type->back()){
		case '6':
		linshi=(List*)stack->back();
		index=s_type->back();
		stack->pop_back();
		s_type->pop_back();
		Change_var(stack,&linshi->value,s_type,&linshi->type);
		stack->push_back(linshi);
		s_type->push_back(index);
		break;
		default:
		temp=(Array*)stack->back();
		type=s_type->back();
		stack->pop_back();
		s_type->pop_back();
		index=*(int*)stack->back();
		Pop(stack,s_type);
		if(type=='3'){
			delete((int*)temp->value[index]);
			temp->value[index]=new int(*(int*)stack->back());
		}
		else{
			delete((double*)temp->value[index]);
			temp->value[index]=new double(*(double*)stack->back());
		}
		Pop(stack,s_type);
		stack->push_back(temp);
		s_type->push_back(type);
	}
}

void push_arr(vector<void*> *stack,vector<int> *s_type,int form){
	Array *temp=new Array(form,*(int*)stack->back());
	Pop(stack,s_type);
	stack->push_back(temp);
	s_type->push_back(form);
}

void Push_Array(vector<void*> *stack,vector<int> *s_type){
	if(!path.size()){
		stack->push_back(new List());
	}
	else{
		stack->push_back(path.back());
		path.pop_back();
	}
	s_type->push_back('6');
}

void Get_subscript(vector<void*> *stack,vector<int> *s_type){
	int t,index=*(int*)stack->back();
	void *temp;
	Pop(stack,s_type);
	switch(s_type->back()){
		case '0':
		t='5';
		temp=new char((*(string*)stack->back())[index]);
		break;
		case '3':
		t='1';
		temp=new int(((int*)stack->back())[index]);
		break;
		case '4':
		t='2';
		temp=new double(((double*)stack->back())[index]);
		break;
	}
	Pop(stack,s_type);
	stack->push_back(temp);
	s_type->push_back(t);
}

void Push_item(vector<void*> *stack,vector<int> *s_type){
	void *value=stack->back();
	stack->pop_back();
	List *temp=(List*)stack->back();
	temp->push_back(value,s_type->back());
	s_type->pop_back();
}

void release(List *temp){
	while(temp->length>0){
		Pop(&temp->value,&temp->type);
		temp->length--;
	}
	path.push_back(temp);
}

void Call(vector<void*> *stack,vector<int> *s_type,vector<void*> *pool,string **onstr,vector<func*> *on_func,vector<int> *func_p,int *on_p,vector<void*> **var,vector<int> **v_type,vector<vector<void*>* >*v_stack,vector<vector<int>* >*v_s_type){
	int index=*(int*)stack->back();
	Pop(stack,s_type);
	on_func->push_back((func*)(*pool)[index]);
	*onstr=on_func->back()->code;
	func_p->push_back(*on_p);
	*on_p=0;
	v_stack->push_back(*var);
	*var=new vector<void*>();
	v_s_type->push_back(*v_type);
	*v_type=new vector<int>();
	for(int i=0;i<on_func->back()->argc;i++){
		(*var)->push_back(stack->back());
		(*v_type)->push_back(s_type->back());
		stack->pop_back();
		s_type->pop_back();
	}
}

void RETURN(string **onstr,vector<string*> *func_s,vector<func*> *on_func,int *on_p,vector<void*> **var,vector<int> **v_type,vector<vector<void*>* >*v_stack,vector<vector<int>* >*v_s_type,vector<int> *func_p){
	*onstr=func_s->back();
	func_s->pop_back();
	on_func->pop_back();
	*on_p=func_p->back();
	func_p->pop_back();
	for(int i=0;i<(*var)->size();i++){
		delete_void(*var,*v_type,i);
	}
	delete(*var);
	*var=v_stack->back();
	v_stack->pop_back();
	delete(*v_type);
	*v_type=v_s_type->back();
	v_s_type->pop_back();
}

void vm(int isfile){
	string code,*onstr;
	vector<void*> stack,pool,*var=new vector<void*>();
	vector<int> s_type,p_type,*v_type=new vector<int>(),loop_p,func_p;
	vector<string*> loop_s,func_s;
	vector<func*> on_func;
	vector<vector<void*>* > v_stack;
	vector<vector<int>* > v_s_type;
	int on_loop=-1,on_p=0,on_f=-1;
	while(!feof(fp)){
		code=getcode(4,onstr,&on_p,isfile);
		if(feof(fp)){
			break;
		}
		switch(htoi(&code,2,4)){
			case null:
			break;
			case iadd:
			Iadd(&stack);
			Pop(&stack,&s_type);
			break;
			case isub:
			Isub(&stack);
			Pop(&stack,&s_type);
			break;
			case imul:
			Imul(&stack);
			Pop(&stack,&s_type);
			break;
			case idiv:
			Idiv(&stack);
			Pop(&stack,&s_type);
			break;
			case imod:
			Imod(&stack);
			Pop(&stack,&s_type);
			break;
			case fadd:
			Fadd(&stack);
			Pop(&stack,&s_type);
			break;
			case fsub:
			Fsub(&stack);
			Pop(&stack,&s_type);
			break;
			case fmul:
			Fmul(&stack);
			Pop(&stack,&s_type);
			break;
			case fdiv:
			Fdiv(&stack);
			Pop(&stack,&s_type);
			break;
			case fmod:
			Fmod(&stack);
			Pop(&stack,&s_type);
			break;
			case sadd:
			Sadd(&stack);
			Pop(&stack,&s_type);
			break;
			case iequal:
			Iequal(&stack);
			Pop(&stack,&s_type);
			break;
			case fequal:
			Fequal(&stack);
			Pop(&stack,&s_type);
			break;
			case sequal:
			Sequal(&stack);
			Pop(&stack,&s_type);
			break;
			case Not:
			NOT(&stack);
			break;
			case Or:
			OR(&stack);
			Pop(&stack,&s_type);
			break;
			case And:
			AND(&stack);
			Pop(&stack,&s_type);
			break;
			case i_greater:
			Igreater(&stack);
			Pop(&stack,&s_type);
			break;
			case i_less:
			Iless(&stack);
			Pop(&stack,&s_type);
			break;
			case f_greater:
			Fgreater(&stack);
			Pop(&stack,&s_type);
			break;
			case f_less:
			Fless(&stack);
			Pop(&stack,&s_type);
			break;
			case call:
			func_s.push_back(onstr);
			Call(&stack,&s_type,&pool,&onstr,&on_func,&func_p,&on_p,&var,&v_type,&v_stack,&v_s_type);
			on_f++;
			isfile=false;
			break;
			case loop:
			Loop(&stack,&s_type,&loop_s,&loop_p,&onstr,&on_loop,&isfile,&on_p);
			break;
			case make_block_for_loop:
			if(on_loop!=-1){
				Make_block_for_loop(&stack,&s_type,isfile,onstr,&on_p);
			}
			else{
				Make_block_for_loop(&stack,&s_type,isfile,NULL,0);
			}
			break;
			case end_loop:
			End_loop(&stack,&s_type,&loop_s,&loop_p,&on_loop,&on_p,&onstr);
			if(on_f==-1&&on_loop==-1){
				isfile=true;
			}
			break;
			case make_func:
			Make_func(&stack,&s_type,&pool,&p_type);
			break;
			case push_fast:
			Push_fast(&stack,&s_type,isfile,onstr,&on_p);
			break;
			case push_const:
			Push_var(&stack,&pool,&s_type,&p_type,isfile,&on_p,onstr);
			break;
			case push_var:
			Push_var(&stack,var,&s_type,v_type,isfile,&on_p,onstr);
			break;
			case add_const:
			Push_fast(&pool,&s_type,isfile,onstr,&on_p);
			break;
			case add_var:
			Add_var(&stack,var,&s_type,v_type);
			break;
			case print:
			Print(&stack,&s_type);
			break;
			case itof:
			Itof(&stack,&s_type);
			break;
			case ftoi:
			Ftoi(&stack,&s_type);
			break;
			case itos:
			Itos(&stack,&s_type);
			break;
			case ftos:
			Ftos(&stack,&s_type);
			break;
			case pop:
			Pop(&stack,&s_type);
			break;
			case change_var:
			Push_fast(&stack,&s_type,isfile,&code,&on_p);
			Change_var(&stack,var,&s_type,v_type);
			break;
			case push_int_arr:
			push_arr(&stack,&s_type,'3');
			break;
			case push_float_arr:
			push_arr(&stack,&s_type,'4');
			break;
			case push_Array:
			Push_Array(&stack,&s_type);
			break;
			case get_subscript:
			Get_subscript(&stack,&s_type);
			break;
			case push_item:
			Push_item(&stack,&s_type);
			break;
			case pop_item:
			Pop(&((List*)stack.back())->value,&((List*)stack.back())->type);
			((List*)stack.back())->length--;
			break;
			case make_block:
			Make_block_for_func(&stack,&s_type,isfile,onstr,&on_p);
			break;
			case Return:
			on_f--;
			RETURN(&onstr,&func_s,&on_func,&on_p,&var,&v_type,&v_stack,&v_s_type,&func_p);
			if(on_f==-1&&on_loop==-1){
				isfile=true;
			}
			break;
			case change_arr_var:
			Change_arr_var(&stack,&s_type);
			break;
			default:
			printf("Error:Unknown code");
			Sprint(&code);
			printf("\n");
		}
	}
}

int main(int argc,char* argv[]){
	fp=fopen(argv[1],"r");
	vm(true);
	fclose(fp);
	return 0;
}
