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
#define call 0x16
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
#define push_int_arr 0x28//静态
#define push_float_arr 0x29//静态
#define push_Array 0x2a//动态
#define get_subscript_int 0x2b
#define get_subscript_float 0x2c
#define get_subscript_str 0x2d
#define get_subscript_Array 0x2e


#define beginblock_loop "0x19"
#define endloop "0x1a"

using namespace std;

FILE *fp=NULL;

wchar_t onget=0;

int type;

void delete_void(vector<void*> *stack,vector<int> *s_type,int index){
	switch((*s_type)[index]){
		case 0://str
		delete((string*)*(stack->begin()+index));
		break;
		case 1://int
		delete((int*)*(stack->begin()+index));
		break;
		case 2://double
		delete((double*)*(stack->begin()+index));
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

void Iprint(vector<void*> *stack){
	printf("%i\n",*(int*)stack->back());
}

void Fprint(vector<void*> *stack){
	printf("%f\n",*(double*)stack->back());
}

void Sprint(vector<void*> *stack){
	string *onstr=(string*)stack->back();
	for(int i=0;i<onstr->size();i++){
		printf("%c",(*onstr)[i]);
	}
	printf("\n" );
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
	stack->push_back((*var)[sign]);
	s_type->push_back((*v_type)[sign]);
}

void Change_var(vector<void*> *stack,vector<void*> *var,vector<int> *s_type,vector<int> *v_type){
	int sign=*(int*)stack->back();
	Pop(stack,s_type);
	delete_void(var,v_type,sign);
	*(var->begin()+sign)=stack->back();
	*(v_type->begin()+sign)=s_type->back();
	Pop(stack,s_type);
}

void vm(int isfile){
	string code,*onstr;
	vector<void*> stack,pool,var;
	vector<int> s_type,p_type,v_type,loop_p;
	vector<string*> loop_s;
	int on_loop=-1,on_p=0;
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
			if(*(int*)stack.back()){
				on_p=0;
			}
			else{
				delete(onstr);
				onstr=loop_s.back();
				loop_s.pop_back();
				on_p=loop_p.back();
				loop_p.pop_back();
				on_loop--;
			}
			Pop(&stack,&s_type);
			if(loop_s.size()==0){
				isfile=1;
			}
			break;
			case push_fast:
			Push_fast(&stack,&s_type,isfile,onstr,&on_p);
			break;
			case push_const:
			Push_var(&stack,&pool,&s_type,&p_type,isfile,&on_p,onstr);
			break;
			case push_var:
			Push_var(&stack,&var,&s_type,&v_type,isfile,&on_p,onstr);
			break;
			case add_const:
			Push_fast(&pool,&s_type,isfile,onstr,&on_p);
			break;
			case add_var:
			var.push_back(stack.back());
			v_type.push_back(s_type.back());
			Pop(&stack,&s_type);
			break;
			case print:
			switch(s_type.back()-'0'){
				case 0:
				Sprint(&stack);
				break;
				case 1:
				Iprint(&stack);
				break;
				case 2:
				Fprint(&stack);
				break;
			}
			Pop(&stack,&s_type);
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
			Change_var(&stack,&var,&s_type,&v_type);
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
