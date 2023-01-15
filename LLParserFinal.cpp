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
   LL1的处理这里分为两种：
    快速模式：即预先处理模式。程序会根据文法，利用预先处理好的解析表完成解析，拓展性差/效率高
    非快速模式/拓展模式：即自动生成模式。程序会根据文法，利用算法自动生成解析表完成解析，效率差/拓展性强

    两种模式均能通过所有样例和对应的附加测试
*/

//非快速模式（自动生成模式）相关函数与定义
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
*/
class LLCFG//文法对象
{
public:
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

    void CreateTable();

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
    }

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
                if(isTer(NowPro[0]))//若该产生式以终结符起头，直接录入First集和解析表
                {
                    if(table[NowNonTerminal][NowPro[0]].size())//如果表中已经存在该表项，直接覆盖，并报告
                        cout<<"文法"<<" "<<NowNonTerminal<<" "<<NowPro[0]<<"存在冲突"<<endl;
                    table[NowNonTerminal][NowPro[0]]=NowPro;//录入解析表
                    first[NowNonTerminal].insert(NowPro[0]);//录入First集合
                }
                else
                {
                    for(int j=0;j<=NowPro.size();j++)
                    {
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
                                follow[NowNonTerminal].insert(*FindNowNT);
                                break;
                            }
                            for(set<string>::iterator NextNTFirst=first[*FindNowNT].begin();NextNTFirst!=first[*FindNowNT].end();NextNTFirst++)
                            {
                                follow[NowNonTerminal].insert(*NextNTFirst);
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

    void parsing(string prog)
    {
        //解析输入为token
        vector<string>input;//token数组
        vector<int>TokenLine;//哈希表：token下标-token所在行
        int line=1;//当前行

        //预处理
        int StartPtr=0;
        for(int i=0;i<prog.length();i++)
        {
            //跳过空字符，记录当前行
            if(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')
            {
                StartPtr++;
                if(prog[i]=='\n')
                    line++;
                continue;
            }
            else break;
        }
        if(prog[StartPtr]!='{')
        {
            prog="{"+prog;
            cout<<"语法错误,第"<<line<<"行,缺少\""<<'{'<<"\""<<endl;
        }


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
            {
                token+=prog[i++];
            }

            //token当前行
            TokenLine.push_back(line);
            //将token压入input
            input.push_back(token);
        }
        //添加终止符$
        input.push_back("$");

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
        vector<int>tabs;//每个输出token的对应tab数量

        //记录Stack中现有的token输出时的tab数量
        vector<int>StackTab;
        StackTab.push_back(0);
        StackTab.push_back(-1);


        //解析过程
        for(int i=0;i<input.size();i++)//input识别从左往右，Stack是从右往左对应
        {

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
                            output.push_back(Stack[top]);
                            tabs.push_back(StackTab[top]+1);
                            output.push_back("E");
                            tabs.push_back(StackTab[top]+2);
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
                                tabs.push_back(StackTab[top]+1);
                                output.push_back("E");
                                tabs.push_back(StackTab[top]+2);
                                top--;
                                continue;
                            }
                            if(ErrorCode==-2)
                            {

                                production=SinglePro[Stack[top]];
                            }

                            //其余错误类型下，调用通常错误处理
                            //找到栈中最上面的终结符，将其置于input的最左端
                            //这里由于input[i-1]这个位置已经废弃，并且input[0]在预处理后不可能出现错误，所以直接将该终结符置于input[i-1]，并回退
                            else
                            {
                                int ErrorPtr;
                                for(ErrorPtr=top;ErrorPtr>=0;ErrorPtr--)
                                {
                                        if(isTer(Stack[ErrorPtr]))
                                           break;

                                }
                                cout<<"语法错误,第"<<TokenLine[i]<<"行,缺少\""<<Stack[ErrorPtr]<<"\""<<endl;
                                i--;
                                input[i]=Stack[ErrorPtr];
                                continue;
                            }

                        }

                        //应用产生式，弹出非终结符，压入产生式
                        //先移出当前要替换的非终结符
                        int NowTab=StackTab[top];
                        output.push_back(Stack[top]);
                        tabs.push_back(StackTab[top]+1);
                        top--;
                        //入栈
                        for(int j=production.size()-1;j>=0;j--)//产生式倒序入栈
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                            while(top>StackTab.size()-1)StackTab.push_back(0);
                            Stack[top]=production[j];
                            StackTab[top]=NowTab+1;
                        }
                    }
            }
            //栈顶和input左端匹配
            output.push_back(Stack[top]);//输出
            tabs.push_back(StackTab[top]+1);
            top--;//弹出

            //继续读入input
        }


        //输出
        //注意output最后一行是$，不能输出
        for(int i=0;i<output.size()-1;i++)
        {
            for(int k=0;k<tabs[i];k++)
                cout<<"\t";
            if(i!=output.size()-2)
                cout<<output[i]<<endl;
            else
                cout<<output[i];//最后一行输出没有回车
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
};


