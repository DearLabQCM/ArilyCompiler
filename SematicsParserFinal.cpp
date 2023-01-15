// C语言LL1分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <stack>
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
vector<string> StringToTokens(string str)//字符串转Token数组函数
{
    vector<string> ret;//以空格为分隔符，解析字符串为Token数组
    string temp="";
    for(int j=0;j<str.length();j++)
    {
        if(str[j]==' ')
        {
            ret.push_back(temp);
            temp="";
        }
        else
        {
            temp+=str[j];
            if(j==str.length()-1)ret.push_back(temp);
        }
    }
    return ret;
}

string TokensToString(vector<string>Tokens)//Token流转字符串
{
    if(Tokens.size()==0)return "";
    string ret;
    for(int i=0;i<Tokens.size()-1;i++)
        ret+=Tokens[i]+" ";
    ret+=Tokens[Tokens.size()-1];
    return ret;

}

string SymbolsToString(set<string>symbols)//符号集合转字符串，用于对比first/follow集合
{
    if(symbols.size()==0)return "";
    string ret;
    set<string>::iterator it;
    for(it=symbols.begin();it!=symbols.end();it++)
    {
        it++;
        if(it==symbols.end())break;
        it--;
        ret+=*it;
        ret+=" ";
    }
    it--;
    ret+=*it;
    return ret;
}


void PrintPro(vector<string> pro)//打出产生式
{
    for(int i=0;i<pro.size();i++)
    {
        cout<<pro[i]<<" ";
    }
    cout<<endl;
}

int StringToInt(string str)
{
    int ret=0;
    for(int i=0;i<str.length();i++)
    {
        char NowChar=str[i];
        NowChar-=48;
        ret*=10;
        ret+=NowChar;
    }
    return ret;
}

string IntToString(int it)
{
    string ret="";
    while(it)
    {
        char c=it%10;
        c+=48;
        ret+=c;
        it/=10;
    }
    reverse(ret.begin(),ret.end());
    return ret;
}

double StringToDouble(string str)
{
    double ret1=0;
    double ret2=0;
    bool DotFlag=0;
    int cnt=1;
    for(int i=0;i<str.length();i++)
    {
        if(str[i]=='.')
        {
            DotFlag=1;
            continue;
        }
        if(DotFlag)
        {
            char NowChar=str[i];
            NowChar-=48;
            ret1*=10;
            ret1+=NowChar;
            cnt*=10;
        }
        else
        {
            char NowChar=str[i];
            NowChar-=48;
            ret2*=10;
            ret2+=NowChar;
        }

    }
    return ret1/cnt+ret2;
}

string DoubleToString(double d)
{
    int it=(int)d;
    double rest=d-it;
    string IntString=IntToString(it);
    string DoubleString="";
    while(rest)
    {
        rest*=10;
        int RestInt=(int)rest;
        rest-=RestInt;
        DoubleString+=(RestInt)+48;
    }
    return IntString+"."+DoubleString;

}
/*
    在这里的文法定义中，有如下概念：
        Token流：为vector<string>类型。为一个字符串按空格分割而成的tokens，可以利用StringToTokens函数生成。
        非终结符：一个string。
        终结符：一个string。
        产生式：一个Token流，如产生式A->if while为["if"，"while"]
        空型字符串："E"，代表空产生式。
        空串：""，用于错误处理。

        非终结符集合（NonTerminal）：一个vector<string>，包含一个CFG所有的非终结符。
        终结符集合（terminal）：一个vector<string>，包含一个CFG所有的终结符。
        产生式集合（productions）：一个map。其键为非终结符，值为键对应的所有产生式的集合，为vector<vector<string>>。
        起始非终结符（StartSym）：第一条产生式的左端。
        解析表（table）：一个双层map，以两层代指解析表的行、列。行键为非终结符，列键为终结符，值为对应的产生式（vector<string>）。在LL1文法中，每个行、列键对只能有一个值。
        单产生式判定映射（SinglePro）：一个map。用于判定一个非终结符是否只有一个产生式，可用于错误处理。键为非终结符，值为对应的单一产生式。如果非终结符不只有一个产生式，值为空串。
        空串判定映射（Nullable）：一个map。判定一个终结符可否转为空串。



        在语义分析中，添加了如下数据结构：
        保留字集合（keywords）：一个vector<string>，包含一个CFG所有的保留字。
        算符集合（operators）：一个vector<string>，包含一个CFG所有的算符。
        输入和原型输入（input/RealInput）：vector<string>，为输入代码解析而成的Token流，其中RealInput为原始输入Token流，input则对标识符处理为ID，数字处理为INTNUM/REALNUM。
        Token定位器（TokenLine）：vector<int>，定位下标对应的Token在代码中对应的行。
        错误标识（isError）：判定语义分析中是否出现错误。
        错误消息（ErrorOutput）：错误时的输出。
*/
class LLCFG//文法对象
{
public:
    //属性
    class Attributes
    {
        private:
        string type;//存储类型
        double value;//数值
        public:
        Attributes(){};
        Attributes(string _type,double val)
        {
            type=_type;
            value=val;
        }

