// C语言词法分析器
#include <iostream>
#include <sstream>
#include<fstream>
#include <string.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <queue>

using namespace std;

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




class LR1CFG
{
public:

    //新的数据结构：LR项 定义
    class Item
    {
    private:
        //Item由以下组成
        //项产生式item，为以string表征的产生式
        //左部，即产生式的左部非终结符
        //右部，即产生式的右端切割而成的Token流
        //符号，即LR1文法中需要进行Reduce的“向前搜索符号”，注意该文件中所有完全以终结符构成的组合均采用set
        //id，即该项出现的时间戳
        //count，静态变量，为总Item数目
        string item;//项
        string left;//项左部
        vector<string> right;//项右部
        set<string> symbols;//向前搜索符号
        static int count;

    public:
        int id;

        //无初始化参数
        Item()
        {

        }

        //参数是左部和右部，将右部切割为Token，添加“.”，并添加默认symbol
        Item(string LeftStr,string RightStr)
        {
            id=count++;
            left=LeftStr;
            right=StringToTokens(RightStr);
            symbols={"$"};
            item=LeftStr+"->"+RightStr;

            if(find(right.begin(),right.end(),".")==right.end())
                AddDot(0);
        }

        //参数是左部 右部 Symbols
        Item(string LeftStr,string RightStr,set<string>_symbols)
        {
            id=count++;
            left=LeftStr;
            right=StringToTokens(RightStr);
            symbols=_symbols;
            item=LeftStr+"->"+RightStr;

            if(find(right.begin(),right.end(),".")==right.end())
                AddDot(0);
        }

        //获取左部
        string GetLeft()
        {
            return left;
        }

        //获取右部
        vector<string> GetRight()
        {
            return right;
        }

        //获取item
        string GetItem()
        {
            item=left+"->"+TokensToString(right);
            return item;
        }

        //获取symbols
        set<string> GetSymbol()
        {
            return symbols;
        }


        //设置symbols
        void SetSymbol(set<string> new_symbol)
        {
            symbols=new_symbol;
        }

        //给文法加点
        void AddDot(int pos)
        {
            right.insert(right.begin()+pos,".");
        }



        //重载运算符，两个Item是否相等只由 左串和右Token流组成的item 和 向前搜索符号集合symbols 决定
        bool operator ==(Item &x)
        {
            if(GetItem()==x.GetItem())
                if(SymbolsToString(GetSymbol())==SymbolsToString(x.GetSymbol()))
                    return 1;
            return 0;

        }
        //得到"."后面的一个文法符号
        /*vector<string> GetPath()
        {
            /*vector<string>buffer=split(item,".");
            buffer[1].erase(0,1);
            string first=firstWord(buffer[1]);
            return first;
            vector<string>DotIt=right.find(".");
            return
        }

        //返回下一个点的串
        vector<string> nextDot()
        {
            vector<string>::iterator DotIt=right.find(".");
            buffer[1].erase(0,1);
            string first=firstWord(buffer[1]);
            int nextPos=dotPos+first.size();
            right.erase(right.find("."),2);
            right.insert(nextPos," .");
            return right;
        }*/
    };

    /*class State
    {
    private:
        set<Item>items;
        int id;
    public:
        void State(set<Item>_items,int _id)
        {
            id=_id;
            items=_items;
        }
    };*/
    vector<string>NonTerminal;//非终结符序列
    vector<string>terminal;//终结符序列
    string StartSym;//开始符号
    map<string,vector<vector<string> > >productions;//产生式,每个产生式对应一串非终止符->Token流（即vector<string>）。注意一个非终止符可能有多条产生式，所以是vector<vector<string>>

    map<int,map<string,vector<string> > >Action;
    map<int,map<string,int> >Goto;//Action和Goto解析表，这里由于已经有了解析表，我们不再创建自动机
    map<string,bool>Nullable;//判定一个非终结符是否可以变为空串
    map<string,bool>Visit;//搜索标记，用于深度优先搜索。为1表示已经搜索过了，避免重复搜索
    map<string,set<string> >first;//First集
    map<string,set<string> >follow;//Follow集

