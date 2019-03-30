#汇编代码每行必须以;结束，；后的注释会被忽略，使用def 常数来设置常数，普通的指令第二个位置为常数，如push_fast

asm_to_hex={"null":"0x00",
"iadd":"0x01",
"isub":"0x02",
"imul":"0x03",
"idiv":"0x04",
"imod":"0x05",
"fadd":"0x06",
"fsub":"0x07",
"fmul":"0x08",
"fdiv":"0x09",
"fmod":"0x0a",
"sadd":"0x0b",
"iequal":"0x0c",
"fequal":"0x0d",
"sequal":"0x0e",
"Not":"0x0f",
"Or":"0x10",
"And":"0x11",
"i_greater":"0x12",
"i_less":"0x13",
"f_greater":"0x14",
"f_less":"0x15",
"call":"0x16",
"call_method":"0x17",
"loop":"0x18",
"make_block_for_loop":"0x19",
"end_loop":"0x1a",
"make_func":"0x1b",
"push_fast":"0x1c",
"push_const":"0x1d",
"push_var":"0x1e",
"dict_insert":"0x1f",
"add_var":"0x20",
"print":"0x21",
"itof":"0x22",
"ftoi":"0x23",
"itos":"0x24",
"ftos":"0x25",
"pop":"0x26",
"change_var":"0x27",
"push_int_arr":"0x28",
"push_float_arr":"0x29",
"push_Array":"0x2a",
"get_subscript":"0x2b",
"push_item":"0x2c",
"pop_item":"0x2d",
"make_block":"0x2e",
"end_block":"0x2f",
"Return":"0x30",
"change_arr_var":"0x31",
"import_api":"0x32",
"get_api":"0x33",
"call_api":"0x34",
"close_api":"0x35",
"push_file_pointer":"0x36",
"getchar_from_file":"0x37",
"write_to_file":"0x38",
"close_file":"0x39",
"get_command_line_arg":"0x3a",
"push_dict":"0x3b",
"dict_find":"0x3c",
"get_dict_subscript":"0x3d",
"remove_dict":"0x3e",
"Continue":"0x3f",
"Break":"0x40",
"if_do":"0x41",
"end_if":"0x42"}

dth=['0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f']

import math

def dtoc(num,dc=7,isfloat=False):
	temp=""
	if(not isfloat):
		if(num>0):
			temp="1"
		else:
			temp="2"
			num=-num
	onstr=hex(num)[2:]
	while(len(onstr)<dc):
		onstr="0"+onstr
	temp+=onstr
	return [temp[0:4],temp[4:8]]


def hre(hs,zc=0):
	temp=""
	i=0
	while(i<zc):
		temp+="0"
		i+=1
	return temp+hs

def stoc(s):
	temp=[]
	for x in s.encode("utf-8"):
		onstr=hex(x)[2:]
		temp.append(hre(onstr,len(onstr)==1))
	if(len(temp)%2):
		temp.append("00")
	onstr=["0"+"".join(dtoc(math.floor(len(temp)/2),3))[1:]]
	i=0
	while(i<len(temp)):
		onstr.append(temp[i]+temp[i+1])
		i+=2
	return onstr

def ftoc(num):
	temp=""
	if(num>0):
		temp="3"
	else:
		temp="4"
		num=-num
	i=0
	while(num-math.floor(num)!=0):
		num*=10
		i+=1
	onstr=temp+hex(i)[2:3]+"".join(dtoc(math.floor(num),6,True))
	return [onstr[0:4],onstr[4:8]]
	
def toconst(string):
	if(string[0]=='"'):
		return stoc(string.split('"')[1])
	elif(string.count('.')):
		return ftoc(float(string))
	else:
		return dtoc(int(string))

def build(path):
	code="1"
	const=[]
	temp=[]
	with open(path) as f:
		while(1):
			code=f.readline()
			if(code=='\n'):
				continue
			elif(code==""):
				break
			oncode=code.split(";")[0]
			if(oncode.count(" ")>0):
				index=oncode.index(" ")
				oncode=[oncode[0:index],oncode[index+1:]]
			else:
				oncode=[oncode]
			if(oncode[0]!="def"):
				temp.append(asm_to_hex[oncode[0]])
				if(len(oncode)>1):
					temp+=toconst(oncode[1])
			else:
				const.append(toconst(oncode[1]))
	onstr=[]
	for x in const:
		onstr+=x
	temp=["phnx","".join(dtoc(len(const),4))[1:5]]+onstr+temp
	onstr=""
	i=0
	for x in temp:
		onstr+=x
		if(i==7):
			onstr+='\n'
			i=-1
		else:
			onstr+=" "
		i+=1
	i=len(path)-1
	while(i>0):
		if(path[i]=='.'):
			break
		i-=1
	with open(path[0:i]+".pc","w") as f:
		f.write(onstr)
	print("build-finish")

if __name__=="__main__":
	import sys,os
	for i in range(1,len(sys.argv)):
		build(sys.argv[i])
