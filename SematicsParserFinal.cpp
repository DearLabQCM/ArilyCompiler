// C����LL1������
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
/* ��Ҫ�޸������׼���뺯�� */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
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


void PrintPro(vector<string> pro)//�������ʽ
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
    ��������ķ������У������¸��
        Token����Ϊvector<string>���͡�Ϊһ���ַ������ո�ָ���ɵ�tokens����������StringToTokens�������ɡ�
        ���ս����һ��string��
        �ս����һ��string��
        ����ʽ��һ��Token���������ʽA->if whileΪ["if"��"while"]
        �����ַ�����"E"������ղ���ʽ��
        �մ���""�����ڴ�����

        ���ս�����ϣ�NonTerminal����һ��vector<string>������һ��CFG���еķ��ս����
        �ս�����ϣ�terminal����һ��vector<string>������һ��CFG���е��ս����
        ����ʽ���ϣ�productions����һ��map�����Ϊ���ս����ֵΪ����Ӧ�����в���ʽ�ļ��ϣ�Ϊvector<vector<string>>��
        ��ʼ���ս����StartSym������һ������ʽ����ˡ�
        ������table����һ��˫��map���������ָ��������С��С��м�Ϊ���ս�����м�Ϊ�ս����ֵΪ��Ӧ�Ĳ���ʽ��vector<string>������LL1�ķ��У�ÿ���С��м���ֻ����һ��ֵ��
        ������ʽ�ж�ӳ�䣨SinglePro����һ��map�������ж�һ�����ս���Ƿ�ֻ��һ������ʽ�������ڴ�������Ϊ���ս����ֵΪ��Ӧ�ĵ�һ����ʽ��������ս����ֻ��һ������ʽ��ֵΪ�մ���
        �մ��ж�ӳ�䣨Nullable����һ��map���ж�һ���ս���ɷ�תΪ�մ���



        ����������У�������������ݽṹ��
        �����ּ��ϣ�keywords����һ��vector<string>������һ��CFG���еı����֡�
        ������ϣ�operators����һ��vector<string>������һ��CFG���е������
        �����ԭ�����루input/RealInput����vector<string>��Ϊ�������������ɵ�Token��������RealInputΪԭʼ����Token����input��Ա�ʶ������ΪID�����ִ���ΪINTNUM/REALNUM��
        Token��λ����TokenLine����vector<int>����λ�±��Ӧ��Token�ڴ����ж�Ӧ���С�
        �����ʶ��isError�����ж�����������Ƿ���ִ���
        ������Ϣ��ErrorOutput��������ʱ�������
*/
class LLCFG//�ķ�����
{
public:
    //����
    class Attributes
    {
        private:
        string type;//�洢����
        double value;//��ֵ
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


    vector<string>NonTerminal;//���ս������
    vector<string>terminal;//�ս������
    string StartSym;//��ʼ����
    map<string,vector<vector<string> > >productions;//����ʽ,ÿ������ʽ��Ӧһ������ֹ��->Token������vector<string>����ע��һ������ֹ�������ж�������ʽ��������vector<vector<string>>
    map<string,map<string,vector<string> > >table;//������
    map<string,vector<string> >SinglePro;//������ʽ����������
    map<string,bool>Nullable;//�ж�һ�����ս���Ƿ���Ա�Ϊ�մ�

    map<string,set<string> >first;//First��
    map<string,set<string> >follow;//Follow��
    map<string,bool>Visit;//������ǣ������������������Ϊ1��ʾ�Ѿ��������ˣ������ظ�����

    map<string,Attributes>SymTab;//���ű��洢�Ѷ����ʶ��������
    vector<string>keywords;//�洢������
    vector<string>operators;//�洢������
    vector<string>input;//���ڴ�Ŵ���������루����ʶ������ΪID�����ִ���ΪINTNUM/REALNUM���и�ɵ�Token��
    vector<int>TokenLine;//Token��Ӧ����
    vector<string>RealInput;//���ڴ����ʵ�����и�ɵ�Token��
    map<string,vector<vector<string> > >EmbeddedProductions;
    vector<string>ActionStack;

    bool isError;
    vector<string>ErrorOutput;




    //�ʷ�����
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

    //��ʼ�������ֺ������
    void SaveWords(vector<string>_keywords,vector<string>_operators)
    {
        //�����账��Ĺؼ���
        keywords=_keywords;
        //���������
        operators=_operators;
    }


