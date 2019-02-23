#include<stdio.h>//使用输入输出
#include<stdlib.h>//使用内存分配
#include<math.h>//使用pow
#define false 0
#define true 1

char** args;//接收argv的值，以供虚拟机调取

FILE *fp;//文件指针

int path_count;//被回收Array指针地址的计数量

wchar_t onget='0';//用于接收每个从文件/代码块读取进来的字符

typedef struct array{
	int length,*type;
	void **pool;
}Array;//数组结构体，用于常变量存储以及作为数据类型在虚拟机中使用

Array *global,**path;//定义全局变量池以及回收指针用的二级指针

void fvm(wchar_t **code,Array *last_pool,Array *last_var,Array *last_calc,int is_loop);

void throw_err(int code){//按照错误码报错
	switch(code){
		case 0:
		printf("错误：下标越界\n");
		break;
	}
	exit(1);
}


int wcmp(wchar_t *a,wchar_t *b){//字符串比较函数
	int temp=true,i=0;
	while(true){
		if(a[i]!=b[i]){
			temp=0;
			break;
		}
		else if(a[i]==0||b[i]==0){
			break;
		}
		i++;
	}
	return temp;
}

void delete(Array *calc){//删除栈顶元素/用于虚拟机结束时对资源的释放，同时回收可能的内存资源用于下次分配
	calc->length--;
	char type=calc->type[calc->length];
	if(type==0||type==1){
		free(calc->pool[calc->length]);
	}
	else if(type==2){
		free(((Array*)(calc->pool[calc->length]))->type);
		while(((Array*)(calc->pool[calc->length]))->length>0){
			delete(((Array*)(calc->pool[calc->length])));
		}
		free(((Array*)(calc->pool[calc->length]))->pool);
		path[path_count]=(Array*)(calc->pool[calc->length]);
		path_count++;
		calc->pool[calc->length]=NULL;
	}
}

int wcount(wchar_t *input){//计算字符串长度
	int i=0;
	while(input[i]!=0){
		i++;
	}
	return i;
}

int htoi(wchar_t *onstr,int start,int end){//十六进制字串转十进制数字
	int temp=0,x;
	while(start<end){
		if(onstr[start]>='0'&&onstr[start]<='9'){
			x=onstr[start]-48;
		}
		else{
			x=onstr[start]-87;
		}
		temp+=x*pow(16,end-start-1);
		start++;
	}
	return temp;
}

int getlen(wchar_t onstr){//返回每种常数数据类型对应获取长度
	int temp;
	if(onstr=='1'||onstr=='2'||onstr=='3'||onstr=='4'){
		temp=7;
	}
	else{
		temp=3;
	}
	return temp;
}

void getstr(wchar_t **p,int len,wchar_t **code,int is_loop,int *func_p){//根据来源从来源中获取对应长度的字符串
	wchar_t *linshi;
	int i=0;
	if(is_loop){
		linshi=*code;
	}
	while(i<len){
		if(is_loop){
			onget=linshi[*func_p];
			(*func_p)++;
			if(onget==0){
				break;
			}
		}
		else{
			onget=fgetc(fp);
			if(onget==' '||onget=='\n'){
				continue;
			}
			else if(onget==EOF){
				break;
			}
		}
		*p[i]=onget;
		i++;
	}
}

wchar_t* concat(wchar_t a[],wchar_t b[]){//字符串连接
	int len_a=wcount(a),len_b=wcount(b),len=len_a+len_b,i=0;
	wchar_t *temp=malloc(sizeof(wchar_t[len]));
	while(i<len_a){
		temp[i]=a[i];
		i++;
	}
	while(i<len){
		temp[i]=b[i-len_a];
		i++;
	}
	return temp;
}

wchar_t* getconst(wchar_t **code,int *length,int is_loop){//根据每种数据长度获取常量字符串，根据模式决定获取来源：代码块/文件
	wchar_t *temp,*onstr=malloc(sizeof(wchar_t*));
	getstr(&temp,1,code,is_loop,length);
	if(temp[0]=='0'){

		getstr(htoi(getstr(3,code,is_loop,length),0,3)*4,code,is_loop,length)
		temp=concat(temp,onstr);
	}
	else{
		temp=concat(temp,getstr(getlen(temp[0]),code,is_loop,length));
	}
	return temp;
}