    vector<set<Item> >States;//状态集合
    Item StartItem;//开始项，整个LR1文法自动机的开始

    //错误计数和错误限制
    int ErrorCount;
    int ErrorLimit;

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

    //用于判定一个项是否在某个状态中
    bool ItemFind(set<Item> State,Item tofind)
    {
        for(Item i:State)
        {
            if(tofind==i)return 1;
        }
        return 0;
    }

    //增广文法，增加新的开始符号及其唯一产生式，并更改其为开始符号
    void Extension()
    {
        string NewStartSym=StartSym+"'";
        NonTerminal.insert(NonTerminal.begin(),NewStartSym);

        vector<vector<string> >NewPro;
        NewPro.push_back({StartSym});
        productions[NewStartSym]=NewPro;

        StartSym=NewStartSym;
    }



    /*
        初始化需要输入：
            1、非终结符序列
            2、终结符序列
            3、输入的单条产生式采用string，在初始化函数终解析为Token流，即vector<string>，所以参数_productions值为vector<string>
               字符串对字符串映射的产生式序列
               函数会将产生式序列中的值变为Token数组，键不变
    */
    LR1CFG(vector<string>_NonTerminal,vector<string>_terminal,map<string,vector<string> > _productions, int _ErrorLimit)
    {
        NonTerminal=_NonTerminal;
        terminal=_terminal;
        StartSym=_NonTerminal[0];
        for(map<string,vector<string> >::iterator it=_productions.begin();it!=_productions.end();it++)
        {
            for(int i=0;i<_productions[it->first].size();i++)
                productions[it->first].push_back(StringToTokens(_productions[it->first][i]));
        }

        //增广文法
        Extension();

        //设置初始项
        StartItem=Item(StartSym,TokensToString(productions[StartSym][0]));

        //初始化错误计数和错误限制
        ErrorLimit=_ErrorLimit;
        ErrorCount=5;

    }