        void SetAttributes(string _type,double val)
        {
            type=_type;
            value=val;
        }

        string GetType()
        {
            return type;
        }

        double GetVal()
        {
            return value;
        }

        void SetVal(double val)
        {
            value=val;
        }

        void SetType(string _type)
        {
            type=_type;
        }
    };


    vector<string>NonTerminal;//非终结符序列
    vector<string>terminal;//终结符序列
    string StartSym;//开始符号
    map<string,vector<vector<string> > >productions;//产生式,每个产生式对应一串非终止符->Token流（即vector<string>）。注意一个非终止符可能有多条产生式，所以是vector<vector<string>>
    map<string,map<string,vector<string> > >table;//解析表
    map<string,vector<string> >SinglePro;//单解析式表，错误处理用
    map<string,bool>Nullable;//判定一个非终结符是否可以变为空串

    map<string,set<string> >first;//First集
    map<string,set<string> >follow;//Follow集
    map<string,bool>Visit;//搜索标记，用于深度优先搜索。为1表示已经搜索过了，避免重复搜索

    map<string,Attributes>SymTab;//符号表，存储已定义标识符和属性
    vector<string>keywords;//存储保留字
    vector<string>operators;//存储操作符
    vector<string>input;//用于存放处理过的输入（将标识符处理为ID，数字处理为INTNUM/REALNUM）切割成的Token流
    vector<int>TokenLine;//Token对应的行
    vector<string>RealInput;//用于存放真实输入切割成的Token流
    map<string,vector<vector<string> > >EmbeddedProductions;
    vector<string>ActionStack;

    bool isError;
    vector<string>ErrorOutput;




    //词法分析
    bool isAlpha(string token)
    {
        if((token[0]>='A'&&token[0]<='Z')||(token[0]>='a'&&token[0]<='z'))
        {
            return true;
        }
        return false;
    }

    bool isKeyWords(string token)
    {
        if(find(keywords.begin(),keywords.end(),token)==keywords.end())
        {
            return false;
        }
        return true;
    }

    bool isOpr(string opr)
    {
        if(find(operators.begin(),operators.end(),opr)==operators.end()){
            return false;
        }
        return true;
    }

    bool isDigit(string token)
    {
        if((token[0]>='0')&&(token[0]<='9'))
            return true;
        return false;
    }

    //初始化保留字和运算符
    void SaveWords(vector<string>_keywords,vector<string>_operators)
    {
        //插入需处理的关键词
        keywords=_keywords;
        //插入操作符
        operators=_operators;
    }


    //词法分析