//快速模式相关定义
//文法定义
    //非终结符
    string NonTerminal[15]={"program","stmt","compoundstmt","stmts","ifstmt","whilestmt","assgstmt","boolexpr","boolop","arithexpr","arithexprprime",
     "multexpr","multexprprime","simpleexpr"};

    //终结符
    //这里<= >= 和 ==作为一个终结符，否则>=和> <=和<产生左公因式
    string terminal[23]={"{","}","if","(",")","then","else","while","ID","=",">","<",">=","<=","==","+","-","*","/","NUM","E",";"};

    //解析表
    /*
    行为非终结符，列为终结符。
    行：program（0）stmt（1）compoundstmt(2) stmts(3) ifstmt(4) whilestmt(5) assgstmt(6) boolexpr(7)
        boolop(8) arithexpr(9) arithexprprime(10) multexpr(11) multexprprime(12) simpleexpr(13)
    列：{（0） }（1） if(2) ((3) )(4) then(5) else(6) while(7) ID(8) =(9) >(10)
        <(11)  >=(12) <=(13) ==(14) +(15) -(16) *(17) /(18) NUM(19) E(20) ;(21)
    */
    string table[15][23];

//解析表创建函数
void GetFirst();
void GetFollow();


void GetFirst()
{
    /*
    program:
            {:program->compoundstmt
    */
    table[0][0]="compoundstmt";

    /*
    stmt:
            {:stmt->compoundstmt
            if:stmt->ifstmt
            while:stmt->whilestmt
            ID:stmt->assgstmt
    */
    table[1][0]="compoundstmt";
    table[1][2]="ifstmt";
    table[1][7]="whilestmt";
    table[1][8]="assgstmt";

    /*
    compoundstmt:
            {:compoundstmt->{stmts}
    */
    table[2][0]="{ stmts }";//{

    /*
    stmts:
            {:stmts->stmt stmts
            if:stmts->stmt stmts
            while:stmts->stmt stmts
            ID:stmts->stmt stmts

            stmts->E
    */
    table[3][0]="stmt stmts";
    table[3][2]="stmt stmts";
    table[3][7]="stmt stmts";
    table[3][8]="stmt stmts";
    table[3][20]="1";//stmts的first集中有E

    /*
    ifstmt:
            if:ifstmt->if (boolexpr) then stmt else stmt
    */
    table[4][2]="if ( boolexpr ) then stmt else stmt";

    /*
    whilestmt:
            while:whilestmt->while (boolexpr) stmt
    */
    table[5][7]="while ( boolexpr ) stmt";

    /*
    assgstmt:
            ID:assgstmt->ID=arithexpr
    */
    table[6][8]="ID = arithexpr ;";

    /*
    boolexpr:
            (:boolexpr->arithexpr boolop arithexpr
            ID:boolexpr->arithexpr boolop arithexpr
            NUM:boolexpr->arithexpr boolop arithexpr
    */
    table[7][3]="arithexpr boolop arithexpr";
    table[7][8]="arithexpr boolop arithexpr";
    table[7][19]="arithexpr boolop arithexpr";

    /*
    boolop:
            >:boolop-> >
            <:boolop-> <
            >=:boolop-> >=
            <=:boolop-> <=
            ==:boolop-> ==
    */
    table[8][10]=">";
    table[8][11]="<";
    table[8][12]=">=";
    table[8][13]="<=";
    table[8][14]="==";

    /*
    arithexpr:
            (:arithexpr->multexpr arithexprprime
            ID:arithexpr->multexpr arithexprprime
            NUM:arithexpr->multexpr arithexprprime
    */
    table[9][3]="multexpr arithexprprime";
    table[9][8]="multexpr arithexprprime";
    table[9][19]="multexpr arithexprprime";

    /*
    arithexprprime:
            +:arithexprprime->+ multexpr arithexprprime
            -:arithexprprime->- multexpr arithexprprime

            arithexprprime->E
    */
    table[10][15]="+ multexpr arithexprprime";
    table[10][16]="- multexpr arithexprprime";
    table[10][20]="1";//arithexprprime first集有E

    /*
    multexpr:
            (:multexpr->simpleexpr arithexprprime
            ID:multexpr->simpleexpr arithexprprime
            NUM:multexpr->simpleexpr arithexprprime
    */
    table[11][3]="simpleexpr multexprprime";
    table[11][8]="simpleexpr multexprprime";
    table[11][19]="simpleexpr multexprprime";

    /*
    multexprprime:
            *:multexprprime->* simpleexpr multexprprime
            /:multexprprime->/ simpleexpr multexprprime

            multexprprime->E
    */
    table[12][17]="* simpleexpr multexprprime";
    table[12][18]="/ simpleexpr multexprprime";
    table[12][20]="1";//multexprprime first集有E

    /*
    simpleexpr:
            ID:simpleexpr->ID
            NUM:simpleexpr->NUM
            (:simpleexpr->(arithexpr)
    */
    table[13][3]="( arithexpr )";
    table[13][8]="ID";
    table[13][19]="NUM";
}
void GetFollow()
{
    //follow{stmts}={ } }
    table[3][1]="E";

    //follow{arithexprprime}={),>,<,>=,<=,==,;}
    table[10][4]="E";
    table[10][10]="E";
    table[10][11]="E";
    table[10][12]="E";
    table[10][13]="E";
    table[10][14]="E";
    table[10][21]="E";

    //follow{multexprprime}={+,-,),>,<,>=,<=,==,;}
    table[12][4]="E";
    table[12][10]="E";
    table[12][11]="E";
    table[12][12]="E";
    table[12][13]="E";
    table[12][14]="E";
    table[12][15]="E";
    table[12][16]="E";
    table[12][21]="E";

}