    //�ʷ�����

    //��������ΪToken��
    void LexicalAnalysis(string prog)
    {
        int line=1;
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
                    token+=prog[i++];
                //token��ǰ��
                TokenLine.push_back(line);

                //input�������������룬RealInput������ʵ����
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

    //��ʼ�����ű�
    void init()
    {
        //�������Ա�
        for(int i=0;i<RealInput.size();i++)
        {
            //token�Ǳ�ʶ��
            if(isAlpha(RealInput[i])&&!(isKeyWords(RealInput[i])))
            {
                Attributes TempAttr;
                TempAttr.SetAttributes("void",0);
                SymTab[RealInput[i]]=TempAttr;
            }
        }
    }


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

    /*
        ��ʼ����Ҫ���룺
            1�����ս������
            2���ս������
            3������ĵ�������ʽ����string���ڳ�ʼ�������ս���ΪToken������vector<string>�����Բ���_productionsֵΪvector<string>
               �ַ������ַ���ӳ��Ĳ���ʽ����
               �����Ὣ����ʽ�����е�ֵ��ΪToken���飬������
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


    //�﷨����
    /*
        ����First����������First���������
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


                int ProStart=0;
                if(NowPro[0][0]=='M')ProStart=1;
                if(isTer(NowPro[ProStart]))//���ò���ʽ���ս����ͷ��ֱ��¼��First���ͽ�����
                {
                    if(table[NowNonTerminal][NowPro[ProStart]].size())//��������Ѿ����ڸñ��ֱ�Ӹ��ǣ�������
                        cout<<"�ķ�"<<" "<<NowNonTerminal<<" "<<NowPro[ProStart]<<"���ڳ�ͻ"<<endl;
                    table[NowNonTerminal][NowPro[ProStart]]=NowPro;//¼�������
                    first[NowNonTerminal].insert(NowPro[ProStart]);//¼��First����
                }
                else
                {
                    for(int j=ProStart;j<=NowPro.size();j++)
                    {
                        if(NowPro[j][0]=='M')continue;
                        if(j==NowPro.size())//��������ĩβ��˵����������ʽ����Ϊ�գ���ֵNullable
                        {
                            Nullable[NowNonTerminal]=1;
                            break;
                        }

                        if(NowPro[j]=="E")
                        {
                            Nullable[NowNonTerminal]=1;
                            continue;
                        }
                        if(isTer(NowPro[j]))//�����ս������ֵ���˳�
                        {
                            if(table[NowNonTerminal][NowPro[j]].size())//��������Ѿ����ڸñ��ֱ�Ӹ��ǣ�������
                            cout<<"�ķ�"<<" "<<NowNonTerminal<<" "<<NowPro[j]<<"���ڳ�ͻ"<<endl;
                            table[NowNonTerminal][NowPro[j]]=NowPro;//¼�������
                            first[NowNonTerminal].insert(NowPro[j]);//¼��First����
                            break;
                        }

                        dfs(NowPro[j]);//�������ս�����ݹ�������First����
                        for(set<string>::iterator it=first[NowPro[j]].begin();it!=first[NowPro[j]].end();it++)//�Ը÷��ս����First�����е�ÿ��Ԫ�أ�¼������i�����ķ��ս����table�к�First����
                        {
                            if(table[NowNonTerminal][*it].size())
                                {
                                    cout<<NowPro[j]<<endl;
                                    cout<<"�ķ�"<<" "<<NowNonTerminal<<" "<<*it<<"���ڳ�ͻ"<<endl;
                                }
                            table[NowNonTerminal][*it]=NowPro;
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
        ����Follow����������First���������
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

        //�����п���Ϊ�յ�NonTerminal���ڽ�����Ķ�Ӧfollow�����롰E��
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




    //������1 ���ʹ���
    void TypeError(int ErrorLine)
    {
          isError=true;
          string ErrorMsg="error message:line " + IntToString(ErrorLine) + ",realnum can not be translated into int type";
          ErrorOutput.push_back(ErrorMsg);
    }
    //������2 ��0��
    void DividedByZeroError(int ErrorLine)
    {
        isError=true;
        string ErrorMsg="error message:line "+ IntToString(ErrorLine) + ",division by zero";
        ErrorOutput.push_back(ErrorMsg);

    }

    //�Ƚ���������ȼ�,ǰ�ߴ��ں���Ϊtrue������Ϊfalse
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
            //��¼���� pos
			string type=RealInput[Begin];
			//pos+1 ��ʶ��
			string id=RealInput[Begin+1];
			//pos+2 ��ֵ����
			string opr=RealInput[Begin+2];
			//pos+3 ��ֵ
			string val=RealInput[Begin+3];

			if(opr=="=")
            {
                //��ֵ����
                Attributes attr=SymTab[id];
                //����i�����Ժ�ֵ
                attr.SetType(type);
                attr.SetVal(StringToDouble(val));
                SymTab[id]=attr;
                //���ʹ���
                if(type=="int"&&val.find(".")!=string::npos)
                    TypeError(TokenLine[Begin]);
                //pos+4 �ֺ�
                string semi=RealInput[Begin+4];
            }
    }

    //�������ʽ�ļ���
    bool ComputeBoolExpr(int Begin,int End)
    {
        //Ĭ�ϲ�ȡ�򵥵���׺���ʽ �� a opr b
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

    //�������ʽ�ļ���
    void ComputeArithExpr(int Begin,int End)
    {
        //�洢��׺���ʽ
        vector<string> PostfixExpr;
        //�洢�������
        stack<string> OpStack;
        //Begin+1��Ϊ=
        string id=RealInput[Begin];
        //�洢λ��
        stack<int>OpLine;
        vector<int>ExprLine;

        //��������Begin+2��ʼ
        for(int j=Begin+2;j<End;j++)
        {
            string NowToken=RealInput[j];
            //����ǲ�����
            if(isOpr(NowToken))
            {
                if(NowToken=="(")//������
                {
                    OpStack.push(RealInput[j]);
                    OpLine.push(TokenLine[j]);
                }
                else
                {
                    //���������ţ�������֮��������ȫ��pop�������׺���ʽ
                     if(NowToken==")")
                     {
                          while(OpStack.top()!="(")
                          {
                                PostfixExpr.push_back(OpStack.top());
                                ExprLine.push_back(OpLine.top());
                                OpStack.pop();
                                OpLine.pop();
                          }
                          //pop������
                          OpStack.pop();
                          OpLine.pop();
                     }
                     else
                     {
                         //����������������
                         /*
                             �Ƚ�ջ���͵�ǰ��������ȼ���
                             ���ջ����������ջ ����popֱ��ջ�����ȼ�С�ڵ�ǰ���ȼ�
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
                //��������ջ
                PostfixExpr.push_back(RealInput[j]);
                ExprLine.push_back(TokenLine[j]);
            }
        }

        //��������׺���ʽ���������ջ�Բ�Ϊ�գ���ȫ�������׺���ʽ
        while(!OpStack.empty())
        {
            PostfixExpr.push_back(OpStack.top());
            ExprLine.push_back(OpLine.top());
            OpStack.pop();
            OpLine.pop();
        }

        //��׺���ʽ����
        //�洢������
        stack<double> NumStack;
        for(int i=0;i<PostfixExpr.size();i++)
        {
            string s=PostfixExpr[i];
            double res=0;
            if(isOpr(s))
            {
                //��������������NumStack pop����Ԫ�ؽ��м���
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
                //�������������ֱ����ջ,���������ܲ������Ա���,��Ҫ���������ж�
                if(SymTab.find(s)==SymTab.end())
                    NumStack.push(StringToDouble(s));
                else
                    NumStack.push(SymTab[s].GetVal());
            }
        }
        //������
        SymTab[id].SetVal(NumStack.top());
    }
    void ExecIf(int Begin,int End)
    {
                    int ThenIndex=Begin+1;//��λthen
                    while(ThenIndex<RealInput.size()&&RealInput[ThenIndex]!="then")
                        ThenIndex++;

                    int ElseIndex=ThenIndex+1;//��λelse
                    while(ElseIndex<RealInput.size()&&RealInput[ElseIndex]!="else")
                        ElseIndex++;
                    int SemiIndex=ElseIndex+1;//��λ�ֺ�
                    while(SemiIndex<RealInput.size()&&RealInput[SemiIndex]!=";")
                        SemiIndex++;

                    //���㲼�����ʽ���
                    bool res=ComputeBoolExpr(Begin+2,ThenIndex-2);
                    if(res)//����Ϊ�� ִ��then
                        ComputeArithExpr(ThenIndex+1,ElseIndex-1);
                    else//����Ϊ�� ִ��else
                        ComputeArithExpr(ElseIndex+1,SemiIndex);
    }


    void parsing(string prog)
    {
        LexicalAnalysis(prog);

        //��������

        //ջ����
        //�������vector����ģ��ջ
        vector<string>Stack;//����ģ��ջ
        int top=0;//ջ��
        Stack.push_back("$");//ѹ��$
        Stack.push_back(StartSym);//ѹ�뿪ʼ����
        top++;

        //�������
        vector<string>output;//�������

        vector<string>OutLeft;
        vector<vector<string> >OutRight;




        //��������
        for(int i=0;i<input.size();i++)//inputʶ��������ң�Stack�Ǵ��������Ӧ
        {
            while(Stack[top][0]=='M')
            {
                ActionStack.push_back(Stack[top]);
                int ActionCode=Stack[top][1]-'0';
                if(ActionCode%2)ActionStack.push_back(IntToString(i-1));
                else ActionStack.push_back(IntToString(i));
                top--;

            }
            //��Ҫ��Stack���б仯����
            while(Stack[top]!=input[i])
            {
                string StackTopIndex=Stack[top];//Stackջ��token
                string InputLeftIndex=input[i];//Input��ǰ����token

                    vector<string> production=table[StackTopIndex][InputLeftIndex];//���Ҳ���ʽ
                    //��Ϊ�ղ���ʽ������ջ��������ղ���ʽ
                    //ע�⣬�ղ���ʽ�ǡ�E����������""��
                    if(production.size()&&(production[0]=="E"))
                        {
                            OutLeft.push_back(Stack[top]);
                            OutRight.push_back({"E"});
                            top--;
                        }
                    //����
                    else
                    {
                        //productionΪ�գ�ƥ�����������
                        if(production.size()==0)
                        {
                            int ErrorCode=perror(Stack[top]);
                            //��������-1 -2�£�ջ��Ϊ��һ����ʽ���ս����ֱ��Ӧ�ò���ʽ
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

                            //������������£�����ͨ��������
                            //�ҵ�ջ����������ս������������input�������
                            else
                            {
                                int ErrorPtr;
                                for(ErrorPtr=top;ErrorPtr>=0;ErrorPtr--)
                                {
                                        if(isTer(Stack[ErrorPtr])&&Stack[ErrorPtr][0]!='M')
                                           break;

                                }
                                //���������ڴʷ������о��Ѿ��ҳ���REALNUM��INTNUM��������﷨������ʵ���Ѿ��ҳ��˸�ֵ�е����ʹ�������˸������޸�INTNUM��REALNUM�����������ˣ�
                                //����������ѡ�������ʹ�øô�����������������嶯��ͬ��
                                //���⣬����Ĵ��������в�ͬ����Ϊ�����input�������������Ҫʹ�ã����Բ��ܸ���ԭ������
                                if(input[i].find("NUM")!=string::npos &&Stack[ErrorPtr].find("NUM")!=string::npos)
                                    input[i]=Stack[ErrorPtr];
                                else
                                    input.insert(input.begin()+i,Stack[ErrorPtr]);
                                continue;
                            }

                        }

                        top--;
                        //��ջ
                        for(int j=production.size()-1;j>=0;j--)//����ʽ������ջ
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
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
            top--;//����

            //��������input
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
        if(isError)//�������
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

    //������
    int perror(string StackTop)
    {
        //��������0��ջ��Ϊ�ս��������input��ƥ��
        if(isTer(StackTop))
            return 0;
        if(SinglePro[StackTop].size())
        {
            //��������-1��ջ��Ϊ��һ����ʽ���ս�����Ҳ���ʽΪ��
            if(productions[StackTop][0][0]=="E")
                return -1;
            //��������-2��ջ��Ϊ��һ����ʽ���ս�����Ҳ���ʽ��Ϊ��
            return -2;
        }
        //��������
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

        //��һ��token����ȣ���Token��һ�������
        else
            return false;
    }
    return true;
}

//�������
void Analysis()
{
    string prog;
    read_prog(prog);
    /* ɧ���� �뿪ʼ���ǵı��� */
    /********* Begin *********/
    //����M��ͷ��token���������������parsing��ʱ�򱻺���
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

    //���嶯��Ƕ��
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