    //解析输入为Token流
    void LexicalAnalysis(string prog)
    {
        int line=1;
        //将输入解析为Token流
            for(int i=0;i<prog.length();i++)
            {
                //跳过空字符，记录当前行
                if(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')
                {
                    if(prog[i]=='\n')
                        line++;
                    continue;
                }

                //解析字符流为token
                string token="";
                while(!(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')&&i<prog.length())
                    token+=prog[i++];
                //token当前行
                TokenLine.push_back(line);

                //input保留处理后的输入，RealInput保留真实输入
                if(isAlpha(token)&&!isKeyWords(token))
                    input.push_back("ID");
                else if(isDigit(token))
                    if(token.find(".")!=string::npos)
                        input.push_back("REALNUM");
                    else
                        input.push_back("INTNUM");
                else
                    input.push_back(token);
                RealInput.push_back(token);
                {
                    if(prog[i]=='\n')
                        line++;
                    continue;
                }
            }
            input.push_back("$");
            RealInput.push_back("$");
    }

    //初始化符号表
    void init()
    {
        //插入属性表
        for(int i=0;i<RealInput.size();i++)
        {
            //token是标识符
            if(isAlpha(RealInput[i])&&!(isKeyWords(RealInput[i])))
            {
                Attributes TempAttr;
                TempAttr.SetAttributes("void",0);
                SymTab[RealInput[i]]=TempAttr;
            }
        }
    }


    //两个小函数，用于判定某个串是否是合法终结符/非终结符
    bool isNon(string str)
    {
        for(int i=0;i<NonTerminal.size();i++)
            if(NonTerminal[i]==str)return 1;
        return 0;
    }

    bool isTer(string str)
    {
        for(int i=0;i<terminal.size();i++)
            if(terminal[i]==str)return 1;
        return 0;
    }

    /*
        初始化需要输入：
            1、非终结符序列
            2、终结符序列
            3、输入的单条产生式采用string，在初始化函数终解析为Token流，即vector<string>，所以参数_productions值为vector<string>
               字符串对字符串映射的产生式序列
               函数会将产生式序列中的值变为Token数组，键不变
    */
    LLCFG(vector<string>_NonTerminal,vector<string>_terminal,map<string,vector<string> > _productions)
    {
        NonTerminal=_NonTerminal;
        terminal=_terminal;
        StartSym=_NonTerminal[0];
        for(map<string,vector<string> >::iterator it=_productions.begin();it!=_productions.end();it++)
        {
            for(int i=0;i<_productions[it->first].size();i++)
                productions[it->first].push_back(StringToTokens(_productions[it->first][i]));
            if(productions[it->first].size()==1)SinglePro[it->first]=productions[it->first][0];
        }
        isError=0;
    }

    void Embed(string Embedder)
    {
        terminal.push_back(Embedder);
    }


    //语法分析
    /*
        创建First集，并根据First补充解析表
    */

    void dfs(string NowNonTerminal)
    {
            if(Visit[NowNonTerminal])return;//若已经搜索过，返回
            Visit[NowNonTerminal]=1;//标记

            for(int i=0;i<productions[NowNonTerminal].size();i++)//对该非终结符对应的每个产生式进行搜索
            {
                vector<string>NowPro=productions[NowNonTerminal][i];//取出该产生式

                if(NowPro[0]=="E")
                {
                    Nullable[NowNonTerminal]=1;
                    continue;
                }


                int ProStart=0;
                if(NowPro[0][0]=='M')ProStart=1;
                if(isTer(NowPro[ProStart]))//若该产生式以终结符起头，直接录入First集和解析表
                {
                    if(table[NowNonTerminal][NowPro[ProStart]].size())//如果表中已经存在该表项，直接覆盖，并报告
                        cout<<"文法"<<" "<<NowNonTerminal<<" "<<NowPro[ProStart]<<"存在冲突"<<endl;
                    table[NowNonTerminal][NowPro[ProStart]]=NowPro;//录入解析表
                    first[NowNonTerminal].insert(NowPro[ProStart]);//录入First集合
                }
                else
                {
                    for(int j=ProStart;j<=NowPro.size();j++)
                    {
                        if(NowPro[j][0]=='M')continue;
                        if(j==NowPro.size())//迭代到了末尾，说明整个产生式可以为空，赋值Nullable
                        {
                            Nullable[NowNonTerminal]=1;
                            break;
                        }

                        if(NowPro[j]=="E")
                        {
                            Nullable[NowNonTerminal]=1;
                            continue;
                        }
                        if(isTer(NowPro[j]))//遇到终结符，赋值并退出
                        {
                            if(table[NowNonTerminal][NowPro[j]].size())//如果表中已经存在该表项，直接覆盖，并报告
                            cout<<"文法"<<" "<<NowNonTerminal<<" "<<NowPro[j]<<"存在冲突"<<endl;
                            table[NowNonTerminal][NowPro[j]]=NowPro;//录入解析表
                            first[NowNonTerminal].insert(NowPro[j]);//录入First集合
                            break;
                        }

                        dfs(NowPro[j]);//遇到非终结符，递归地求解其First集合
                        for(set<string>::iterator it=first[NowPro[j]].begin();it!=first[NowPro[j]].end();it++)//对该非终结符的First集合中的每个元素，录入现在i迭代的非终结符的table行和First集合
                        {
                            if(table[NowNonTerminal][*it].size())
                                {
                                    cout<<NowPro[j]<<endl;
                                    cout<<"文法"<<" "<<NowNonTerminal<<" "<<*it<<"存在冲突"<<endl;
                                }
                            table[NowNonTerminal][*it]=NowPro;
                            first[NowNonTerminal].insert(*it);

                        }
                        if(Nullable[NowPro[j]]!=1)
                                break;//如果该非终结符也可以为空，继续往下递归。否则中断循环。
                    }
                }
            }
    }
    void CreateByFirst()
    {
        //从首个非终结符开始，进行深度优先搜索
        for(int i=0;i<NonTerminal.size();i++)
            dfs(NonTerminal[i]);
    }

    /*
        创建Follow集，并根据First补充解析表
    */
    void CreateByFollow()
    {
        //在开始符号的follow集加上$
        follow[StartSym].insert("$");

        //这里求follow的算法为：
        //对每个非终结符，遍历其在右边的产生式，将可能在其后的终结符、非终结符的first置入其follow
        //有多少个NonTerminal，将这个过程重复多少次
        int NTCnt=NonTerminal.size();
        while(NTCnt--)
        {
            //遍历所有非终结符
            for(int i=0;i<NonTerminal.size();i++)
            {
                string NowNonTerminal=NonTerminal[i];
                //遍历所有产生式，寻找右边包含当前非终结符的产生式
                //这里直接用iterator迭代productions会有异常，改成根据非终结符去访问productions进行迭代
                for(int j=0;j<NonTerminal.size();j++)
                {
                    string NTKey=NonTerminal[j];
                    for(int k=0;k<productions[NTKey].size();k++)
                    {
                        vector<string>NowPro=productions[NTKey][k];//当前产生式

                        //若当前产生不包含当前迭代的非终结符，continue
                        vector<string>::iterator FindNowNT=find(NowPro.begin(),NowPro.end(),NowNonTerminal);
                        if(FindNowNT==NowPro.end())
                        {
                            continue;
                        }

                        //否则当前产生式为NTKey->…NowNonTerminal…的形式
                        //从NowNonTerminal的下一个Token开始，为NowNonTerminal获取follow集合
                        FindNowNT++;
                        for(;FindNowNT!=NowPro.end();FindNowNT++)
                        {
                            if(isTer(*FindNowNT))
                            {
                                if((*FindNowNT)[0]=='M')continue;
                                follow[NowNonTerminal].insert(*FindNowNT);
                                break;
                            }
                            //PrintPro(NowPro);
                            //cout<<NowNonTerminal<<":"<<endl;
                            for(set<string>::iterator NextNTFirst=first[*FindNowNT].begin();NextNTFirst!=first[*FindNowNT].end();NextNTFirst++)
                            {
                                follow[NowNonTerminal].insert(*NextNTFirst);
                                //cout<<NowNonTerminal<<" "<<*FindNowNT<<" "<<*NextNTFirst<<endl;
                            }
                            //下一个非终结符可以为空，继续往下迭代
                            if(!Nullable[*FindNowNT])
                                break;
                        }
                        //如果NowNonTerminal可以作为NTKey产生式的尾部，将NTKey的follow置入NowNonTerminal的follow
                        if(FindNowNT==NowPro.end())
                        {
                            for(set<string>::iterator NTKeyFollow=follow[NTKey].begin();NTKeyFollow!=follow[NTKey].end();NTKeyFollow++)
                            {
                                follow[NowNonTerminal].insert(*NTKeyFollow);
                            }
                        }
                    }
                }
            }
        }

        //对所有可能为空的NonTerminal，在解析表的对应follow列置入“E”
        for(int i=0;i<NonTerminal.size();i++)
        {
            string NowNonTerminal=NonTerminal[i];
            if(Nullable[NowNonTerminal]==1)
            {
                for(set<string>::iterator it=follow[NowNonTerminal].begin();it!=follow[NowNonTerminal].end();it++)
                {
                    table[NowNonTerminal][*it]={"E"};
                }
            }
        }
    }




    //错误处理1 类型错误
    void TypeError(int ErrorLine)
    {
          isError=true;
          string ErrorMsg="error message:line " + IntToString(ErrorLine) + ",realnum can not be translated into int type";
          ErrorOutput.push_back(ErrorMsg);
    }
    //错误处理2 被0除
    void DividedByZeroError(int ErrorLine)
    {
        isError=true;
        string ErrorMsg="error message:line "+ IntToString(ErrorLine) + ",division by zero";
        ErrorOutput.push_back(ErrorMsg);

    }

    //比较运算符优先级,前者大于后者为true，否则为false
    bool ComparePriority(string opr1,string opr2)
    {
        if(((opr1=="+")||(opr1=="-"))&&((opr2=="*")||(opr2=="/")))
        {
            return false;
        }
        return true;
    }


    void Assign(int Begin,int End)
    {
            //记录类型 pos
			string type=RealInput[Begin];
			//pos+1 标识符
			string id=RealInput[Begin+1];
			//pos+2 赋值符号
			string opr=RealInput[Begin+2];
			//pos+3 数值
			string val=RealInput[Begin+3];

			if(opr=="=")
            {
                //赋值运算
                Attributes attr=SymTab[id];
                //设置i的属性和值
                attr.SetType(type);
                attr.SetVal(StringToDouble(val));
                SymTab[id]=attr;
                //类型错误
                if(type=="int"&&val.find(".")!=string::npos)
                    TypeError(TokenLine[Begin]);
                //pos+4 分号
                string semi=RealInput[Begin+4];
            }
    }

    //布尔表达式的计算
    bool ComputeBoolExpr(int Begin,int End)
    {
        //默认采取简单的中缀表达式 即 a opr b
        string left=RealInput[Begin];//a
        string right=RealInput[End];//b
        Attributes AttrLeft=SymTab[left];
        Attributes AttrRight=SymTab[right];
        double ValueLeft=AttrLeft.GetVal();
        double ValueRight=AttrRight.GetVal();
        string opr=RealInput[Begin+1];
        if(opr==">=") return ValueLeft>=ValueRight;
        if(opr=="<=") return ValueLeft<=ValueRight;
        if(opr==">") return ValueLeft>ValueRight;
        if(opr=="<") return ValueLeft<ValueRight;
        if(opr=="==") return ValueLeft==ValueRight;
        return false;
    }

    //算术表达式的计算
    void ComputeArithExpr(int Begin,int End)
    {
        //存储后缀表达式
        vector<string> PostfixExpr;
        //存储运算符号
        stack<string> OpStack;
        //Begin+1处为=
        string id=RealInput[Begin];
        //存储位置
        stack<int>OpLine;
        vector<int>ExprLine;

        //运算语句从Begin+2开始
        for(int j=Begin+2;j<End;j++)
        {
            string NowToken=RealInput[j];
            //如果是操作符
            if(isOpr(NowToken))
            {
                if(NowToken=="(")//左括号
                {
                    OpStack.push(RealInput[j]);
                    OpLine.push(TokenLine[j]);
                }
                else
                {
                    //遇到右括号，把括号之间的运算符全部pop，加入后缀表达式
                     if(NowToken==")")
                     {
                          while(OpStack.top()!="(")
                          {
                                PostfixExpr.push_back(OpStack.top());
                                ExprLine.push_back(OpLine.top());
                                OpStack.pop();
                                OpLine.pop();
                          }
                          //pop左括号
                          OpStack.pop();
                          OpLine.pop();
                     }
                     else
                     {
                         //如果遇到其他运算符
                         /*
                             比较栈顶和当前运算符优先级，
                             如果栈顶更高则入栈 否则pop直至栈顶优先级小于当前优先级
                         */
                         while(!OpStack.empty()&&ComparePriority(OpStack.top(),NowToken))
                         {
                            PostfixExpr.push_back(OpStack.top());
                            ExprLine.push_back(OpLine.top());
                            OpStack.pop();
                            OpLine.pop();
                         }
                         OpStack.push(RealInput[j]);
                         OpLine.push(TokenLine[j]);
                     }
                }
            }
            else
            {
                //操作数入栈
                PostfixExpr.push_back(RealInput[j]);
                ExprLine.push_back(TokenLine[j]);
            }
        }

        //处理完中缀表达式，如果符号栈仍不为空，则全部加入后缀表达式
        while(!OpStack.empty())
        {
            PostfixExpr.push_back(OpStack.top());
            ExprLine.push_back(OpLine.top());
            OpStack.pop();
            OpLine.pop();
        }

        //后缀表达式计算
        //存储运算数
        stack<double> NumStack;
        for(int i=0;i<PostfixExpr.size();i++)
        {
            string s=PostfixExpr[i];
            double res=0;
            if(isOpr(s))
            {
                //如果遇到运算符从NumStack pop两个元素进行计算
                double AriLeft=NumStack.top();
                NumStack.pop();
                double AriRight=NumStack.top();
                NumStack.pop();
                if(s=="+")
                    res=AriRight+AriLeft;
                else
                    if(s=="-")
                        res=AriRight-AriLeft;
                    else
                        if(s=="*")
                            res=AriRight*AriLeft;
                        else
                            {
                                if(AriLeft== 0)
                                    DividedByZeroError(ExprLine[i]);
                                res=AriRight/AriLeft;
                            }
                NumStack.push(res);
            }
            else
            {
                //如果遇到操作数直接入栈,操作数可能不在属性表中,需要单独进行判断
                if(SymTab.find(s)==SymTab.end())
                    NumStack.push(StringToDouble(s));
                else
                    NumStack.push(SymTab[s].GetVal());
            }
        }
        //计算结果
        SymTab[id].SetVal(NumStack.top());
    }
    void ExecIf(int Begin,int End)
    {
                    int ThenIndex=Begin+1;//定位then
                    while(ThenIndex<RealInput.size()&&RealInput[ThenIndex]!="then")
                        ThenIndex++;

                    int ElseIndex=ThenIndex+1;//定位else
                    while(ElseIndex<RealInput.size()&&RealInput[ElseIndex]!="else")
                        ElseIndex++;
                    int SemiIndex=ElseIndex+1;//定位分号
                    while(SemiIndex<RealInput.size()&&RealInput[SemiIndex]!=";")
                        SemiIndex++;

                    //计算布尔表达式结果
                    bool res=ComputeBoolExpr(Begin+2,ThenIndex-2);
                    if(res)//条件为真 执行then
                        ComputeArithExpr(ThenIndex+1,ElseIndex-1);
                    else//条件为假 执行else
                        ComputeArithExpr(ElseIndex+1,SemiIndex);
    }


    void parsing(string prog)
    {
        LexicalAnalysis(prog);

        //解析过程

        //栈构造
        //这里采用vector数组模拟栈
        vector<string>Stack;//数组模拟栈
        int top=0;//栈顶
        Stack.push_back("$");//压入$
        Stack.push_back(StartSym);//压入开始符号
        top++;

        //输出构造
        vector<string>output;//输出数组

        vector<string>OutLeft;
        vector<vector<string> >OutRight;




        //解析过程
        for(int i=0;i<input.size();i++)//input识别从左往右，Stack是从右往左对应
        {
            while(Stack[top][0]=='M')
            {
                ActionStack.push_back(Stack[top]);
                int ActionCode=Stack[top][1]-'0';
                if(ActionCode%2)ActionStack.push_back(IntToString(i-1));
                else ActionStack.push_back(IntToString(i));
                top--;

            }
            //需要对Stack进行变化操作
            while(Stack[top]!=input[i])
            {
                string StackTopIndex=Stack[top];//Stack栈顶token
                string InputLeftIndex=input[i];//Input当前最左token

                    vector<string> production=table[StackTopIndex][InputLeftIndex];//查找产生式
                    //若为空产生式，弹出栈顶，输出空产生式
                    //注意，空产生式是“E”，而不是""。
                    if(production.size()&&(production[0]=="E"))
                        {
                            OutLeft.push_back(Stack[top]);
                            OutRight.push_back({"E"});
                            top--;
                        }
                    //否则
                    else
                    {
                        //production为空，匹配出错，错误处理
                        if(production.size()==0)
                        {
                            int ErrorCode=perror(Stack[top]);
                            //错误类型-1 -2下，栈顶为单一产生式非终结符，直接应用产生式
                            if(ErrorCode==-1)
                            {
                                output.push_back(Stack[top]);
                                output.push_back("E");
                                top--;
                                continue;
                            }
                            if(ErrorCode==-2)
                            {
                                production=SinglePro[Stack[top]];
                            }

                            //其余错误类型下，调用通常错误处理
                            //找到栈中最上面的终结符，将其置于input的最左端
                            else
                            {
                                int ErrorPtr;
                                for(ErrorPtr=top;ErrorPtr>=0;ErrorPtr--)
                                {
                                        if(isTer(Stack[ErrorPtr])&&Stack[ErrorPtr][0]!='M')
                                           break;

                                }
                                //由于我们在词法解析中就已经找出了REALNUM和INTNUM，这里的语法解析事实上已经找出了赋值中的类型错误并完成了改正（修改INTNUM或REALNUM本身，不做回退）
                                //但这里我们选择不输出，使得该错误的输出与其余的语义动作同步
                                //此外，这里的错误处理稍有不同。因为这里的input后面语义分析需要使用，所以不能更改原有内容
                                if(input[i].find("NUM")!=string::npos &&Stack[ErrorPtr].find("NUM")!=string::npos)
                                    input[i]=Stack[ErrorPtr];
                                else
                                    input.insert(input.begin()+i,Stack[ErrorPtr]);
                                continue;
                            }

                        }

                        top--;
                        //入栈
                        for(int j=production.size()-1;j>=0;j--)//产生式倒序入栈
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                            Stack[top]=production[j];
                        }
                    }
                    while(Stack[top][0]=='M')
                    {
                        ActionStack.push_back(Stack[top]);
                        int ActionCode=Stack[top][1]-'0';
                        if(ActionCode%2)ActionStack.push_back(IntToString(i-1));
                        else ActionStack.push_back(IntToString(i));
                        top--;
                    }
            }
            top--;//弹出

            //继续读入input
        }

        for(int i=0;i<ActionStack.size();i++)
        {
            if((ActionStack[i]=="M0")||(ActionStack[i]=="M2"))
            {
                Assign(StringToInt(ActionStack[i+1]),StringToInt(ActionStack[i+3]));
                i+=3;
                continue;
            }
            if(ActionStack[i]=="M4")
            {
                ExecIf(StringToInt(ActionStack[i+1]),StringToInt(ActionStack[i+3]));
                i+=7;
                continue;
            }
            if(ActionStack[i]=="M6")
            {
                ComputeArithExpr(StringToInt(ActionStack[i+1]),StringToInt(ActionStack[i+3]));
                i+=3;
                continue;
            }

        }
        if(isError)//程序错误
            for(int i=0;i<ErrorOutput.size();i++)
            {
                if(i==ErrorOutput.size()-1)
                    cout<<ErrorOutput[i];
                else
                    cout<<ErrorOutput[i]<<endl;
            }
        else
        {
            for(map<string,Attributes>::iterator it=SymTab.begin();it!=SymTab.end();it++)
            {
                string id=it->first;
                cout<<id<<": ";
                if((it->second).GetType()=="int")
                    cout<<(int)SymTab[id].GetVal()<<endl;
                else
                    cout<<SymTab[id].GetVal()<<endl;
            }
        }

    }

    //错误处理
    int perror(string StackTop)
    {
        //错误类型0：栈顶为终结符，但与input不匹配
        if(isTer(StackTop))
            return 0;
        if(SinglePro[StackTop].size())
        {
            //错误类型-1：栈顶为单一产生式非终结符，且产生式为空
            if(productions[StackTop][0][0]=="E")
                return -1;
            //错误类型-2：栈顶为单一产生式非终结符，且产生式不为空
            return -2;
        }
        //其它错误
        return -3;
    }

    void PrintTable()
    {
        for(int i=0;i<NonTerminal.size();i++)
        {
            for(int j=0;j<terminal.size();j++)
            {
                cout<<NonTerminal[i]<<" ";
                cout<<terminal[j]<<" ";
                PrintPro(table[NonTerminal[i]][terminal[j]]);
            }
        }
    }
};

bool operator ==(vector<string> &x,vector<string> &y)
{
    auto it1=x.begin();
    auto it2=y.begin();

    for(; it1!=x.end(),it2!=y.end(); it1++,it2++)
    {
        string a=*it1;
        string b=*it2;
        if(a==b)
            continue;

        //有一个token不相等，两Token流一定不相等
        else
            return false;
    }
    return true;
}

//语义分析
void Analysis()
{
    string prog;
    read_prog(prog);
    /* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    //所有M起头的token是语义操作，会在parsing的时候被忽略
    vector<string> NonTerminal={"program","stmt","decl","decls","compoundstmt","stmts","ifstmt","assgstmt","boolexpr","boolop","arithexpr","arithexprprime",
     "multexpr","multexprprime","simpleexpr"};
    vector<string> terminal={"{","}","if","(",")","then","else","ID","=",">","<",">=","<=","==","+","-","*","/","INTNUM","REALNUM","int","real","E",";"};
    vector<string> keywords={"if","int","real","then","else"};
    vector<string> AriOperators={"+","-","*","/","(",")"};
    map<string,vector<string> >ExPro;
    ExPro["program"].push_back("decls compoundstmt");
    ExPro["decls"].push_back("decl ; decls");
    ExPro["decls"].push_back("E");
    ExPro["decl"].push_back("M0 int ID = INTNUM M1");
    ExPro["decl"].push_back("M2 real ID = REALNUM M3");
    ExPro["stmt"].push_back("ifstmt");
    ExPro["stmt"].push_back("assgstmt");
    ExPro["stmt"].push_back("compoundstmt");
    ExPro["compoundstmt"].push_back("{ stmts }");
    ExPro["stmts"].push_back("stmt stmts");
    ExPro["stmts"].push_back("E");
    ExPro["ifstmt"].push_back("M4 if ( boolexpr ) then stmt else stmt M5");
    ExPro["assgstmt"].push_back("M6 ID = arithexpr ; M7");
    ExPro["boolexpr"].push_back("arithexpr boolop arithexpr");
    ExPro["boolop"].push_back(">");
    ExPro["boolop"].push_back(">=");
    ExPro["boolop"].push_back("<");
    ExPro["boolop"].push_back("<=");
    ExPro["boolop"].push_back("==");
    ExPro["arithexpr"].push_back("multexpr arithexprprime");
    ExPro["arithexprprime"].push_back("+ multexpr arithexprprime");
    ExPro["arithexprprime"].push_back("- multexpr arithexprprime");
    ExPro["arithexprprime"].push_back("E");
    ExPro["multexpr"].push_back("simpleexpr multexprprime");
    ExPro["multexprprime"].push_back("* simpleexpr multexprprime");
    ExPro["multexprprime"].push_back("/ simpleexpr multexprprime");
    ExPro["multexprprime"].push_back("E");
    ExPro["simpleexpr"].push_back("ID");
    ExPro["simpleexpr"].push_back("INTNUM");
    ExPro["simpleexpr"].push_back("REALNUM");
    ExPro["simpleexpr"].push_back("( arithexpr )");
    LLCFG ExCFG=LLCFG(NonTerminal,terminal,ExPro);

    //语义动作嵌入
    ExCFG.Embed("M0");
    ExCFG.Embed("M1");
    ExCFG.Embed("M2");
    ExCFG.Embed("M3");
    ExCFG.Embed("M4");
    ExCFG.Embed("M5");
    ExCFG.Embed("M6");
    ExCFG.Embed("M7");
    ExCFG.Embed("M8");
    ExCFG.Embed("M9");
    ExCFG.SaveWords(keywords,AriOperators);
    ExCFG.CreateByFirst();
    ExCFG.CreateByFollow();
    //ExCFG.PrintTable();
    ExCFG.parsing(prog);
    /********* End *********/

}








