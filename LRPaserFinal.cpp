// C���Դʷ�������
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

/* ���������������� */
vector<string> StringToTokens(string str)//�ַ���תToken���麯��
{
    vector<string> ret;//�Կո�Ϊ�ָ����������ַ���ΪToken����
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

string TokensToString(vector<string>Tokens)//Token��ת�ַ���
{
    if(Tokens.size()==0)return "";
    string ret;
    for(int i=0;i<Tokens.size()-1;i++)
        ret+=Tokens[i]+" ";
    ret+=Tokens[Tokens.size()-1];
    return ret;

}

string SymbolsToString(set<string>symbols)//���ż���ת�ַ��������ڶԱ�first/follow����
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

    //�µ����ݽṹ��LR�� ����
    class Item
    {
    private:
        //Item���������
        //�����ʽitem��Ϊ��string�����Ĳ���ʽ
        //�󲿣�������ʽ���󲿷��ս��
        //�Ҳ���������ʽ���Ҷ��и���ɵ�Token��
        //���ţ���LR1�ķ�����Ҫ����Reduce�ġ���ǰ�������š���ע����ļ���������ȫ���ս�����ɵ���Ͼ�����set
        //id����������ֵ�ʱ���
        //count����̬������Ϊ��Item��Ŀ
        string item;//��
        string left;//����
        vector<string> right;//���Ҳ�
        set<string> symbols;//��ǰ��������
        static int count;

    public:
        int id;

        //�޳�ʼ������
        Item()
        {

        }

        //�������󲿺��Ҳ������Ҳ��и�ΪToken����ӡ�.���������Ĭ��symbol
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

        //�������� �Ҳ� Symbols
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

        //��ȡ��
        string GetLeft()
        {
            return left;
        }

        //��ȡ�Ҳ�
        vector<string> GetRight()
        {
            return right;
        }

        //��ȡitem
        string GetItem()
        {
            item=left+"->"+TokensToString(right);
            return item;
        }

        //��ȡsymbols
        set<string> GetSymbol()
        {
            return symbols;
        }


        //����symbols
        void SetSymbol(set<string> new_symbol)
        {
            symbols=new_symbol;
        }

        //���ķ��ӵ�
        void AddDot(int pos)
        {
            right.insert(right.begin()+pos,".");
        }



        //���������������Item�Ƿ����ֻ�� �󴮺���Token����ɵ�item �� ��ǰ�������ż���symbols ����
        bool operator ==(Item &x)
        {
            if(GetItem()==x.GetItem())
                if(SymbolsToString(GetSymbol())==SymbolsToString(x.GetSymbol()))
                    return 1;
            return 0;

        }
        //�õ�"."�����һ���ķ�����
        /*vector<string> GetPath()
        {
            /*vector<string>buffer=split(item,".");
            buffer[1].erase(0,1);
            string first=firstWord(buffer[1]);
            return first;
            vector<string>DotIt=right.find(".");
            return
        }

        //������һ����Ĵ�
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
    vector<string>NonTerminal;//���ս������
    vector<string>terminal;//�ս������
    string StartSym;//��ʼ����
    map<string,vector<vector<string> > >productions;//����ʽ,ÿ������ʽ��Ӧһ������ֹ��->Token������vector<string>����ע��һ������ֹ�������ж�������ʽ��������vector<vector<string>>

    map<int,map<string,vector<string> > >Action;
    map<int,map<string,int> >Goto;//Action��Goto���������������Ѿ����˽��������ǲ��ٴ����Զ���
    map<string,bool>Nullable;//�ж�һ�����ս���Ƿ���Ա�Ϊ�մ�
    map<string,bool>Visit;//������ǣ������������������Ϊ1��ʾ�Ѿ��������ˣ������ظ�����
    map<string,set<string> >first;//First��
    map<string,set<string> >follow;//Follow��

    vector<set<Item> >States;//״̬����
    Item StartItem;//��ʼ�����LR1�ķ��Զ����Ŀ�ʼ

    //��������ʹ�������
    int ErrorCount;
    int ErrorLimit;

    //����С�����������ж�ĳ�����Ƿ��ǺϷ��ս��/���ս��
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

    //�����ж�һ�����Ƿ���ĳ��״̬��
    bool ItemFind(set<Item> State,Item tofind)
    {
        for(Item i:State)
        {
            if(tofind==i)return 1;
        }
        return 0;
    }

    //�����ķ��������µĿ�ʼ���ż���Ψһ����ʽ����������Ϊ��ʼ����
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
        ��ʼ����Ҫ���룺
            1�����ս������
            2���ս������
            3������ĵ�������ʽ����string���ڳ�ʼ�������ս���ΪToken������vector<string>�����Բ���_productionsֵΪvector<string>
               �ַ������ַ���ӳ��Ĳ���ʽ����
               �����Ὣ����ʽ�����е�ֵ��ΪToken���飬������
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

        //�����ķ�
        Extension();

        //���ó�ʼ��
        StartItem=Item(StartSym,TokensToString(productions[StartSym][0]));

        //��ʼ����������ʹ�������
        ErrorLimit=_ErrorLimit;
        ErrorCount=5;

    }

    /*
        ����First��
    */

    void dfs(string NowNonTerminal)
    {
            if(Visit[NowNonTerminal])return;//���Ѿ�������������
            Visit[NowNonTerminal]=1;//���

            for(int i=0;i<productions[NowNonTerminal].size();i++)//�Ը÷��ս����Ӧ��ÿ������ʽ��������
            {
                vector<string>NowPro=productions[NowNonTerminal][i];//ȡ���ò���ʽ

                if(NowPro[0]=="E")
                {
                    Nullable[NowNonTerminal]=1;
                    continue;
                }
                if(isTer(NowPro[0]))//���ò���ʽ���ս����ͷ��ֱ��¼��First���ͽ�����
                {
                    first[NowNonTerminal].insert(NowPro[0]);//¼��First����
                }
                else
                {
                    for(int j=0;j<=NowPro.size();j++)
                    {
                        if(j==NowPro.size())//��������ĩβ��˵����������ʽ����Ϊ�գ���ֵNullable
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
                        if(isTer(NowPro[j]))//�����ս������ֵ���˳�
                        {
                            first[NowNonTerminal].insert(NowPro[j]);//¼��First����
                            break;
                        }

                        dfs(NowPro[j]);//�������ս�����ݹ�������First����
                        for(set<string>::iterator it=first[NowPro[j]].begin();it!=first[NowPro[j]].end();it++)//�Ը÷��ս����First�����е�ÿ��Ԫ�أ�¼������i�����ķ��ս����table�к�First����
                        {
                            first[NowNonTerminal].insert(*it);

                        }
                        if(Nullable[NowPro[j]]!=1)
                                break;//����÷��ս��Ҳ����Ϊ�գ��������µݹ顣�����ж�ѭ����
                    }
                }
            }
    }
    void CreateByFirst()
    {
        //���׸����ս����ʼ�����������������
        for(int i=0;i<NonTerminal.size();i++)
            dfs(NonTerminal[i]);
    }

    /*
        ����Follow��
    */
    void CreateByFollow()
    {
        //�ڿ�ʼ���ŵ�follow������$
        follow[StartSym].insert("$");

        //������follow���㷨Ϊ��
        //��ÿ�����ս�������������ұߵĲ���ʽ���������������ս�������ս����first������follow
        //�ж��ٸ�NonTerminal������������ظ����ٴ�
        int NTCnt=NonTerminal.size();
        while(NTCnt--)
        {
            //�������з��ս��
            for(int i=0;i<NonTerminal.size();i++)
            {
                string NowNonTerminal=NonTerminal[i];
                //�������в���ʽ��Ѱ���ұ߰�����ǰ���ս���Ĳ���ʽ
                //����ֱ����iterator����productions�����쳣���ĳɸ��ݷ��ս��ȥ����productions���е���
                for(int j=0;j<NonTerminal.size();j++)
                {
                    string NTKey=NonTerminal[j];
                    for(int k=0;k<productions[NTKey].size();k++)
                    {
                        vector<string>NowPro=productions[NTKey][k];//��ǰ����ʽ

                        //����ǰ������������ǰ�����ķ��ս����continue
                        vector<string>::iterator FindNowNT=find(NowPro.begin(),NowPro.end(),NowNonTerminal);
                        if(FindNowNT==NowPro.end())
                        {
                            continue;
                        }

                        //����ǰ����ʽΪNTKey->��NowNonTerminal������ʽ
                        //��NowNonTerminal����һ��Token��ʼ��ΪNowNonTerminal��ȡfollow����
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
                            //��һ�����ս������Ϊ�գ��������µ���
                            if(!Nullable[*FindNowNT])
                                break;
                        }
                        //���NowNonTerminal������ΪNTKey����ʽ��β������NTKey��follow����NowNonTerminal��follow
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

    //���ҽ�����ȡFirst�󣬺�����Ч
    //��ȡĳһ��Token����First���ϣ������������������First����һ����Ϊ�м��ã�û��Nullable��Ϊ������Ϣ�����Է���ֵ���԰�����E��
    set<string> GetFirst(vector<string> Tokens)
    {
        //ʣ��Token��Ϊ��
        if(!Tokens.size())return {"E"};

        //Token�����ս����ͷ
        if(isTer(Tokens[0]))
            return {Tokens[0]};

        //�������
        set<string> ret;
        for(int i=0;i<=Tokens.size();i++)
        {
            //������ĩβ��˵��ǰ�����е�Token������Ϊ��
            if(i==Tokens.size())
            {
                ret.insert("E");
                return ret;
            }

            //�����ս����ֱ�Ӳ��벢����
            if(isTer(Tokens[i]))
            {
                ret.insert(Tokens[i]);
                return ret;
            }

            //���뵱ǰ���ս��������first�����ж����Ƿ����Ϊ�գ�����������return���������������չ
            ret.insert(first[Tokens[i]].begin(),first[Tokens[i]].end());
            if(!Nullable[Tokens[i]])return ret;
        }
    }

    //��ĳ����ıհ�������״̬��
    set<Item> GetClosure(Item item)
    {
        set<Item> C;//��Ŀ�հ�
        C.insert(item);


        //���ù������������ȡ��ǰItem�ıհ�
        queue<Item> bfs;//����
        bfs.push(item);//��ʼ����


        while(!bfs.empty())
        {
            Item now=bfs.front();
            bfs.pop();

            //��ȡ��ǰ���Ҷ�Token��
            vector<string>NowRight=now.GetRight();

            //�ҵ���.���ĵ����������ߵ���һ��
            vector<string>::iterator DotPos=find(NowRight.begin(),NowRight.end(),".");
            DotPos++;

            //�����ǰʽ���Ѿ�������ɣ���.������û�����ݣ���continue
            if(DotPos==NowRight.end())continue;

            //�����.�������ս����continue
            if(isTer(*DotPos))continue;

            //��ȡ�÷��ս�����÷��ս����Ϊ��һ��Item���󴮣��ߵ���һ��
            string NowNonTerminal=*DotPos;
            DotPos++;

            //����ʣ��Token
            vector<string>rest;
            rest.insert(rest.begin(),DotPos,NowRight.end());

            //��ȡʣ��Token����First���ϣ��ü��Ͽ��԰�����E�����˼�Ϊ��һ��Item��Symbols
            set<string>FirstOfRest=GetFirst(rest);


            //���ʣ��Token������Ϊ�գ�����ǰItem��symbolsҲ��ӵ���һ��Item��symbols��
            if(find(FirstOfRest.begin(),FirstOfRest.end(),"E")!=FirstOfRest.end())
            {
                set<string>NowSymbols=now.GetSymbol();
                for(set<string>::iterator it=NowSymbols.begin();it!=NowSymbols.end();it++)
                {
                    FirstOfRest.insert(*it);
                }
            }

            //�Ƴ���E
            if(FirstOfRest.find("E")!=FirstOfRest.end())
                FirstOfRest.erase(FirstOfRest.find("E"));

            //��ÿ����һItem�󴮵Ĳ���ʽ������Item
            for(int i=0;i<productions[NowNonTerminal].size();i++)
            {
                //���ɸ�Item
                vector<string> NowPro=productions[NowNonTerminal][i];
                if(NowPro[0]=="E")NowPro={};
                Item temp=Item(NowNonTerminal,TokensToString(NowPro),FirstOfRest);

                //�����Item�Ѿ����ڸ�State�У�continue�������ظ�����

                //����״̬�����������
                if(ItemFind(C,temp))continue;
                C.insert(temp);
                bfs.push(temp);
            }
        }
        return C;
    }

    //��״̬�е�Item����Ҫ����Token����Ϊ��ͬ��vector
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
            //���"."��Item.right��ĩβ������Ҫ����Token��ֵΪ�մ�
            if(DotPos==NowRight.end())
                TokenToGet="";
            else TokenToGet=*DotPos;
            ret[TokenToGet].push_back(*it);

        }
        return ret;
    }

    //����Action���Goto��
    /*
      �˴������������£�
        ActionΪ˫��map���м�Ϊ״̬�ţ�������States�е�ID���м�Ϊ�ս����
        ֵΪShift������Ϊһ��Token����Token����������Token����S����Shift��״̬�Ŷ�Ӧ���ַ�����
        ֵΪReduce������Ϊһ��Token����Token������������ɣ���R��������ʽ��˷��ս����ʣ��Token����ʽ�Ҷ˶�Ӧ��Token��
        ע��Token����vector<string>ʵ��

        GotoΪ˫��map���м�Ϊ״̬�ţ��м�Ϊ���ս����ֵΪgoto��״̬�š�
    */
    void CreateTable()
    {
        //������ʼ״̬I0��������States����
        //StateCntΪ״̬��������
        set<Item>State;
        int StateCnt=0;
        State=GetClosure(StartItem);
        States.push_back(State);
        StateCnt++;

        //���ù������������������������Ǩͼ�����������Action��Goto
        queue<set<Item> >bfs;
        bfs.push(State);

        //bfscntΪ��ǰ������ɵ�״̬��
        //ע�⵽��״̬��ͬʱ����States��bfs���У�����bfscntΪ��ǰ������State��States�е��±�
        int bfscnt=0;

        //BFS
        while(!bfs.empty())
        {
            //ȡ��״̬
            set<Item>Now=bfs.front();
            bfs.pop();

            //��״̬�е�Item���ձ��ϵ��ַ�������.�����Token����
            map<string,vector<Item> >ClassifedState=Classify(Now);

            for(map<string,vector<Item> >::iterator it=ClassifedState.begin();it!=ClassifedState.end();it++)
            {
                if(it->first=="")continue;//����ClassifiedState�Ķ��壬��Ϊ�մ�˵���˴�Ҫ���й�Լ������


                //������״̬
                set<Item>NewState;
                //��ÿ��Item�������λ�ú��ƣ�����հ���������״̬��
                for(int i=0;i<it->second.size();i++)
                {
                    //ȡ��Item���󲿡��Ҳ�����ǰ��������
                    Item ItemPtr=(it->second)[i];
                    string NewLeft=ItemPtr.GetLeft();
                    vector<string> NewRight=ItemPtr.GetRight();
                    set<string>NewSymbols=ItemPtr.GetSymbol();

                    //�޼��Ҳ����Ƴ���.������Ϊ�ŵ������Ժ�
                    vector<string>::iterator DotPos=find(NewRight.begin(),NewRight.end(),".");
                    DotPos=NewRight.erase(DotPos);
                    DotPos++;
                    NewRight.insert(DotPos,".");

                    //�����µ�Item����ȡ�հ���������״̬
                    Item NewItem=Item(NewLeft,TokensToString(NewRight),NewSymbols);
                    set<Item>Closure=GetClosure(NewItem);
                    NewState.insert(Closure.begin(),Closure.end());
                }

                //ȷ�ϸ�״̬�Ƿ��Ѿ����ֹ�
                vector<set<Item> >::iterator StatePos=find(States.begin(),States.end(),NewState);

                //��Ϊ��״̬������States��bfs���У����������
                if(StatePos==States.end())
                {
                    int StateID=StateCnt;
                    StateCnt++;

                    //�����ϵ��ַ�Ϊ�ս��������Action
                    if(isTer(it->first))
                    {
                        if(Action[bfscnt][it->first].size())
                            cout<<"�ķ� "<<bfscnt<<"�� "<<it->first<<"�г��ִ���"<<endl;
                        Action[bfscnt][it->first]={"S",IntToString(StateID)};
                        States.push_back(NewState);
                    }
                    //���򲹳�Goto
                    else
                    {
                        Goto[bfscnt][it->first]=StateID;
                        States.push_back(NewState);
                    }
                    bfs.push(NewState);
                }
                else//����ֱ���޸Ľ�����
                {
                    int StateID=StatePos-States.begin();
                    if(isTer(it->first))
                    {
                        if(Action[bfscnt][it->first].size())
                            cout<<"�ķ� "<<bfscnt<<"�� "<<it->first<<"�г��ִ���"<<endl;
                        Action[bfscnt][it->first]={"S",IntToString(StateID)};
                    }
                    else
                    {
                        Goto[bfscnt][it->first]=StateID;
                    }
                }
            }

            //��Լ��
            //ȡ�����д���Լ��
            vector<Item >ItemToReduce=ClassifedState[""];
            for(int i=0;i<ItemToReduce.size();i++)
            {
                //ȡ��Item�����󲿡��Ҳ�����ǰ��������
                Item ReduceItem=ItemToReduce[i];
                string ReduceItemLeft=ReduceItem.GetLeft();
                vector<string>ReduceItemRight=ReduceItem.GetRight();
                set<string> ReduceSymbol=ReduceItem.GetSymbol();

                //���Ҳ��еġ�.���Ƴ�����Ϊ��Լ����ʽ
                vector<string>::iterator DotPos=find(ReduceItemRight.begin(),ReduceItemRight.end(),".");
                ReduceItemRight.erase(DotPos);

                //��Լʽ��R��ͷ��Ȼ���ǲ���ʽ��ˣ�Ȼ���ǲ���ʽ�Ҷ�
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

                //����Action��
                for(set<string>::iterator it=ReduceSymbol.begin();it!=ReduceSymbol.end();it++)
                {
                    if(Action[bfscnt][*it].size())
                            cout<<"�ķ� "<<bfscnt<<"�� "<<*it<<"�г��ִ���"<<endl;
                    Action[bfscnt][*it]=ReducePro;
                }

            }
            //�������״̬����һ
            bfscnt++;
        }
    }

    void parsing(string prog)
    {
        //����ջ�����롢�������
        vector<string>Stack;
        vector<string>input;

        //�����Ϊ���룬OutLeft��ÿһ�����õĲ���ʽ����/OutRight���Ƕ�Ӧ���Ҳ�
        vector<vector<string> >OutRight;
        vector<string>OutLeft;
        vector<int>TokenLine;
        int line=1;//��ʶ��ǰ��
        int top=0;//ջ��

        //���������ΪToken��
        for(int i=0;i<prog.length();i++)
        {
            //�������ַ�����¼��ǰ��
            if(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')
            {
                if(prog[i]=='\n')
                    line++;
                continue;
            }

            //�����ַ���Ϊtoken
            string token="";
            while(!(prog[i] == '\n' || prog[i] == ' ' || prog[i] == '\t' || prog[i] == '\r')&&i<prog.length())
            {
                token+=prog[i++];
            }

            //Ԥ�����ô��󲻼����������
            if(input.size()==0)
            {
                if(token!="{")
                {
                    cout<<"�﷨���󣬵�"<<line<<"�У�ȱ��\"{\""<<endl;
                    input.push_back("{");
                    TokenLine.push_back(line);
                }
            }
            //token��ǰ��
            TokenLine.push_back(line);
            //��tokenѹ��input
            input.push_back(token);
            {
                if(prog[i]=='\n')
                    line++;
                continue;
            }
        }
        //�����ֹ��$
        input.push_back("$");

        vector<string>inputbk=input;
        int DerivePtr=input.size()-1;

        //��������
        Stack.push_back("0");

        for(int i=0;i<input.size();i++)
        {
            //���ܱ�ʶ
            bool acc=0;
            //һ��Token�����ж�ζ���
            while(1)
            {
                //ջ��״̬
                int NowState=StringToInt(Stack[top]);
                //����Action��ջ��״̬��Input����Token��ѯ��ǰ����
                vector<string>NowAction=Action[NowState][input[i]];

                //��ǰ����Ϊ�գ�������
                if(NowAction.size()==0)
                {
                    //���õݹ�ģ���Լ��
                    set<string>SymbolsToReduce;

                    //�ӵ�ǰ״̬ȡ�����н�����ɵ����ǰ����������
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

                    //���û�У������޷�����return
                    if(SymbolsToReduce.size()==0)return;

                    //���򣬶����е�ǰ����������
                    //perror��������ģ�������������ĵ�ǰ״̬������input��ǰһ������˵�ǰ������ǰ����������
                    //���������ɽ�����perror����1��˵����ǰ���ſ��ã���������input��Ӹ��ַ��������������
                    //���δ�����������÷��ţ�return
                    bool ErrorFlag=0;
                    for(set<string>::iterator it=SymbolsToReduce.begin();it!=SymbolsToReduce.end();it++)
                    {
                        ErrorFlag=perror(Stack,top,input,TokenLine,i,*it);
                        if(ErrorFlag)
                        {
                            i--;
                            input[i]=*it;
                            cout<<"�﷨���󣬵�"<<TokenLine[i]<<"�У�ȱ��\""<<*it<<"\""<<endl;
                            break;
                        }
                    }
                    if(ErrorFlag)continue;
                    else return;
                }


                //Shift������ѹ��input������Token����״̬��������ѭ�������һ��Token
                if(NowAction[0]=="S")
                {
                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                    Stack[top]=input[i];

                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                    Stack[top]=NowAction[1];


                    break;
                }

                //Reduce����
                if(NowAction[0]=="R")
                {
                    //ȡ��Reduce���ò���ʽ����
                    string ReduceLeft=NowAction[1];

                    //�����Ϊ�����ķ��еĿ�ʼ���ţ�ֱ��acc���˳�ѭ��
                    if(ReduceLeft==StartSym)
                    {
                        acc=1;
                        break;
                    }

                    //ȡ�������г��ˡ�R�����󲿵�ʣ��Token��������ʽ�Ҳ�
                    vector<string>rest;
                    rest.assign(NowAction.begin()+2,NowAction.end());

                    //����������뵱ǰ����ʽ��
                    OutLeft.push_back(ReduceLeft);

                    //�����ǰ����ʽ�Ҳ�Ϊ�գ��������������һ����Token�����������뵱ǰ�Ҳ�
                    if(rest[0]=="E")OutRight.push_back({});
                    else OutRight.push_back(rest);

                    //�����Ҳ�Token����Ӧ��Token����������Ԫ�أ�һ��Token��һ��״̬��
                    for(int i=0;i<rest.size();i++)
                    {
                        if(rest[i]=="E")
                            break;
                        top--;
                        top--;
                    }

                    //ִ��Goto����
                    //ȡ��ջ��״̬
                    int TopState=StringToInt(Stack[top]);

                    //ѹ����Token
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=ReduceLeft;

                    //ѹ����״̬
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=IntToString(Goto[TopState][ReduceLeft]);
                }


            }
            //����ѽ��ܣ��˳�
            if(acc)break;
        }

        //���������BOTTOMUP���������ǵ��Ƶģ�������Ҫ����Ӧ��OutLeft/OutRight��Ӧ�Ĳ���ʽ�����

        //ÿһ����Ҫ�����Token��
        vector<string>OutTokens;

        //�����һ������ʽ���󲿿�ʼ
        int cnt=OutLeft.size();
        OutTokens.push_back(OutLeft[cnt-1]);
        cout<<TokensToString(OutTokens)<<" => "<<endl;

        //Ӧ�ò���ʽ��ע���Ƶ��ǵ����
        for(int i=OutLeft.size()-1;i>=0;i--)
        {
            //��ǰ����ʽ�����Ҳ�
            string NowLeft=OutLeft[i];
            vector<string> NowRight=OutRight[i];

            //����BOTTOMUP��Ӧ�����Ƶ�������һ����OutTokens���е����ұ߿�ʼѰ�ұ��Ƶ�����
            int ReducePtr;
            for(ReducePtr=OutTokens.size()-1;ReducePtr>=0;ReducePtr--)
            {
                if(OutTokens[ReducePtr]==NowLeft)
                    break;
            }

            //��OutTokens���Ƴ����󲿣��ö�Ӧ�Ĳ���ʽ�Ҳ��滻֮�������һ���Ƶ�
            vector<string>::iterator ReducePos=OutTokens.erase(OutTokens.begin()+ReducePtr);
            OutTokens.insert(ReducePos,NowRight.begin(),NowRight.end());

            //�����һ����OutTokens
            if(i)cout<<TokensToString(OutTokens)<<" => "<<endl;
            else cout<<TokensToString(OutTokens)<<" ";
        }
    }

    bool perror(vector<string>Stack,int top,vector<string>input,vector<int>TokenLine,int ReadPtr,string ModifiedSymbol)
    {
        ReadPtr--;
        input[ReadPtr]=ModifiedSymbol;

        ErrorCount++;
        int ErrorCountBK=ErrorCount;//���ݣ�����ݹ�������е��ظ�����
        if(ErrorCount==ErrorLimit)return 0;
        //ģ�⵱ǰ������״̬����������
        for(int i=ReadPtr;i<input.size();i++)
        {
            //���ܱ�ʶ
            bool acc=0;

            //һ��Token�����ж�ζ���
            while(1)
            {
                //ջ��״̬
                int NowState=StringToInt(Stack[top]);
                //����Action��ջ��״̬��Input����Token��ѯ��ǰ����
                vector<string>NowAction=Action[NowState][input[i]];
                if(NowAction.size()==0)
                {
                    //����ģ���Լ��
                    set<string>SymbolsToReduce;

                    //�ӵ�ǰ״̬ȡ�����н�����ɵ����ǰ����������
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

                    //���û�У������޷�����return
                    if(SymbolsToReduce.size()==0)return 0;

                    //���򣬶����е�ǰ����������
                    //perror��������ģ�������������ĵ�ǰ״̬������input��ǰһ������˵�ǰ������ǰ����������
                    //���������ɽ�����perror����1��˵����ǰ���ſ��ã���������input��Ӹ��ַ��������������
                    //���δ�����������÷��ţ�return
                    bool ErrorFlag=0;
                    for(set<string>::iterator it=SymbolsToReduce.begin();it!=SymbolsToReduce.end();it++)
                    {
                        ErrorFlag=perror(Stack,top,input,TokenLine,i,*it);
                        ErrorCount=ErrorCountBK;//�����ظ�����
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
                //Shift������ѹ��input������Token����״̬��������ѭ�������һ��Token
                if(NowAction[0]=="S")
                {
                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                    Stack[top]=input[i];

                    top++;
                    while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                    Stack[top]=NowAction[1];


                    break;
                }

                //Reduce����
                if(NowAction[0]=="R")
                {
                    //ȡ��Reduce���ò���ʽ����
                    string ReduceLeft=NowAction[1];
                    //�����Ϊ�����ķ��еĿ�ʼ���ţ�ֱ��acc���˳�ѭ��
                    if(ReduceLeft==StartSym)
                    {
                        acc=1;
                        return 1;
                    }

                    //ȡ�������г��ˡ�R�����󲿵�ʣ��Token��������ʽ�Ҳ�
                    vector<string>rest;
                    rest.assign(NowAction.begin()+2,NowAction.end());

                    //�����Ҳ�Token����Ӧ��Token����������Ԫ�أ�һ��Token��һ��״̬��
                    for(int i=0;i<rest.size();i++)
                    {
                        if(rest[i]=="E")
                            break;
                        top--;
                        top--;
                    }

                    //ִ��Goto����
                    //ȡ��ջ��״̬
                    int TopState=StringToInt(Stack[top]);

                    //ѹ����Token
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=ReduceLeft;

                    //ѹ����״̬
                    top++;
                    while(Stack.size()<=top)Stack.push_back("");
                    Stack[top]=IntToString(Goto[TopState][ReduceLeft]);
                }


            }
        }
    }

};
int LR1CFG::Item::count=0;

//���������
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

        //��һ����Ŀ����ȣ�����Ŀ��һ�������
        else
            return false;
    }
    return true;
}


/* ��Ҫ�޸������׼���뺯�� */
void read_prog(string& prog)
{
    char c;
    while(scanf("%c",&c)!=EOF){
        prog += c;
    }
}
/* ���������������� */

void Analysis()
{
    string prog;
    read_prog(prog);
    /* ɧ���� �뿪ʼ���ǵı��� */
    /********* Begin *********/
    //���ս��
    vector<string> NonTerminal={"program","stmt","compoundstmt","stmts","ifstmt","whilestmt","assgstmt","boolexpr","boolop","arithexpr","arithexprprime",
     "multexpr","multexprprime","simpleexpr"};
    //�ս��
    //����<= >= �� ==��Ϊһ���ս��������>=��> <=��<��������ʽ
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
    //�趨����������Ŀ
    int ErrorLimit=20;
    LR1CFG ExCFG=LR1CFG(NonTerminal,terminal,ExPro,ErrorLimit);
    ExCFG.CreateByFirst();
    ExCFG.CreateByFollow();
    ExCFG.CreateTable();
    ExCFG.parsing(prog);

    /********* End *********/

}
