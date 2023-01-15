// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
using namespace std;
/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

/*
    正则定义：
    token->保留字|标识符|常数|界符|运算符
    保留字->auto|……|while
    标识符->(_|letter)(letter|digit)*
    常数->(digit)(digit)*(.(digit)(digit)*|E)
    界符->(|)|{|}|[|]|'|"|,|.|:|;   这里把注释归为运算符，因为它是/开头
    运算符->+|-|\*|\/|<|>|=|!|&|^|~|||%|?|>=|<=|//|\*\/ |等等  这里在测试中发现%d一类被认为了标识符（……），这一类标识符在运算符中判定
*/

//Token结构体
struct Token{
    int number;//个数
    string type;//类型
    int lab;//编号
};

int CheckFirst(char first);//识别Token首个字符
int IdentifierOrReserved(string& prog,int cur);//识别标识符/保留字
int Number(string& prog,int cur);//识别常数
int Delimiter(string& prog,int cur);//识别界符
int Operator(string& prog, int cur);//识别操作符
void perror(string& prog);//错误处理函数

Token* TokenInstance;//存放当前读取的Token
int ReadPtr;//当前读取位置



/*
正则定义第一行：
    token->保留字|标识符|常数|界符|运算符
根据首字符判定token的类型。其中保留字和标识符结构相似，归作一类。

如果是字母或者下划线开头，认为是第一类
  补充：在测试后发现还有%开头，这类标识符我们算作第四类，因为由%开头

如果是数字，我们认为是第二类（常数）

如果是界符，我们认为是第三类。
  补充：注意到注释是由/开头，和运算符/的首字符重复，这里注释归纳到了运算符中去解决。
        这样也可以将所有界符设定为单字符，从而设为一个字符串

如果是运算符，我们认为是第四类。

如果以上都不是，抛出错误，返回错误码，即第五类字符。
*/
int CheckFirst(char first)
{
   if ((first>= 'a' && first<= 'z') || (first>= 'A' && first<= 'Z') || first== '_')//检测是否是标识符或是保留字
     return 1;//第一类

   if(first>= '0' && first<= '9')//检测是否是常数
    return 2;//第二类

    //其余情况
    string Delimiters="(){}[]'\",.:;";//界符可能的first

    //检测是否为界符
    for(int i=0;i<12;i++)
    {
        if(first==Delimiters[i])
            return 3;
    }
    //创建运算符可能的第一位
    string Operators="+-*/<>=!&^~|%?";

    //是否为运算符
    for(int i=0;i<14;i++)
    {
        if(first==Operators[i])
            return 4;
    }
    return 5;
}

//识别该标识符或保留字token是否合法，并写入token实例
int IdentifierOrReserved(string& prog,int cur)
{
     string TokenString;
     //将该token全部放在TokenString
     while((prog[ReadPtr]>= 'a' && prog[ReadPtr]<= 'z') || (prog[ReadPtr]>= 'A' && prog[ReadPtr]<= 'Z') || prog[ReadPtr]== '_')
     {
         TokenString+=prog[ReadPtr++];
     }
     //一一遍历是否为保留字
     string KeyWord[32] = {"auto", "break", "case", "char", "const", "continue","default", "do", "double", "else", "enum", "extern",
      "float", "for", "goto", "if", "int", "long","register", "return", "short", "signed", "sizeof", "static",
      "struct", "switch", "typedef", "union", "unsigned", "void","volatile", "while"
     };


     int flag=1;//标识是否为保留字
     for(int j=0;j<32&&flag;j++)
     {

         if(TokenString == KeyWord[j])
            {
                flag=0;
                TokenInstance->lab=j+1;
            }

     }
     //如果是保留字
     if(!flag)
     {
        TokenInstance->number = cur;
        TokenInstance->type = TokenString;
     }
     //否则为标识符
     else
    {
        TokenInstance->number=cur;
        TokenInstance->type=TokenString;
        TokenInstance->lab=81;
     }
     return 1;
}

//识别该常数token是否合法，并写入token实例
int Number(string& prog,int cur)
{
    string TokenString;
    int flag=1;//标识flag是否出现
    while((prog[ReadPtr]>='0'&&prog[ReadPtr]<='9')||(prog[ReadPtr]=='.'&&flag))
    {
        if(prog[ReadPtr]!='.')
            TokenString+=prog[ReadPtr++];
        else
        {
            TokenString+=prog[ReadPtr++];
            flag=0;
        }
    }

    TokenInstance->number=cur;
    TokenInstance->type=TokenString;
    TokenInstance->lab=80;
    return 1;
}