void load_const(wchar_t input[],Array *pool){//通过获取的字符串向传入栈中加载常数
	int i=1,value_type,count=0;
	double *len=malloc(sizeof(double*));
	wchar_t *onstr;
	pool->pool=realloc(pool->pool,sizeof(void*[pool->length+1]));
	switch(input[0]){
		case '0':
		*len=wcount(input)-1;
		onstr=malloc(sizeof(wchar_t[(int)*len/2]));
		while(i<*len){
			onstr[count]=htoi(input,i,i+2);
			count++;
			i+=2;
		}
		pool->pool[pool->length]=onstr;
		value_type=0;
		break;
		case '1':
		*len=htoi(input,1,8);
		pool->pool[pool->length]=len;
		value_type=1;
		break;
		case '2':
		*len=-htoi(input,1,8);
		pool->pool[pool->length]=len;
		value_type=1;
		case '3':
		*len=htoi(input,2,8)*pow(0.1,htoi(input,1,2));
		pool->pool[pool->length]=len;
		value_type=1;
		break;
		case '4':
		*len=-htoi(input,2,8)*pow(0.1,htoi(input,1,2));
		pool->pool[pool->length]=len;
		value_type=1;
		break;
		case '5':
		*len=htoi(input,1,4);
		pool->pool[pool->length]=len;
		value_type=1;
		break;
		case '6':
		*len=-htoi(input,1,4);
		pool->pool[pool->length]=len;
		value_type=1;
		break;
		default:
		break;
	}
	pool->type=realloc(pool->type,sizeof(int[pool->length+1]));
	pool->type[pool->length]=value_type;
	(pool->length)++;
	free(input);
}

void push_const(Array *calc,Array *pool,wchar_t **code,int *func_p,int is_loop){//从常量池中读取常量并推入栈中
	load_const(getconst(code,func_p,is_loop),calc);
	calc->length--;
	double *num=(double*)calc->pool[calc->length];
	calc->pool[calc->length-1]=pool->pool[(int)*num];
	calc->type[calc->length-1]=pool->type[(int)*num];
	free(calc->pool[calc->length]);
}

void push_var(Array *calc,Array *var,wchar_t **code,int *func_p,int is_loop){//从变量池中读取变量并推入栈中
	load_const(getconst(code,func_p,is_loop),calc);
	calc->length--;
	calc->type[calc->length]=var->type[(int)*(double*)(calc->pool[calc->length])];
	calc->pool[calc->length]=var->pool[(int)*(double*)(calc->pool[calc->length])];
	calc->length++;
}

void store_var(Array *calc,Array *var,wchar_t **code,int *func_p,int is_loop){//存储变量
	load_const(getconst(code,func_p,is_loop),calc);
	calc->length--;
	double *num=(double*)(calc->pool[calc->length]);
	calc->length--;
	var->pool[(int)*num]=calc->pool[calc->length];
	var->type[(int)*num]=calc->type[calc->length];
	calc->length++;
	free(calc->pool[calc->length]);
}

void print_value(Array *calc,int index,int is_base){//数据输出
	int i=0,max;
	switch(calc->type[index]){
	case -1:
	printf("null");
	break;
	case 0://字符串
	printf("%ls",(wchar_t*)calc->pool[index]);
	break;
	case 1://数字
	printf("%f",*(double*)calc->pool[index]);
	break;
	case 2://数组，递归显示
	max=((Array*)calc->pool[index])->length;
	printf("[" );
	while(i<max){
		print_value((Array*)calc->pool[index],i,false);
		printf(",");
		i++;
	}
	printf("]");
	break;
	}
	delete(calc);
	if(is_base){
		printf("\n" );
	}
}

void frame_gc(Array *pool,Array *calc,Array *var,int is_loop){//垃圾释放与回收
	if(!is_loop){
		while(pool->length>0){
			delete(pool);
		};
		free(pool->type);
		free(pool->pool);
		free(pool);
		while(calc->length>0){
			delete(calc);
		}
		free(calc->pool);
		free(calc->type);
		free(calc);
		while(var->length>0){
			delete(var);
		}
		free(var->pool);
		free(var->type);
		free(var);
	}
}

void service(Array *calc,wchar_t **code,int *func_p,int is_loop){//根据常数开启各种服务
	load_const(getconst(code,func_p,is_loop),calc);
	calc->length--;
	double *num=(double*)calc->pool[calc->length];
	free(calc->pool[calc->length]);
	switch((int)*num){
		case 1:break;
		case 2:break;
		case 3:
		print_value(calc,calc->length-1,true);//显示服务
		break;
	}
}

void change_var_item(Array *calc,Array *var,wchar_t **code,int *func_p,int is_loop){
	load_const(getconst(code,func_p,is_loop),calc);
	(calc->length)--;
	double *num=(double*)calc->pool[calc->length];
	delete(calc);
	if((int)*num>=((Array*)(var->pool[(int)*num]))->length){
		throw_err(0);
	}
	else{
		var->pool[(int)*num]=calc->pool[calc->length];
	}
}

