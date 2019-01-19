import MFtoken
import os
import sys
if(sys.platform=="linux"or sys.platform=="darwin"):
	path=os.getcwd()+"/"
else:
	path=os.getcwd()+"\\"
path+=input("请输入文件名\n")
with open(path) as f:
	code=f.read()
function_list={}
var_list={}

var_index=0
start_point=0
iserr=False

rank={'+':5,'-':5,'*':6,'/':6,'%':6,'+=':0,'-=':0,'*=':0,'/=':0,
	'=':0,'==':4,'!=':4,'>':4,'<':4,'>=':4,'<=':4,'&&':3,'||':2,
	',':1,'.':7,':':1}

logic={'>','<','==','!=','>=','<=','&&','||'}
numeric={'+','-','*','/','%','+=','-=','*=','/='}

bool_trans={'true':1,'false':0}

cal_trans={'+':"add",'-':"sub",'*':"mul",'/':"div",'%':'mod',
	'>':"gt",'<':"lt",'==':"eq",'>=':'lt','<=':'gt','!=':"eq","+=":'add',
	'-=':'sub',"*=":'mul',"/=":'div','&&':"and",'||':"or"}

class var:
	def __init__(self,form,base,indexs=[]):
		self.type=form
		self.base=base
		self.indexs=indexs
	def __repr__(self):
		back="base:"+str(self.base)+" sub_index:"
		for x in self.indexs:
			back+=str(x)+" "
		return self.type+" "+back

class function:
	def __init__(self,args,stp,form="null"):
		self.args=args
		self.start=stp
		self.type=form
	def out(self):
		print("开始点："+str(self.start))
		print(self.args)

def do():
	return

def reverse(cin):
	linshi=[]
	for x in range(len(cin)-1,-1,-1):
		linshi.append(cin[x])
	return linshi

def define(input_list,on_line):
	global var_index
	on_type=""
	on_name=""
	cal_list=[]
	for x in range(len(input_list)-1,-1,-1):
		if(input_list[x].type in MFtoken.switchs or input_list[x].type[0]=="G"):
			on_type=input_list[x].type
			on_name=input_list[x].value
			var_list[on_name]=var(on_type,var_index)
			var_index+=1
		cal_list.append(input_list[x])
	
	out_code=calculate(cal_list,on_line,"")
	return out_code

def calculate(input_list,on_line,out_code):
	isdef=False
	global var_list
	stack=[]
	global iserr
	rt=""
	on_type=""
	for x in input_list:
		if(x.type=="op"):
			try:
				x2=stack.pop()
				x1=stack.pop()
				pass
			except IndexError as e:
				print("第"+str(on_line+1)+"行："+code[on_line]+"\n错误：运算符'"+x.value+"'缺少操作数")
				iserr=True
				exit()
			if(x.value!="@"and x.value!=","):
				if(isdef):
					isdef=False
					out_code+=f"ldv {x1.value} 1\n"
				elif(x1.type in MFtoken.switchs):
					out_code+=f"ldc {x1.value} 1\n"
				elif(x1.type[0:2]=="r-"):
					out_code+="mov 3 1\n"
				elif(x1.type!="function"):
					out_code+=f"ldv {x1.value} 1\n"
				if(x2.type in MFtoken.switchs):
					out_code+=f"ldc {x2.value} 2\n"
				elif(x2.type[0:2]=="r-"):
					out_code+="mov 3 2\n"
				elif(x2.type!="function"):
					out_code+=f"ldv {x2.value} 2\n"
				if(x.value in {'+=','*=','-=','/='}):
					out_code+=cal_trans[x.value]+"\n"
				elif(x.value=='='):
					out_code+="st\n"
				elif(x.value in cal_trans):
					out_code+=cal_trans[x.value]+"\n"
					if(x.value in {">=","<=","!="}):
						out_code+="not\n"
				if(x.value in logic):
					on_type="bool"
				elif(x1.type=="str"or x1.type=="r-str"):
					on_type="str"
				elif(x.value in numeric):
					on_type="float"
			else:
				if(x.value=="@"):
					i=0
					if(x2.type!="args"):
						x2=MFtoken.token("args",[x2])
					if(len(x2.value)<len(function_list[x1.value].args)):
						print("第"+str(on_line+1)+"行："+code[on_line]+f"\n错误：函数'{x1.value}'所需要的参数个数为"+str(len(function_list[x1.value].args))+"个，但只传入了"+str(len(x2.value))+"个")
						iserr=True
						exit()
					for x in range(0,len(function_list[x1.value].args)):
						if(function_list[x1.value].args[x].type!="auto"):
							if(x2.value[i].type!=function_list[x1.value].args[x].type):
								print("第"+str(on_line+1)+"行："+code[on_line]+"\n错误：函数'"+x1.value+"'参数"+str(x+1)+"所传入的类型'"+x2.value[i].type+"'与所定义的类型'"+function_list[x1.value].args[x].type+"'不符")
								iserr=True
								exit()
						if(x2.value[i].type=="var"):
							out_code+="ldv "+x2.value[i].value+" 2\nldc "+str(-1*(x+1))+" 1\nst\n"
						elif(x2.value[i].type in {'int','float','bool','str'}):
							out_code+="ldc "+x2.value[i].value+" 2\nldc "+str(-1*(x+1))+" 1\nst\n"
						i+=1
					out_code+=f"go {function_list[x1.value].start}\n"
				elif(x.value==","):
					if(type(x1)==list):
						x1.append(x2)
						stack.append(x1)
					else:
						stack.append(MFtoken.token("args",[x1,x2]))
			stack.append(MFtoken.token("r-"+on_type,"3"))
		else:
			if(x.value in bool_trans):
				x.value=str(bool_trans[x.value])
			elif(x.type=="var"):
				if(x.value in var_list):
					x.value=str(var_list[x.value].base)
				elif(x.value in function_list):
					x.type="function"
				else:
					iserr=True
					print(f"第{line}行：{code[line-1]}\n错误：变量'{x.value}'未定义")
					exit()
			elif(x.value[0]not in MFtoken.num and x.value[0]not in {'"',"'"}):
				isdef=True
				x.value=str(var_list[x.value].base)
			stack.append(x)
	if(isdef==False and stack[-1].type in MFtoken.switchs):
		out_code+="ldc "+str(stack.pop().value)+" 3\n"
	return out_code