//识别该界符token是否合法，并写入token实例
int Delimiter(string& prog,int cur)
{
     string TokenString;
     //判断是哪一个界符，从而给到正确的编号
     if(prog[ReadPtr]=='(')
        TokenInstance->lab=44;
     else if(prog[ReadPtr]==')')
        TokenInstance->lab=45;
     else if(prog[ReadPtr]==',')
        TokenInstance->lab=48;
     else if(prog[ReadPtr]=='.')
        TokenInstance->lab=49;
     else if(prog[ReadPtr]==':')
        TokenInstance->lab=52;
     else if(prog[ReadPtr]==';')
        TokenInstance->lab=53;
     else if(prog[ReadPtr]=='[')
        TokenInstance->lab=55;
     else if(prog[ReadPtr]==']')
        TokenInstance->lab=56;
     else if(prog[ReadPtr]=='{')
        TokenInstance->lab=59;
     else if(prog[ReadPtr]=='}')
        TokenInstance->lab=63;
     else if(prog[ReadPtr]=='\"')
        TokenInstance->lab=78;

     TokenString+=prog[ReadPtr++];
     TokenInstance->type=TokenString;
     TokenInstance->number=cur;
     return 1;

}

//按照编号从小到大的顺序去识别是哪一种操作符，或注释、占位符
int Operator(string& prog,int cur)
{
   string TokenString;
     //-开头，-（33），--（34），-=（35），->(36)
   if(prog[ReadPtr]=='-')
   {
       ReadPtr++;
       if(prog[ReadPtr]=='-')
       {
           TokenInstance->lab=34;
           TokenString="--";
           ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
           TokenInstance->lab=35;
           TokenString="-=";
           ReadPtr++;
       }
       else if(prog[ReadPtr]=='>')
       {
           TokenInstance->lab=36;
           TokenString="->";
           ReadPtr++;
       }
       else
       {
           TokenInstance->lab=33;
           TokenString="-";
       }
   }

   //!开头，!（37），!=（38）
   else if(prog[ReadPtr]=='!')
   {
       ReadPtr++;
       if(prog[ReadPtr++]=='=')
       {
           TokenInstance->lab=38;
           TokenString="!=";
           ReadPtr++;
       }
       else
       {
           TokenInstance->lab=37;
           TokenString="!";
       }
   }

   //%开头，%（39），%=（40），这里根据测试知道还有%开头的占位符，占位符算作标识符。
   else if(prog[ReadPtr]=='%')
   {
       ReadPtr++;
       if(prog[ReadPtr]=='=')
       {
           TokenInstance->lab=40;
           TokenString="%=";
           ReadPtr++;
       }
       else
       {
        TokenInstance->lab=39;
        TokenString="%";
        if(prog[ReadPtr]=='c'||prog[ReadPtr]=='f'||prog[ReadPtr]=='d'||prog[ReadPtr]=='x'||prog[ReadPtr]=='u'||prog[ReadPtr]=='o')
        {
            TokenString+=prog[ReadPtr++];
            TokenInstance->lab=81;
        }
       }
   }

   //&开头，&（41），&&（42），&=（43）
   else if(prog[ReadPtr]=='&')
   {
       ReadPtr++;
        if(prog[ReadPtr]=='&')
       {
           TokenInstance->lab=42;
           TokenString="&&";
           ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
            TokenInstance->lab=43;
            TokenString="&=";
       }
       else
       {
           TokenInstance->lab=41;
           TokenString="&";
       }
   }
   //*开头，*（46），*=（47）
  else if(prog[ReadPtr]=='*')
  {
    ReadPtr++;
    if(prog[ReadPtr]=='=')
    {
        TokenInstance->lab=47;
        TokenString="*=";
        ReadPtr++;
    }
    else
    {
        TokenInstance->lab=46;
        TokenString="*";
    }
  }

  // /开头，/（50），/=(51),//(79),/*(79)
  else if(prog[ReadPtr]=='/')
  {

      ReadPtr++;
      if(prog[ReadPtr]=='=')
      {
          TokenInstance->lab=51;
          TokenString="/=";
          ReadPtr++;
      }
      else if(prog[ReadPtr]=='/'||prog[ReadPtr]=='*')
      {
          TokenInstance->lab=79;
          TokenString="/";
            //如果是//注释，直到下一行为止均为注释
          if(prog[ReadPtr]=='/')
          {
              while(prog[ReadPtr]!='\n')
                TokenString+=prog[ReadPtr++];
          }
          //如果是/*注释，直到*/均为注释*/
          else
          {
              while(prog[ReadPtr]!='*'||prog[ReadPtr+1]!='/')
              TokenString+=prog[ReadPtr++];
              TokenString+="*/";
              ReadPtr+=2;
          }
      }
      else//否则为普通的/
      {
        TokenString="/";
        TokenInstance->lab=50;
      }
  }
  //?开头，?（54）
  else if(prog[ReadPtr]=='?')
  {
      TokenString="?";
      TokenInstance->lab=54;
      ReadPtr++;
  }
  //^开头^(57),^=(58)
  else if(prog[ReadPtr]=='^')
  {
      ReadPtr++;
      if(prog[ReadPtr]=='=')
      {
          TokenInstance->lab=58;
          TokenString="^=";
          ReadPtr++;
      }
      else
      {
          TokenInstance->lab=57;
          TokenString="^";
      }
  }
  //|开头，|（60），||（61），|=（62）
   else if(prog[ReadPtr]=='|')
   {
       ReadPtr++;
        if(prog[ReadPtr]=='|')
       {
           TokenInstance->lab=61;
           TokenString="||";
           ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
        TokenInstance->lab=62;
        TokenString="|=";
       }
       else
       {
           TokenInstance->lab=60;
           TokenString="|";
       }
   }
   //~开头，~（64）
   else if(prog[ReadPtr]=='~')
   {
        TokenString="~";
      TokenInstance->lab=64;
      ReadPtr++;
   }
   //+开头，+（65），++（66），+=（67）
    else if(prog[ReadPtr]=='+')
   {
       ReadPtr++;
        if(prog[ReadPtr]=='+')
       {
           TokenInstance->lab=66;
           TokenString="++";
           ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
        TokenInstance->lab=67;
        TokenString="+=";
       }
       else
       {
           TokenInstance->lab=65;
           TokenString="+";
       }
   }

   //<开头，<(68),<<(69),<<=(70),<=(71)

    else if(prog[ReadPtr]=='<')
   {
       ReadPtr++;
        if(prog[ReadPtr]=='<'&&prog[ReadPtr+1]=='=')
       {
           TokenInstance->lab=70;
           TokenString="<<=";
           ReadPtr+=2;
       }
       else if(prog[ReadPtr]=='<')
       {
        TokenInstance->lab=69;
        TokenString="<<";
        ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
           TokenInstance->lab=71;
           TokenString="<=";
           ReadPtr++;
       }
       else
       {
           TokenInstance->lab=68;
           TokenString="<";
       }
   }
   //=开头，=（72），==（73）
   else if(prog[ReadPtr]=='=')
   {
       ReadPtr++;
       if(prog[ReadPtr]=='=')
       {
           TokenInstance->lab=73;
           TokenString="==";
           ReadPtr++;
       }
       else
       {
           TokenInstance->lab=72;
           TokenString="=";
       }
   }
   //>开头，>(74),>=(75),>>(76),>>=(77)
    else if(prog[ReadPtr]=='>')
   {
       ReadPtr++;
        if(prog[ReadPtr]=='>'&&prog[ReadPtr+1]=='=')
       {
           TokenInstance->lab=77;
           TokenString=">>=";
           ReadPtr+=2;
       }
       else if(prog[ReadPtr]=='>')
       {
        TokenInstance->lab=76;
        TokenString=">>";
        ReadPtr++;
       }
       else if(prog[ReadPtr]=='=')
       {
           TokenInstance->lab=75;
           TokenString=">=";
           ReadPtr++;
       }
       else
       {
           TokenInstance->lab=74;
           TokenString=">";
       }
   }
   TokenInstance->type=TokenString;
   TokenInstance->number=cur;
   return 1;
}

