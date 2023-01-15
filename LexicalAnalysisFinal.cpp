// C���Դʷ�������
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
/* ��Ҫ�޸������׼���뺯�� */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* ���������������� */

/*
    �����壺
    token->������|��ʶ��|����|���|�����
    ������->auto|����|while
    ��ʶ��->(_|letter)(letter|digit)*
    ����->(digit)(digit)*(.(digit)(digit)*|E)
    ���->(|)|{|}|[|]|'|"|,|.|:|;   �����ע�͹�Ϊ���������Ϊ����/��ͷ
    �����->+|-|\*|\/|<|>|=|!|&|^|~|||%|?|>=|<=|//|\*\/ |�ȵ�  �����ڲ����з���%dһ�౻��Ϊ�˱�ʶ��������������һ���ʶ������������ж�
*/

//Token�ṹ��
struct Token{
    int number;//����
    string type;//����
    int lab;//���
};

int CheckFirst(char first);//ʶ��Token�׸��ַ�
int IdentifierOrReserved(string& prog,int cur);//ʶ���ʶ��/������
int Number(string& prog,int cur);//ʶ����
int Delimiter(string& prog,int cur);//ʶ����
int Operator(string& prog, int cur);//ʶ�������
void perror(string& prog);//��������

Token* TokenInstance;//��ŵ�ǰ��ȡ��Token
int ReadPtr;//��ǰ��ȡλ��



/*
�������һ�У�
    token->������|��ʶ��|����|���|�����
�������ַ��ж�token�����͡����б����ֺͱ�ʶ���ṹ���ƣ�����һ�ࡣ

�������ĸ�����»��߿�ͷ����Ϊ�ǵ�һ��
  ���䣺�ڲ��Ժ��ֻ���%��ͷ�������ʶ���������������࣬��Ϊ��%��ͷ

��������֣�������Ϊ�ǵڶ��ࣨ������

����ǽ����������Ϊ�ǵ����ࡣ
  ���䣺ע�⵽ע������/��ͷ���������/�����ַ��ظ�������ע�͹��ɵ����������ȥ�����
        ����Ҳ���Խ����н���趨Ϊ���ַ����Ӷ���Ϊһ���ַ���

������������������Ϊ�ǵ����ࡣ

������϶����ǣ��׳����󣬷��ش����룬���������ַ���
*/
int CheckFirst(char first)
{
   if ((first>= 'a' && first<= 'z') || (first>= 'A' && first<= 'Z') || first== '_')//����Ƿ��Ǳ�ʶ�����Ǳ�����
     return 1;//��һ��

   if(first>= '0' && first<= '9')//����Ƿ��ǳ���
    return 2;//�ڶ���

    //�������
    string Delimiters="(){}[]'\",.:;";//������ܵ�first

    //����Ƿ�Ϊ���
    for(int i=0;i<12;i++)
    {
        if(first==Delimiters[i])
            return 3;
    }
    //������������ܵĵ�һλ
    string Operators="+-*/<>=!&^~|%?";

    //�Ƿ�Ϊ�����
    for(int i=0;i<14;i++)
    {
        if(first==Operators[i])
            return 4;
    }
    return 5;
}

//ʶ��ñ�ʶ��������token�Ƿ�Ϸ�����д��tokenʵ��
int IdentifierOrReserved(string& prog,int cur)
{
     string TokenString;
     //����tokenȫ������TokenString
     while((prog[ReadPtr]>= 'a' && prog[ReadPtr]<= 'z') || (prog[ReadPtr]>= 'A' && prog[ReadPtr]<= 'Z') || prog[ReadPtr]== '_')
     {
         TokenString+=prog[ReadPtr++];
     }
     //һһ�����Ƿ�Ϊ������
     string KeyWord[32] = {"auto", "break", "case", "char", "const", "continue","default", "do", "double", "else", "enum", "extern",
      "float", "for", "goto", "if", "int", "long","register", "return", "short", "signed", "sizeof", "static",
      "struct", "switch", "typedef", "union", "unsigned", "void","volatile", "while"
     };


     int flag=1;//��ʶ�Ƿ�Ϊ������
     for(int j=0;j<32&&flag;j++)
     {

         if(TokenString == KeyWord[j])
            {
                flag=0;
                TokenInstance->lab=j+1;
            }

     }
     //����Ǳ�����
     if(!flag)
     {
        TokenInstance->number = cur;
        TokenInstance->type = TokenString;
     }
     //����Ϊ��ʶ��
     else
    {
        TokenInstance->number=cur;
        TokenInstance->type=TokenString;
        TokenInstance->lab=81;
     }
     return 1;
}