void push_Array(Array *calc){//创建一个数组数据，并将引用入栈
	Array *temp;
	if(path_count>0){
		path_count--;
		temp=path[path_count];//这里的指针析构时将地址保存下来，下次添加优先分配保存的地址
	}
	else{
		temp=malloc(sizeof(Array));
		temp->length=0;
		temp->type=malloc(sizeof(int[1]));
		temp->pool=malloc(sizeof(void*[1]));
	}
	calc->pool[calc->length]=temp;
	calc->type[calc->length]=2;
	calc->length++;
}

void loop(Array *calc,wchar_t **code,int *func_p,int is_loop,Array *var,Array *pool){
	int kh=1,length=0;
	wchar_t *block=malloc(sizeof(wchar_t*)),*onstr=NULL;
	while(onget!=EOF){
		free(onstr);
		onstr=getstr(4,code,is_loop,func_p);
		if(wcmp(onstr,L"0x14")){
			kh++;
		}
		else if(wcmp(onstr,L"0x1d")){
			kh--;
		}
		if(kh==0){
			for(int i=0;i<4;i++){
				block=realloc(block,sizeof(wchar_t[length+2]));
				block[length]=onstr[i];
				length++;
			}
			break;
		}
		for(int i=0;i<4;i++){
			block=realloc(block,sizeof(wchar_t[length+2]));
			block[length]=onstr[i];
			length++;
		}
	}//循环获取代码块
	calc->length--;
	double *num=(double*)calc->pool[calc->length];//获取循环条件值
	if(*num){//为真
		fvm(&block,pool,var,calc,true);
	}
	free(calc->pool[calc->length]);
	free(block);
}

void end_loop(Array *calc,int *func_p){
	calc->length--;
	double *num=(double*)calc->pool[calc->length];
	free(calc->pool[calc->length]);
	if(*num){//满足执行
		*func_p=0;
	}
}

void calculate(char sign,Array *pool){//计算函数，根据指令解析对传入栈中的2个/1个数据进行相应运算
	void *temp=malloc(sizeof(double)),*op1,*op2;
	(pool->length)--;
	op2=pool->pool[pool->length];
	(pool->length)--;
	op1=pool->pool[pool->length];
	double *num=malloc(sizeof(double));
	wchar_t *onstr=malloc(sizeof(wchar_t[1]));
	switch(sign){
		case '+':
		if(pool->type[pool->length]==0){
			free(temp);
			temp=concat((wchar_t*)op1,(wchar_t*)op2);
			sign=0;
		}
		else if(pool->type[pool->length]){
			*num=*(double*)op1+*(double*)op2;
			temp=num;
			sign=1;
		}
		break;
		case '-':
		*num=*(double*)op1-*(double*)op2;
		temp=num;
		sign=1;
		break;
		case '*':
		*num=*(double*)op1**(double*)op2;
		temp=num;
		sign=1;
		break;
		case '/':
		*num=*(double*)op1/ *(double*)op2;
		temp=num;
		sign=1;
		break;
		case '%':
		*num=*(double*)op1-(int)(*(double*)op1/ *(double*)op2)**(double*)op2;
		temp=num;
		sign=1;
		break;
		case '=':
		if(pool->type[pool->length]==0){
			*num=wcmp((wchar_t*)op1,(wchar_t*)op2);
		}
		else if(pool->type[pool->length]){
			*num=*(double*)op1==*(double*)op2;
		}
		temp=num;
		sign=1;
		break;
		case '>':
		*num=*(double*)op1>*(double*)op2;
		temp=num;
		sign=1;
		break;
		case '<':
		*num=*(double*)op1<*(double*)op2;
		temp=num;
		sign=1;
		break;
		case '|':
		*num=*(double*)op1||*(double*)op2;
		temp=num;
		sign=1;
		break;
		case '&':
		*num=*(double*)op1&&*(double*)op2;
		temp=num;
		sign=1;
		break;
		case '[':
		*num=*(double*)op2;
		sign=pool->type[(int)*num];
		if(sign==0){
			if(*num>=wcount((wchar_t*)op1)){
				throw_err(0);
			}
			else{
				onstr[0]=((wchar_t*)op1)[(int)*num];
				temp=onstr;
			}
		}
		else if(sign==2){
			if(*num>=((Array*)op1)->length){
				throw_err(0);
			}
			else{
				temp=((Array*)op1)->pool[(int)*num];
				sign=((Array*)op1)->type[(int)*num];
			}
		}
		break;
		case ']':
		*num=*(double*)op1;
		(pool->length)--;
		temp=pool->pool[pool->length];
		if(*num>=((Array*)temp)->length){
			throw_err(0);
		}
		else{
			delete(((Array*)temp)->pool[(int)*num]);
			((Array*)temp)->pool[(int)*num]=op2;
			((Array*)temp)->type[(int)*num]=pool->type[pool->length+2];
		}
		break;
		case 'a':
		((Array*)op1)->pool[((Array*)op1)->length]=op2;
		((Array*)op1)->type[((Array*)op1)->length]=pool->type[pool->length+1];
		((Array*)op1)->length++;
		break;
	}
	if(sign!='a'&&sign!=']'){
		pool->pool[pool->length]=temp;
		pool->type[pool->length]=sign;
	}
	(pool->length)++;
	free(onstr);
}