    /*
        创建First集
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
                    first[NowNonTerminal].insert(NowPro[0]);//录入First集合
                }
                else
                {
                    for(int j=0;j<=NowPro.size();j++)
                    {
                        if(j==NowPro.size())//迭代到了末尾，说明整个产生式可以为空，赋值Nullable
                        {
                            Nullable[NowNonTerminal]=1;
                            cout<<NowPro[0]<<NowNonTerminal<<endl;
                            break;
                        }

                        if(NowPro[j]=="E")
                        {
                            Nullable[NowNonTerminal]=1;
                            continue;
                        }
                        if(isTer(NowPro[j]))//遇到终结符，赋值并退出
                        {
                            first[NowNonTerminal].insert(NowPro[j]);//录入First集合
                            break;
                        }

                        dfs(NowPro[j]);//遇到非终结符，递归地求解其First集合
                        for(set<string>::iterator it=first[NowPro[j]].begin();it!=first[NowPro[j]].end();it++)//对该非终结符的First集合中的每个元素，录入现在i迭代的非终结符的table行和First集合
                        {
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
        创建Follow集
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
    }

    //当且仅当求取First后，函数有效
    //求取某一段Token流的First集合，由于这个函数产生的First集合一般作为中间用，没有Nullable作为辅助信息，所以返回值可以包含“E”
    set<string> GetFirst(vector<string> Tokens)
    {
        //剩余Token流为空
        if(!Tokens.size())return {"E"};

        //Token流以终结符起头
        if(isTer(Tokens[0]))
            return {Tokens[0]};

        //其余情况
        set<string> ret;
        for(int i=0;i<=Tokens.size();i++)
        {
            //遍历到末尾，说明前面所有的Token均可以为空
            if(i==Tokens.size())
            {
                ret.insert("E");
                return ret;
            }

            //遇到终结符，直接插入并返回
            if(isTer(Tokens[i]))
            {
                ret.insert(Tokens[i]);
                return ret;
            }

            //置入当前非终结符的所有first，并判定其是否可以为空，若不可以则return，否则继续往下拓展
            ret.insert(first[Tokens[i]].begin(),first[Tokens[i]].end());
            if(!Nullable[Tokens[i]])return ret;
        }
    }

    //求某个项的闭包，即“状态”
    set<Item> GetClosure(Item item)
    {
        set<Item> C;//项目闭包
        C.insert(item);


        //利用广度优先搜索求取当前Item的闭包
        queue<Item> bfs;//队列
        bfs.push(item);//初始置入


        while(!bfs.empty())
        {
            Item now=bfs.front();
            bfs.pop();

            //获取当前的右端Token流
            vector<string>NowRight=now.GetRight();

            //找到“.”的迭代器，并走到下一格
            vector<string>::iterator DotPos=find(NowRight.begin(),NowRight.end(),".");
            DotPos++;

            //如果当前式子已经处理完成（“.”后面没有内容），continue
            if(DotPos==NowRight.end())continue;

            //如果“.”后是终结符，continue
            if(isTer(*DotPos))continue;

            //获取该非终结符，该非终结符即为下一个Item的左串，走到下一格
            string NowNonTerminal=*DotPos;
            DotPos++;

            //接受剩余Token
            vector<string>rest;
            rest.insert(rest.begin(),DotPos,NowRight.end());

            //求取剩余Token流的First集合，该集合可以包含“E”，此即为下一个Item的Symbols
            set<string>FirstOfRest=GetFirst(rest);


            //如果剩余Token流可以为空，将当前Item的symbols也添加到下一个Item的symbols中
            if(find(FirstOfRest.begin(),FirstOfRest.end(),"E")!=FirstOfRest.end())
            {
                set<string>NowSymbols=now.GetSymbol();
                for(set<string>::iterator it=NowSymbols.begin();it!=NowSymbols.end();it++)
                {
                    FirstOfRest.insert(*it);
                }
            }

            //移除掉E
            if(FirstOfRest.find("E")!=FirstOfRest.end())
                FirstOfRest.erase(FirstOfRest.find("E"));

            //对每条下一Item左串的产生式，产生Item
            for(int i=0;i<productions[NowNonTerminal].size();i++)
            {
                //生成该Item
                vector<string> NowPro=productions[NowNonTerminal][i];
                if(NowPro[0]=="E")NowPro={};
                Item temp=Item(NowNonTerminal,TokensToString(NowPro),FirstOfRest);

                //如果该Item已经存在该State中，continue，避免重复搜索

                //置入状态并推入队列中
                if(ItemFind(C,temp))continue;
                C.insert(temp);
                bfs.push(temp);
            }
        }
        return C;
    }

    //将状态中的Item按将要接受Token分类为不同的vector
    map<string,vector<Item> > Classify(set<Item> State)
    {
        map<string,vector<Item> > ret;
        for(set<Item>::iterator it=State.begin();it!=State.end();it++)
        {
            Item NowItem=*it;
            vector<string>NowRight=NowItem.GetRight();
            vector<string>::iterator DotPos=find(NowRight.begin(),NowRight.end(),".");
            string TokenToGet;

            DotPos++;
            //如果"."在Item.right的末尾，将将要接受Token赋值为空串
            if(DotPos==NowRight.end())
                TokenToGet="";
            else TokenToGet=*DotPos;
            ret[TokenToGet].push_back(*it);

        }
        return ret;
    }

    //构造Action表和Goto表
    /*
      此处解析表定义如下：
        Action为双层map，行键为状态号，即其在States中的ID。列键为终结符。
        值为Shift动作，为一个Token流，Token流中有两个Token：“S”和Shift的状态号对应的字符串。
        值为Reduce动作，为一个Token流，Token流由三部分组成：“R”，产生式左端非终结符，剩余Token产生式右端对应的Token流
        注：Token流以vector<string>实现

        Goto为双层map，行键为状态号，列键为非终结符。值为goto的状态号。
    */
    void CreateTable()
    {
        //创建初始状态I0，并推入States集合
        //StateCnt为状态数计数器
        set<Item>State;
        int StateCnt=0;
        State=GetClosure(StartItem);
        States.push_back(State);
        StateCnt++;

        //利用广度优先搜索，搜索出整个变迁图，进而整理出Action和Goto
        queue<set<Item> >bfs;
        bfs.push(State);

        //bfscnt为当前搜索完成的状态数
        //注意到新状态会同时推入States和bfs队列，故有bfscnt为当前搜索的State在States中的下标
        int bfscnt=0;

        //BFS
        while(!bfs.empty())
        {
            //取出状态
            set<Item>Now=bfs.front();
            bfs.pop();

            //将状态中的Item按照边上的字符，即“.”后的Token分类
            map<string,vector<Item> >ClassifedState=Classify(Now);

            for(map<string,vector<Item> >::iterator it=ClassifedState.begin();it!=ClassifedState.end();it++)
            {
                if(it->first=="")continue;//按照ClassifiedState的定义，键为空串说明此处要进行归约，跳过


                //构造新状态
                set<Item>NewState;
                //对每个Item，将点的位置后移，并求闭包，存入新状态中
                for(int i=0;i<it->second.size();i++)
                {
                    //取出Item、左部、右部、向前搜索符号
                    Item ItemPtr=(it->second)[i];
                    string NewLeft=ItemPtr.GetLeft();
                    vector<string> NewRight=ItemPtr.GetRight();
                    set<string>NewSymbols=ItemPtr.GetSymbol();

                    //修剪右部，移除“.”，改为放到两格以后
                    vector<string>::iterator DotPos=find(NewRight.begin(),NewRight.end(),".");
                    DotPos=NewRight.erase(DotPos);
                    DotPos++;
                    NewRight.insert(DotPos,".");

                    //创建新的Item，求取闭包，置入新状态
                    Item NewItem=Item(NewLeft,TokensToString(NewRight),NewSymbols);
                    set<Item>Closure=GetClosure(NewItem);
                    NewState.insert(Closure.begin(),Closure.end());
                }

                //确认该状态是否已经出现过
                vector<set<Item> >::iterator StatePos=find(States.begin(),States.end(),NewState);

                //若为新状态，推入States和bfs队列，补充解析表
                if(StatePos==States.end())
                {
                    int StateID=StateCnt;
                    StateCnt++;

                    //若边上的字符为终结符，补充Action
                    if(isTer(it->first))
                    {
                        if(Action[bfscnt][it->first].size())
                            cout<<"文法 "<<bfscnt<<"行 "<<it->first<<"列出现错误"<<endl;
                        Action[bfscnt][it->first]={"S",IntToString(StateID)};
                        States.push_back(NewState);
                    }
                    //否则补充Goto
                    else
                    {
                        Goto[bfscnt][it->first]=StateID;
                        States.push_back(NewState);
                    }
                    bfs.push(NewState);
                }
                else//否则直接修改解析表
                {
                    int StateID=StatePos-States.begin();
                    if(isTer(it->first))
                    {
                        if(Action[bfscnt][it->first].size())
                            cout<<"文法 "<<bfscnt<<"行 "<<it->first<<"列出现错误"<<endl;
                        Action[bfscnt][it->first]={"S",IntToString(StateID)};
                    }
                    else
                    {
                        Goto[bfscnt][it->first]=StateID;
                    }
                }
            }

            //归约项
            //取出所有待归约项
            vector<Item >ItemToReduce=ClassifedState[""];
            for(int i=0;i<ItemToReduce.size();i++)
            {
                //取出Item及其左部、右部、向前搜索符号
                Item ReduceItem=ItemToReduce[i];
                string ReduceItemLeft=ReduceItem.GetLeft();
                vector<string>ReduceItemRight=ReduceItem.GetRight();
                set<string> ReduceSymbol=ReduceItem.GetSymbol();

                //将右部中的“.”移除，作为归约产生式
                vector<string>::iterator DotPos=find(ReduceItemRight.begin(),ReduceItemRight.end(),".");
                ReduceItemRight.erase(DotPos);

                //归约式以R起头，然后是产生式左端，然后是产生式右端
                vector<string>ReducePro;
                ReducePro.push_back("R");
                ReducePro.push_back(ReduceItemLeft);
                if(ReduceItemRight.size()==0)
                {
                    ReducePro.insert(ReducePro.end(),"E");
                }
                else
                {
                    ReducePro.insert(ReducePro.end(),ReduceItemRight.begin(),ReduceItemRight.end());
                }

                //补充Action表
                for(set<string>::iterator it=ReduceSymbol.begin();it!=ReduceSymbol.end();it++)
                {
                    if(Action[bfscnt][*it].size())
                            cout<<"文法 "<<bfscnt<<"行 "<<*it<<"列出现错误"<<endl;
                    Action[bfscnt][*it]=ReducePro;
                }

            }
            //搜索完成状态数加一
            bfscnt++;
        }
    }