tokens=MFtoken.parse(code)

class box:
	def __init__(self,name):
		self.name=name
		self.sub=[]

def transexp(tokens):
	isglobal=False
	on_type=""
	out_code=""
	stack=[]
	out=[]
	i=0
	while(i<len(tokens)):
		on=tokens[i]
		if(on.value=='('or on.type=="keyword"):
			stack.append(on)
		elif(on.value==')'):
			while(len(stack)>0):
				if(stack[len(stack)-1].value=='('):
					break
				out.append(stack.pop())
			if(stack.pop().type=="call"):
				out.append(MFtoken.token("op","@"))
		elif(on.type=="switch"):
			if(on.value=="global"):
				isglobal=True
			else:
				on_type=on.value
		elif(on.type=="sign"):
			if(isglobal):
				out.append(MFtoken.token("G"+on_type,on.value))
				isglobal=False
			else:
				out.append(MFtoken.token(on_type,on.value))
			on_type=""
		elif(on.value=='\n'):
			while(len(stack)>0):
				out.append(stack.pop())
			out.append(on)
		elif(on.value==';'):
			while(len(stack)>0):
				out.append(stack.pop())
		elif(on.type=="op"):
			if(len(stack)==0):
				stack.append(on)
			elif(stack[len(stack)-1].value=='('or stack[len(stack)-1].type=="keyword"):
				stack.append(on)
			elif(rank[on.value]>rank[stack[len(stack)-1].value]):
				stack.append(on)
			else:
				while(len(stack)>0):
					if(stack[len(stack)-1].type=="keyword"or stack[len(stack)-1].value=='('):
						break
					elif(rank[on.value]>=rank[stack[len(stack)-1].value]):
						break
					out.append(stack.pop())
				stack.append(on)
		else:
			out.append(on)
		i+=1
	while(len(stack)>0):
		out.append(stack.pop())
	return out
def fold(out):
	i=0
	temp=[]
	stack=[]
	while(i<len(out)):
		if(out[i].type=="keyword"):
			temp.append(box(out[i].value))
			while(len(stack)>0):
				temp[len(temp)-1].sub.append(stack.pop())
			i+=1
		elif(out[i].value=='\n'):
			temp.append(box("exp"))
			while(len(stack)>0):
				temp[len(temp)-1].sub.append(stack.pop())
		else:
			stack.append(out[i])
		i+=1
	return temp
