def 0;
def 1;
def 3;
make_block;函数 阶乘(阶乘数)
push_const 1;输出=1
make_block_for_loop;当阶乘数>1
push_var 0;
imul;
push_var 0;
push_const 1;
isub;
change_var 0;
push_var 0;
push_const 1;
i_greater;
end_loop;
push_var 0;
push_const 1;
i_greater;
loop;开始循环
Return;返回结果
end_block;
push_const 1;
make_func;
push_fast 4;求阶乘数入栈
push_const 2;
call;调用阶乘
print;输出结果