    void parsing(string prog)
    {
        //解析栈、输入、输出构造
        vector<string>Stack;
        vector<string>input;

        //输出分为两半，OutLeft是每一步所用的产生式的左部/OutRight则是对应的右部
        vector<vector<string> >OutRight;
        vector<string>OutLeft;
        vector<int>TokenLine;
        int line=1;//标识当前行
        int top=0;//栈顶

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
            {
                token+=prog[i++];
            }

            //预处理，该错误不计入错误限制
            if(input.size()==0)
            {
                if(token!="{")
                {
                    cout<<"语法错误，第"<<line<<"行，缺少\"{\""<<endl;
                    input.push_back("{");
                    TokenLine.push_back(line);
                }
            }
            //token当前行
            TokenLine.push_back(line);
            //将token压入input
            input.push_back(token);
            {
                if(prog[i]=='\n')
                    line++;
                continue;
            }
        }
        //添加终止符$
        input.push_back("$");

        vector<string>inputbk=input;
        int DerivePtr=input.size()-1;

        //解析过程
        Stack.push_back("0");

        for(int i=0;i<input.size();i++)
        {
            //接受标识
            bool acc=0;
            //一个Token可能有多次动作
            while(1)
            {
                //栈顶状态
                int NowState=StringToInt(Stack[top]);
                //根据Action表、栈顶状态、Input最左Token查询当前动作
                vector<string>NowAction=Action[NowState][input[i]];

                //当前动作为空，错误处理
                if(NowAction.size()==0)
                {
                    //采用递归模拟归约法
                    set<string>SymbolsToReduce;

                    //从当前状态取出所有进度完成的项的前向搜索符号
                    for(set<Item>::iterator it=States[NowState].begin();it!=States[NowState].end();it++)
                    {
                        Item TempItem=*it;
                        vector<string>NowRight=TempItem.GetRight();
                        vector<string>::iterator DotPos=find(NowRight.begin(),NowRight.end(),".");
                        if(DotPos+1==NowRight.end())
                        {

                            {
                                set<string>NowSymbols=TempItem.GetSymbol();
                                SymbolsToReduce.insert(NowSymbols.begin(),NowSymbols.end());
                            }
                        }
                    }

                    //如果没有，错误无法处理，return
                    if(SymbolsToReduce.size()==0)return;

                    //否则，对所有的前向搜索符号
                    //perror的作用是模拟整个解析器的当前状态，但向input的前一格添加了当前搜索的前向搜索符号
                    //如果可以完成解析，perror返回1，说明当前符号可用，向真正的input添加该字符；否则继续搜索
                    //如果未能搜索到可用符号，return
                    bool ErrorFlag=0;
                    for(set<string>::iterator it=SymbolsToReduce.begin();it!=SymbolsToReduce.end();it++)
                    {
                        ErrorFlag=perror(Stack,top,input,TokenLine,i,*it);
                        if(ErrorFlag)
                        {
                            i--;
                            input[i]=*it;
                            cout<<"语法错误，第"<<TokenLine[i]<<"行，缺少\""<<*it<<"\""<<endl;
                            break;
                        }
                    }
                    if(ErrorFlag)continue;
                    else return;
                }


                //Shift动作：压入input的最左Token和新状态，并跳出循环检测下一个Token
                if(NowAction[0]=="S")
                {
                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                    Stack[top]=input[i];

                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                    Stack[top]=NowAction[1];


                    break;
                }

                //Reduce动作
                if(NowAction[0]=="R")
                {
                    //取出Reduce所用产生式的左部
                    string ReduceLeft=NowAction[1];

                    //如果左部为增广文法中的开始符号，直接acc并退出循环
                    if(ReduceLeft==StartSym)
                    {
                        acc=1;
                        break;
                    }

                    //取出动作中除了“R”、左部的剩余Token，即产生式右部
                    vector<string>rest;
                    rest.assign(NowAction.begin()+2,NowAction.end());

                    //左输出中置入当前产生式左部
                    OutLeft.push_back(ReduceLeft);

                    //如果当前产生式右部为空，在右输出中置入一个空Token流，否则置入当前右部
                    if(rest[0]=="E")OutRight.push_back({});
                    else OutRight.push_back(rest);

                    //弹出右部Token流对应的Token数量两倍的元素（一个Token、一个状态）
                    for(int i=0;i<rest.size();i++)
                    {
                        if(rest[i]=="E")
                            break;
                        top--;
                        top--;
                    }

                    //执行Goto动作
                    //取出栈顶状态
                    int TopState=StringToInt(Stack[top]);

                    //压入新Token
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=ReduceLeft;

                    //压入新状态
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=IntToString(Goto[TopState][ReduceLeft]);
                }


            }
            //如果已接受，退出
            if(acc)break;
        }

