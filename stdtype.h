#include<vector>
#include<string>

using namespace std;

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
#define call_method 0x17//未实现
#define loop 0x18//先弹出条件后代码块
#define make_block_for_loop 0x19//标识
#define end_loop 0x1a//标识，运行时判断条件是否成立
#define make_func 0x1b//先弹出代码块后弹出参数个数
#define push_fast 0x1c//立即数入栈
#define push_const 0x1d//常数入栈
#define push_var 0x1e//变量入栈
#define add_const 0x1f//常数加入常量池
#define add_var 0x20//常数加入变量中
#define print 0x21
#define itof 0x22
#define ftoi 0x23
#define itos 0x24
#define ftos 0x25
#define pop 0x26//运算栈
#define change_var 0x27//先弹出索引再弹出值
#define push_int_arr 0x28//静态 1s
#define push_float_arr 0x29//静态 1s
#define push_Array 0x2a//动态
#define get_subscript 0x2b//先下标再数组
#define push_item 0x2c//先值再数组
#define pop_item 0x2d//动态数组对象
#define make_block 0x2e//标记
#define end_block 0x2f//标记
#define Return 0x30//返回值压栈后使用
#define change_arr_var 0x31//先数组再索引再值
#define import_api 0x32//地址
#define get_api 0x33//先名称再api
#define call_api 0x34//先数组再api函数
#define close_api 0x35//api句柄

#define beginblock_loop "0x19"
#define endloop "0x1a"
#define beginblock_func "0x2e"
#define endfunc "0x2f"

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
};

typedef void* (*FP)(List*);

class func{
public:
	string *code;
	int argc,no;
	func(string *code,int argc,int *num){
		this->code=new string(*code);
		this->argc=argc;
		this->no=*num;
		(*num)++;
	}
	~func(){
		delete(this->code);
	}
};

class api_func{
public:
	int type;
	FP p;
	string name;
	api_func(int type,FP p,string name){
		this->type=type;
		this->p=p;
		this->name=name;
	}
};