start_point=0
line=0
funcs=[]
def compile(temp,st=0,yx=True,ed=0):
	i=0
	is_loop=False
	global line,start_point,var_list
	out_code=""
	breaks=0
	while(i<len(temp)):
		if(yx):
			line+=1
		if(len(temp[i].sub)>0):
			if(temp[i].name=="func"):
				k=len(temp[i].sub)-2
				v_c=0
				on_list=[]
				while(k>-1):
					if(temp[i].sub[k].type=="op"):
						if(temp[i].sub[k].value!=","):
							print(f"第{line}行：{code[i]}\n错误：非法符号'{temp[i].sub[k].value}'")
					else:
						v_c+=1
						on_list.append(temp[i].sub[k])
						var_list[temp[i].sub[k].value]=var(temp[i].sub[k].type,str(-1*v_c))
					k-=1
				kh=1
				name=temp[i].sub[-1].value
				function_list[name]=function(on_list,start_point+1,temp[i].sub[-1].type)
				on_list=[]
				while(i<len(temp)):
					i+=1
					if(temp[i].name=="func"):
						kh+=1
					elif(temp[i].name=="endfunc"):
						kh-=1
					if(kh==0):
						break
					on_list.append(temp[i])
				if(kh!=0):
					print(f"第{line}行：{code[i-1]}\n错误：缺少'endfunc'语句")
					iserr=True
					exit()
				linshi=compile(on_list,start_point)[0]
				if(linshi.split("\n")[-2]!="rt"):
					linshi+="rt\n"
				for x in function_list[name].args:
					del var_list[x.value]
				i+=1
				line+=1
				funcs.append(linshi)
				start_point+=len(funcs[-1].split("\n"))
				continue
			elif(temp[i].name=="if"):
				linshi=reverse(temp[i].sub)
				on_list=[]
				kh=1
				es=False
				while(i<len(temp)):
					i+=1
					if(temp[i].name=="if"):
						kh+=1
					elif(temp[i].name=="else" and es==False):
						esp=True
					elif(es):
						print(f"第{line}行：{code[i-1]}\n错误：'else'语句无配对的if-endif语句")
						iserr=True
						exit()
					elif(temp[i].name=="endif"):
						kh-=1
					if(kh==0):
						break
					on_list.append(temp[i])
				if(kh!=0):
					print(f"第{line}行：{code[i-1]}\n错误：缺少'endif'语句")
					iserr=True
					exit()
				out_code+=calculate(linshi,line,out_code)
				linshi=compile(on_list,start_point)[0]
				start_point+=len(linshi.split("\n"))
				out_code+="fjp "+str(start_point)+linshi+"\n"
				continue
			elif(temp[i].name=="while"):
				linshi=reverse(temp[i].sub)
				on_list=[]
				kh=1
				while(i<len(temp)):
					i+=1
					if(temp[i].name=="while"):
						kh+=1
					elif(temp[i].name=="endwhile"):
						kh-=1
					if(kh==0):
						break
					on_list.append(temp[i])
				if(kh!=0):
					print(f"第{line}行：{code[i-1]}\n错误：缺少'endwhile'语句")
					iserr=True
					exit()
				last=start_point-1
				cond=calculate(linshi,line,"")
				out_code+=cond
				linshi=compile(on_list,start_point,False)
				start_point+=len(linshi[0].split("\n"))+len(cond.split("\n"))+linshi[2]-3
				linshi=compile(on_list,last,True,start_point)
				out_code+="fjp "+str(start_point-2)+"\n"+linshi[0]+"jp "+str(last)+"\n"
				continue
			elif(temp[i].name=="return"):
				linshi=calculate(reverse(temp[i].sub),line,"")+"rt\n"
				start_point+=len(linshi.split("\n"))
				out_code+=linshi
			elif(temp[i].sub[len(temp[i].sub)-1].type in MFtoken.switchs or temp[i].sub[len(temp[i].sub)-1].type[0]=="G"):
				linshi=define(temp[i].sub,i)
				start_point+=len(linshi.split("\n"))
				out_code+=linshi
			elif(temp[i].name=="exp"):
				linshi=calculate(reverse(temp[i].sub),line,"")
				start_point+=len(linshi.split("\n"))
				out_code+=linshi
			elif(temp[i].name=="import"):
				menu=os.listdir(os.getcwd())
				for x in temp[i].sub:
					if(x.value not in menu):
						print("第"+str(line)+"行："+code[i]+"\n错误：'"+x.value+"'库不在当期目录下")
						iserr=True
						exit()
					else:
						if(sys.platform=="darwin"or sys.platform=="linux"):
							path=os.getcwd()+"/"+x.value+"/"
						else:
							path=os.getcwd()+"\\"+x.value+"\\"
						files=os.listdir(path)
						with open(path+"info") as f:
							info=f.read().split("\n")
						for y in files:
							if(y=="info" or y==".DS_Store"):
								continue
							args=[]
							k=0
							info[i+1]=info[i+1].split(" ")
							while(k<len(info[i+1])):
								args.append(MFtoken.token(info[i+1][k],info[i+1][k+1]))
								k+=2
							with open(path+y) as f:
								im_code=f.read()
							info[i]=info[i].split(" ")
							if(info[i][0] in MFtoken.switchs):
								function_list[info[i][1]]=function(args,start_point,info[i][0])
							else:
								function_list[info[i][0]]=function(args,start_point)
							start_point+=len(im_code.split("\n"))
							funcs.append(im_code)
		else:
			if(temp[i].name=="return"):
				out_code+="rt\n"
			elif(temp[i].name=="break"):
				if(yx):
					out_code+="jp "+str(ed)+"\n"
				else:
					breaks+=1
			elif(temp[i].name=="continue"):
				out_code+="jp "+str(st)+"\n"
		i+=1
		start_point+=1
	return out_code,funcs,breaks
code=code.split("\n")
back=compile(fold(transexp(tokens)))
code=back[0]
start_point=0
for x in back[1]:
	code=x+code
	start_point+=len(x.split("\n"))

code="fjp "+str(start_point)+"\n"+code