//ʶ��ó���token�Ƿ�Ϸ�����д��tokenʵ��
int Number(string& prog,int cur)
{
    string TokenString;
    int flag=1;//��ʶflag�Ƿ����
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

//ʶ��ý��token�Ƿ�Ϸ�����д��tokenʵ��
int Delimiter(string& prog,int cur)
{
     string TokenString;
     //�ж�����һ��������Ӷ�������ȷ�ı��
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

//���ձ�Ŵ�С�����˳��ȥʶ������һ�ֲ���������ע�͡�ռλ��
int Operator(string& prog,int cur)
{
   string TokenString;
     //-��ͷ��-��33����--��34����-=��35����->(36)
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

   //!��ͷ��!��37����!=��38��
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

   //%��ͷ��%��39����%=��40����������ݲ���֪������%��ͷ��ռλ����ռλ��������ʶ����
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

   //&��ͷ��&��41����&&��42����&=��43��
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
   //*��ͷ��*��46����*=��47��
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

  // /��ͷ��/��50����/=(51),//(79),/*(79)
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
            //�����//ע�ͣ�ֱ����һ��Ϊֹ��Ϊע��
          if(prog[ReadPtr]=='/')
          {
              while(prog[ReadPtr]!='\n')
                TokenString+=prog[ReadPtr++];
          }
          //�����/*ע�ͣ�ֱ��*/��Ϊע��*/
          else
          {
              while(prog[ReadPtr]!='*'||prog[ReadPtr+1]!='/')
              TokenString+=prog[ReadPtr++];
              TokenString+="*/";
              ReadPtr+=2;
          }
      }
      else//����Ϊ��ͨ��/
      {
        TokenString="/";
        TokenInstance->lab=50;
      }
  }
  //?��ͷ��?��54��
  else if(prog[ReadPtr]=='?')
  {
      TokenString="?";
      TokenInstance->lab=54;
      ReadPtr++;
  }
  //^��ͷ^(57),^=(58)
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
  //|��ͷ��|��60����||��61����|=��62��
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
   //~��ͷ��~��64��
   else if(prog[ReadPtr]=='~')
   {
        TokenString="~";
      TokenInstance->lab=64;
      ReadPtr++;
   }
   //+��ͷ��+��65����++��66����+=��67��
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

   //<��ͷ��<(68),<<(69),<<=(70),<=(71)

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
   //=��ͷ��=��72����==��73��
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
   //>��ͷ��>(74),>=(75),>>(76),>>=(77)
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

//���ִ���ʱ������ʶ����ַ�
//��ʵ�ϸ�ʵ����û�д���
void perror(string& prog)
{
      ReadPtr++;
}

void Analysis()
{
	string prog;//prog���������ȡ���ַ�
	read_prog(prog);//�ú������ļ��е��ַ�ȫ����ȡ��prog��
	/* ɧ���� �뿪ʼ���ǵı��� */
    /********* Begin *********/
    int cur=1;
     int cnt=1;//��¼�ж��ٸ�token
     TokenInstance = new Token;
     ReadPtr=0;
    while(ReadPtr<prog.length())
    {

        //�������ַ�
        while(ReadPtr<prog.length()&&(prog[ReadPtr]==' '||prog[ReadPtr]=='\n'||prog[ReadPtr]=='\t'))
            ReadPtr++;
      /*
        ��������Ҫʶ����ַ�����ʼ���,��������һһ����token��ÿһ���ַ�
        ����ʵ����,ʶ���˵�һ���ַ��������жϸ�token�����
        ��������CheckFirst�б�token���Ȼ����ö�Ӧ�����ж��Ƿ�Ϸ���
      */

      //���Token
      if(cur!=cnt)
      cout << TokenInstance->number << ": <" << TokenInstance->type << "," << TokenInstance->lab << ">" << endl;

     cnt=cur;
     switch(CheckFirst(prog[ReadPtr]))//�ȷ���
      {
          //�����ֻ��ʶ��
         case 1:
         if(IdentifierOrReserved(prog,cur))
            cur++;//ʶ��ɹ�
         else
            perror(prog);//������
         break;

         //����
         case 2:
         if(Number(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //���
         case 3:
         if(Delimiter(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //�����
         case 4:
         if(Operator(prog,cur))
            cur++;
         else
            perror(prog);
         break;

         //����
         default:
         perror(prog);
         break;
      }
    }
    if(cur!=1)//���һ����������س�
     cout << TokenInstance->number << ": <" << TokenInstance->type << "," << TokenInstance->lab << ">";
     free(TokenInstance);
    /********* End *********/

}