        //输出：由于BOTTOMUP分析过程是倒推的，我们需要反向应用OutLeft/OutRight对应的产生式来输出

        //每一步需要输出的Token流
        vector<string>OutTokens;

        //从最后一条产生式的左部开始
        int cnt=OutLeft.size();
        OutTokens.push_back(OutLeft[cnt-1]);
        cout<<TokensToString(OutTokens)<<" => "<<endl;

        //应用产生式，注意推导是倒序的
        for(int i=OutLeft.size()-1;i>=0;i--)
        {
            //当前产生式的左右部
            string NowLeft=OutLeft[i];
            vector<string> NowRight=OutRight[i];

            //由于BOTTOMUP对应最右推导，从上一步的OutTokens流中的最右边开始寻找被推导的左部
            int ReducePtr;
            for(ReducePtr=OutTokens.size()-1;ReducePtr>=0;ReducePtr--)
            {
                if(OutTokens[ReducePtr]==NowLeft)
                    break;
            }

            //从OutTokens中移除该左部，用对应的产生式右部替换之，以完成一步推导
            vector<string>::iterator ReducePos=OutTokens.erase(OutTokens.begin()+ReducePtr);
            OutTokens.insert(ReducePos,NowRight.begin(),NowRight.end());

            //输出这一步的OutTokens
            if(i)cout<<TokensToString(OutTokens)<<" => "<<endl;
            else cout<<TokensToString(OutTokens)<<" ";
        }
    }

    bool perror(vector<string>Stack,int top,vector<string>input,vector<int>TokenLine,int ReadPtr,string ModifiedSymbol)
    {
        ReadPtr--;
        input[ReadPtr]=ModifiedSymbol;

        ErrorCount++;
        int ErrorCountBK=ErrorCount;//备份，避免递归错误处理中的重复计数
        if(ErrorCount==ErrorLimit)return 0;
        //模拟当前解析器状态，继续解析
        for(int i=ReadPtr;i<input.size();i++)
        {
            //接受标识
            bool acc=0;

            //一个Token可能有多次动作
            while(1)
            {
                //栈顶状态
                int NowState=StringToInt(Stack[top]);
                //根据Action表、栈顶状态、Input最左Token查询当前动作
                vector<string>NowAction=Action[NowState][input[i]];
                if(NowAction.size()==0)
                {
                    //采用模拟归约法
                    set<string>SymbolsToReduce;

                    //从当前状态取出所有进度完成的项的前向搜索符号
                    for(set<Item>::iterator it=States[NowState].begin();it!=States[NowState].end();it++)
                    {
                        Item TempItem=*it;
                        vector<string>NowRight=TempItem.GetRight();
                        vector<string>::iterator DotPos=find(NowRight.begin(),NowRight.end(),".");
                        if(DotPos+1==NowRight.end())
                        {

                            {
                                set<string>NowSymbols=TempItem.GetSymbol();
                                SymbolsToReduce.insert(NowSymbols.begin(),NowSymbols.end());
                            }
                        }
                    }

                    //如果没有，错误无法处理，return
                    if(SymbolsToReduce.size()==0)return 0;

                    //否则，对所有的前向搜索符号
                    //perror的作用是模拟整个解析器的当前状态，但向input的前一格添加了当前搜索的前向搜索符号
                    //如果可以完成解析，perror返回1，说明当前符号可用，向真正的input添加该字符；否则继续搜索
                    //如果未能搜索到可用符号，return
                    bool ErrorFlag=0;
                    for(set<string>::iterator it=SymbolsToReduce.begin();it!=SymbolsToReduce.end();it++)
                    {
                        ErrorFlag=perror(Stack,top,input,TokenLine,i,*it);
                        ErrorCount=ErrorCountBK;//避免重复计数
                        if(ErrorFlag)
                        {
                            i--;
                            input[i]=*it;
                            break;
                        }
                    }
                    if(ErrorFlag)continue;
                    else return 0;
                }
                //Shift动作：压入input的最左Token和新状态，并跳出循环检测下一个Token
                if(NowAction[0]=="S")
                {
                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                    Stack[top]=input[i];

                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//这里的栈是vector，进行下标检测
                    Stack[top]=NowAction[1];


                    break;
                }

                //Reduce动作
                if(NowAction[0]=="R")
                {
                    //取出Reduce所用产生式的左部
                    string ReduceLeft=NowAction[1];
                    //如果左部为增广文法中的开始符号，直接acc并退出循环
                    if(ReduceLeft==StartSym)
                    {
                        acc=1;
                        return 1;
                    }

                    //取出动作中除了“R”、左部的剩余Token，即产生式右部
                    vector<string>rest;
                    rest.assign(NowAction.begin()+2,NowAction.end());

                    //弹出右部Token流对应的Token数量两倍的元素（一个Token、一个状态）
                    for(int i=0;i<rest.size();i++)
                    {
                        if(rest[i]=="E")
                            break;
                        top--;
                        top--;
                    }

                    //执行Goto动作
                    //取出栈顶状态
                    int TopState=StringToInt(Stack[top]);

                    //压入新Token
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=ReduceLeft;

                    //压入新状态
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=IntToString(Goto[TopState][ReduceLeft]);
                }


            }
        }
    }

};
int LR1CFG::Item::count=0;

//重载运算符
bool operator <(const LR1CFG::Item &x,const LR1CFG::Item &y)
{
    return x.id<y.id;
}

bool operator ==(const set<LR1CFG::Item> &x,const set<LR1CFG::Item> &y)
{
    auto it1=x.begin();
    auto it2=y.begin();

    for(; it1!=x.end(),it2!=y.end(); it1++,it2++)
    {
        LR1CFG::Item a=*it1;
        LR1CFG::Item b=*it2;
        if(a==b)
            continue;

        //有一个项目不相等，两项目集一定不相等
        else
            return false;
    }
    return true;
}


/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
    char c;
    while(scanf("%c",&c)!=EOF){
        prog += c;
    }
}
/* 你可以添加其他函数 */

void Analysis()
{
    string prog;
    read_prog(prog);
    /* 骚年们 请开始你们的表演 */
    /********* Begin *********/
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
    //设定错误限制数目
    int ErrorLimit=20;
    LR1CFG ExCFG=LR1CFG(NonTerminal,terminal,ExPro,ErrorLimit);
    ExCFG.CreateByFirst();
    ExCFG.CreateByFollow();
    ExCFG.CreateTable();
    ExCFG.parsing(prog);

    /********* End *********/

}
