class token:
	def __init__(self,form,value):
		self.type=form
		self.value=value
	def __repr__(self):
		return self.type+" "+self.value

num="0123456789."
sym="+-*/()%!<>=|}{[].,!&:"
spe={"==","<=",">=",'!=','*=','/=','+=','-=','||','&&'}
keyword={'func','endfunc','if','else','endif','while','endwhile','continue','break','return','import'}
switchs={'int','float','string','bool','auto','global'}
empty=" 	"

def parse(text):
	i=0
	onstr=""
	temp=[]
	if(text[len(text)-1]!=' '):
		text+=' '
	while(i<len(text)):
		if(text[i] in sym):
			if(onstr!=""):
				temp.append(token("var",onstr))
				onstr=""
			if(len(temp)>0):
				if(temp[len(temp)-1].value+text[i] in spe):
					temp[len(temp)-1].value+=text[i]
				else:
					temp.append(token("op",text[i]))
			else:
				temp.append(token("op",text[i]))
		elif(text[i] in num):
			isfloat=False
			if(onstr!=""):
				onstr+=text[i]
				i+=1
				continue
			onstr=text[i]
			while(i<len(text)):
				i+=1
				if(text[i]=='.'):
					isfloat=True
				if(text[i] not in num):
					break
				onstr+=text[i]
			if(isfloat):
				temp.append(token("float",onstr))
				isfloat=False
			else:
				temp.append(token("int",onstr))
			onstr=""
			continue
		elif(text[i]=='\n'or text[i]==';'):
			if(onstr!=""):
				temp.append(token("var",onstr))
			temp.append(token("sep",text[i]))
			onstr=""
		elif(text[i] in empty):
			if(onstr!=""):
				temp.append(token("var",onstr))
			onstr=""
		elif(text[i] in {'"',"'"}):
			if(onstr!=""):
				print("字符串语法错误")
				break
			onstr=text[i]
			sy=text[i]
			while(i<len(text)):
				i+=1
				if(text[i]==sy and text[i-1]!='\\'):
					onstr+=sy
					break
				onstr+=text[i]
			temp.append(token("str",onstr))
			onstr=""
		else:
			onstr+=text[i]
		i+=1
	i=0
	while(i<len(temp)):
		if(temp[i].type=="var"):
			if(temp[i].value in keyword):
				temp[i].type="keyword"
			elif(temp[i].value in switchs):
				temp[i].type="switch"
			elif(temp[i].value in {"true","false"}):
				temp[i].type="bool"
			elif(temp[i-1].value=="func"):
				temp[i].type="sign"
		elif(temp[i].value=="="):
			if(temp[i-1].type!="var"):
				print("赋值语法错误")
				break;
			else:
				if(temp[i-1].type=="keyword"):
					print("关键字"+temp[i-1].value+"不能作为变量名")
					break;
				else:
					temp[i-1].type="sign"
		elif(temp[i].value=="."):
			temp[i-1].type="var"
			if(i+1==len(temp)):
				print(".运算符缺少运算量")
				break;
			temp[i+1].type="sub-var"
		elif(temp[i].value=="("):
			if(i>0):
				if(temp[i-1].type!="op"and temp[i-1].type!="sign"):
					temp[i].type="call"
		i+=1
	i=0
	while(i<len(temp)):
		if(temp[i].type=="var" and temp[i-1].type=="switch"):
			temp[i].type="sign"
		i+=1
	return temp