void Analysis()
{
	string prog;
	read_prog(prog);
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    bool fast=0;//快速模式标识

    if(!fast)//非快速模式/自动生成模式，拓展性强，效率低（因为每次都要生成解析表）
    {
        //非终结符
        vector<string> NonTerminal={"program","stmt","compoundstmt","stmts","ifstmt","whilestmt","assgstmt","boolexpr","boolop","arithexpr","arithexprprime",
         "multexpr","multexprprime","simpleexpr"};
        //终结符
        //这里<= >= 和 ==作为一个终结符，否则>=和> <=和<产生左公因式
        vector<string> terminal={"{","}","if","(",")","then","else","while","ID","=",">","<",">=","<=","==","+","-","*","/","NUM","E",";"};
        map<string,vector<string> >ExPro;
        ExPro["program"].push_back("compoundstmt");
        ExPro["stmt"].push_back("ifstmt");
        ExPro["stmt"].push_back("whilestmt");
        ExPro["stmt"].push_back("assgstmt");
        ExPro["stmt"].push_back("compoundstmt");
        ExPro["compoundstmt"].push_back("{ stmts }");
        ExPro["stmts"].push_back("stmt stmts");
        ExPro["stmts"].push_back("E");
        ExPro["ifstmt"].push_back("if ( boolexpr ) then stmt else stmt");
        ExPro["whilestmt"].push_back("while ( boolexpr ) stmt");
        ExPro["assgstmt"].push_back("ID = arithexpr ;");
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
        ExPro["simpleexpr"].push_back("NUM");
        ExPro["simpleexpr"].push_back("( arithexpr )");
        LLCFG ExCFG=LLCFG(NonTerminal,terminal,ExPro);
        ExCFG.CreateByFirst();
        ExCFG.CreateByFollow();
        ExCFG.parsing(prog);
    }
    else//快速模式，直接利用预先处理好的文法进行解析，但牺牲了拓展性
    {
        //文法无左递归、公共左因子，直接构造解析表。
        //此外，根据该文法构造的解析表没有冲突项，文法是LL1文法
        GetFirst();
        GetFollow();

        //解析输入为token
        vector<string>input;//token数组
        vector<int>TokenLine;//哈希表：token下表-token所在行
        int line=1;//当前行

        //预处理
        int StartPtr=0;
        for(int i=0;i<prog.length();i++)
        {
            //跳过空字符，记录当前行
            if(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')
            {
                StartPtr++;
                if(prog[i]=='\n')
                    line++;
                continue;
            }
            else break;
        }
        if(prog[StartPtr]!='{')
        {
            prog="{"+prog;
            cout<<"语法错误,第"<<line<<"行,缺少\""<<'{'<<"\""<<endl;
        }


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
            {
                token+=prog[i++];
            }

            //token当前行
            TokenLine.push_back(line);
            //将token压入input
            input.push_back(token);
        }
        //添加终止符$
        input.push_back("$");


        //解析过程

        //栈构造
        vector<string>Stack;//数组模拟栈
        int top=0;//栈顶
        Stack.push_back("$");//压入$
        Stack.push_back("program");//压入开始符号
        top++;

        //输出构造
        vector<string>output;//输出数组
        vector<int>tabs;//每个输出token的对应tab数量

        //记录Stack中现有的token输出时的tab数量
        vector<int>StackTab;
        StackTab.push_back(0);
        StackTab.push_back(-1);

        for(int i=0;i<input.size();i++)//input识别从左往右，Stack是从右往左对应
        {

            //需要对Stack进行变化操作
            while(Stack[top]!=input[i])
            {
                int StackTopIndex=-1;//Stack栈顶非终结符对应的下标,实际意义为机器认知意义上的NonTerminal
                int InputLeftIndex=-1;//Input当前最左未处理输入终结符对应的下标,实际意义为机器认知意义上的Terminal

                //匹配StackTopIndex
                for(int j=0;j<14;j++)
                {
                    if(Stack[top]==NonTerminal[j])
                    {
                        StackTopIndex=j;
                        break;
                    }
                }

                    //匹配InputLeftIndex
                    for(int j=0;j<22;j++)
                    {
                        if(input[i]==terminal[j])
                            InputLeftIndex=j;
                    }
                    //若为空产生式，弹出栈顶，输出空产生式
                    //注意，空产生式是“E”，而不是“”，后者是匹配错误
                    if(table[StackTopIndex][InputLeftIndex]=="E")
                        {
                            output.push_back(Stack[top]);
                            tabs.push_back(StackTab[top]+1);
                            output.push_back("E");
                            tabs.push_back(StackTab[top]+2);
                            top--;
                        }
                    //否则
                    else
                    {
                        string production=table[StackTopIndex][InputLeftIndex];//查找产生式
                        string temp="";
                        vector<string>ProTokens;//解析产生式为Token
                        for(int j=0;j<production.length();j++)
                        {
                            if(production[j]==' ')
                            {
                                ProTokens.push_back(temp);
                                temp="";
                            }
                            else
                            {
                                temp+=production[j];
                                if(j==production.length()-1)ProTokens.push_back(temp);
                            }
                        }

                        //production为空，匹配出错，错误处理
                        if(ProTokens.size()==0)
                        {
                            int flag=0;
                            int ErrorPtr;
                            for(ErrorPtr=top;ErrorPtr>=0;ErrorPtr--)
                            {
                                flag=0;
                                for(int j=0;j<14;j++)
                                {
                                    if(Stack[ErrorPtr]==NonTerminal[j])
                                    {
                                       flag=1;
                                    }
                                }
                                if(!flag)break;
                            }
                            cout<<"语法错误,第"<<TokenLine[i]<<"行,缺少\""<<Stack[ErrorPtr]<<"\""<<endl;
                            i--;
                            input[i]=Stack[ErrorPtr];
                            continue;
                        }

                        //应用产生式，弹出非终结符，压入产生式
                        //先移出当前要替换的非终结符
                        int NowTab=StackTab[top];
                        output.push_back(Stack[top]);
                        tabs.push_back(StackTab[top]+1);
                        top--;
                        //入栈
                        for(int j=ProTokens.size()-1;j>=0;j--)//产生式倒序入栈
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                            while(top>StackTab.size()-1)StackTab.push_back(0);
                            Stack[top]=ProTokens[j];
                            StackTab[top]=NowTab+1;
                        }
                    }
            }
            //栈顶和input左端匹配
            output.push_back(Stack[top]);//输出
            tabs.push_back(StackTab[top]+1);
            top--;//弹出

            //继续读入input
        }


        //输出
        //注意output最后一行是$，不能输出
        for(int i=0;i<output.size()-1;i++)
        {
            for(int k=0;k<tabs[i];k++)
                cout<<"\t";
            if(i!=output.size()-2)
                cout<<output[i]<<endl;
            else
                cout<<output[i];//最后一行输出没有回车
        }

    }
    /********* End *********/

}