//出现错误时，跳过识别该字符
//事实上该实验中没有错误
void perror(string& prog)
{
      ReadPtr++;
}

void Analysis()
{
	string prog;//prog用来保存读取的字符
	read_prog(prog);//该函数将文件中的字符全部读取到prog中
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    int cur=1;
     int cnt=1;//记录有多少个token
     TokenInstance = new Token;
     ReadPtr=0;
    while(ReadPtr<prog.length())
    {

        //跳过空字符
        while(ReadPtr<prog.length()&&(prog[ReadPtr]==' '||prog[ReadPtr]=='\n'||prog[ReadPtr]=='\t'))
            ReadPtr++;
      /*
        遇到了需要识别的字符，开始检测,从左至右一一检测该token的每一个字符
        本次实验中,识别了第一个字符就足以判断该token的类别。
        所以首先CheckFirst判别token类别，然后调用对应函数判定是否合法。
      */

      //输出Token
      if(cur!=cnt)
      cout << TokenInstance->number << ": <" << TokenInstance->type << "," << TokenInstance->lab << ">" << endl;

     cnt=cur;
     switch(CheckFirst(prog[ReadPtr]))//先分类
      {
          //保留字或标识符
         case 1:
         if(IdentifierOrReserved(prog,cur))
            cur++;//识别成功
         else
            perror(prog);//错误处理
         break;

         //常数
         case 2:
         if(Number(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //界符
         case 3:
         if(Delimiter(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //运算符
         case 4:
         if(Operator(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //出错
         default:
         perror(prog);
         break;
      }
    }
    if(cur!=1)//最后一次输出不带回车
     cout << TokenInstance->number << ": <" << TokenInstance->type << "," << TokenInstance->lab << ">";
     free(TokenInstance);
    /********* End *********/

}