void fvm(wchar_t **code,Array *last_pool,Array *last_var,Array *last_calc,int is_loop){//虚拟机函数体，参数为(代码块),(上个常量池),(上个变量池),(上个计算栈),是否为循环状态
	wchar_t *onstr=NULL,*block=NULL;
	onget='0';
	double *num=NULL;
	void *vp=NULL;
	int kh,length=0,func_p=0;
	Array *pool=NULL,*calc=NULL,*var=NULL,*temp=NULL;
	if(last_pool==NULL){//如果执行状态为函数
		pool=malloc(sizeof(Array)),calc=malloc(sizeof(Array)),var=malloc(sizeof(Array));
		pool->length=0;
		pool->pool=malloc(sizeof(void*[1]));
		pool->type=malloc(sizeof(int[1]));
		calc->length=0;
		calc->pool=malloc(sizeof(void*[1]));
		calc->type=malloc(sizeof(int[1]));
		var->length=0;
		var->pool=malloc(sizeof(void*[1]));
		var->type=malloc(sizeof(int[1]));
	}
	else{//执行状态为代码块
		pool=last_pool,var=last_var,calc=last_calc;
	}
	while(onget!=EOF){//循环到文件读取完毕
		onstr=getstr(4,code,is_loop,&func_p);//获取指令
		if(onget==EOF){
			break;
		}
		switch(htoi(onstr,2,4)){
			case 0x1:
			calculate('+',calc);
			break;
			case 0x2:
			calculate('-',calc);
			break;
			case 0x3:
			calculate('*',calc);
			break;
			case 0x4:
			calculate('/',calc);
			break;
			case 0x5:
			calculate('%',calc);
			break;
			case 0x6:
			load_const(getconst(code,&func_p,is_loop),calc);
			break;
			case 0x7:
			calculate('[',calc);
			break;
			case 0x8:
			push_Array(calc);
			break;
			case 0x9:
			push_var(calc,var,code,&func_p,is_loop);
			break;
			case 0xa:
			store_var(calc,var,code,&func_p,is_loop);
			break;
			case 0xb:
			break;
			case 0xc:
			calculate('=',calc);
			break;
			case 0xd:
			*(double*)pool->pool[pool->length-1]=!*(double*)pool->pool[pool->length-1];
			break;
			case 0xe:
			calculate('>',calc);
			break;
			case 0xf:
			calculate('<',calc);
			break;
			case 0x10:
			calculate('|',calc);
			break;
			case 0x11:
			calculate('&',calc);
			break;
			case 0x12:
			push_var(calc,global,code,&func_p,is_loop);
			break;
			case 0x13:
			store_var(calc,global,code,&func_p,is_loop);
			break;
			case 0x14:
			loop(calc,code,&func_p,is_loop,var,pool);
			break;
			case 0x15:
			push_const(calc,pool,code,&func_p,is_loop);
			break;
			case 0x16:
			service(calc,code,&func_p,is_loop);
			break;
			case 0x17:
			calculate(']',calc);
			break;
			case 0x18:
			change_var_item(calc,var,code,&func_p,is_loop);
			break;
			case 0x19:
			calculate('a',calc);
			break;
			case 0x1c://弹出运算栈栈顶元素
			delete(calc);
			calc->length--;
			break;
			case 0x1d://循环结束
			end_loop(calc,&func_p);
			break;
			default:
			exit(1);
		}
	}

	//资源释放和垃圾回收
	frame_gc(pool,calc,var,is_loop);
	free(onstr);
}

int main(int argc,char* argv[]){
	path=malloc(sizeof(Array));
	path_count=0;
	global=malloc(sizeof(Array));
	global->length=0;
	global->type=malloc(sizeof(int[1]));
	global->pool=malloc(sizeof(void*[1]));
	args=argv;
	fp=fopen(argv[1],"r");
	if(fp==NULL){
		exit(1);
	}
	fvm(NULL,NULL,NULL,NULL,false);//初始化结束，开始执行虚拟机
	fclose(fp);
	fp=NULL;
}