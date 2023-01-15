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
   LL1�Ĵ��������Ϊ���֣�
    ����ģʽ����Ԥ�ȴ���ģʽ�����������ķ�������Ԥ�ȴ���õĽ�������ɽ�������չ�Բ�/Ч�ʸ�
    �ǿ���ģʽ/��չģʽ�����Զ�����ģʽ�����������ķ��������㷨�Զ����ɽ�������ɽ�����Ч�ʲ�/��չ��ǿ

    ����ģʽ����ͨ�����������Ͷ�Ӧ�ĸ��Ӳ���
*/

//�ǿ���ģʽ���Զ�����ģʽ����غ����붨��
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
*/
class LLCFG//�ķ�����
{
public:
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

    void CreateTable();

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
    }

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
                if(isTer(NowPro[0]))//���ò���ʽ���ս����ͷ��ֱ��¼��First���ͽ�����
                {
                    if(table[NowNonTerminal][NowPro[0]].size())//��������Ѿ����ڸñ��ֱ�Ӹ��ǣ�������
                        cout<<"�ķ�"<<" "<<NowNonTerminal<<" "<<NowPro[0]<<"���ڳ�ͻ"<<endl;
                    table[NowNonTerminal][NowPro[0]]=NowPro;//¼�������
                    first[NowNonTerminal].insert(NowPro[0]);//¼��First����
                }
                else
                {
                    for(int j=0;j<=NowPro.size();j++)
                    {
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
                                follow[NowNonTerminal].insert(*FindNowNT);
                                break;
                            }
                            for(set<string>::iterator NextNTFirst=first[*FindNowNT].begin();NextNTFirst!=first[*FindNowNT].end();NextNTFirst++)
                            {
                                follow[NowNonTerminal].insert(*NextNTFirst);
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

    void parsing(string prog)
    {
        //��������Ϊtoken
        vector<string>input;//token����
        vector<int>TokenLine;//��ϣ��token�±�-token������
        int line=1;//��ǰ��

        //Ԥ����
        int StartPtr=0;
        for(int i=0;i<prog.length();i++)
        {
            //�������ַ�����¼��ǰ��
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
            cout<<"�﷨����,��"<<line<<"��,ȱ��\""<<'{'<<"\""<<endl;
        }


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

            //token��ǰ��
            TokenLine.push_back(line);
            //��tokenѹ��input
            input.push_back(token);
        }
        //�����ֹ��$
        input.push_back("$");

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
        vector<int>tabs;//ÿ�����token�Ķ�Ӧtab����

        //��¼Stack�����е�token���ʱ��tab����
        vector<int>StackTab;
        StackTab.push_back(0);
        StackTab.push_back(-1);


        //��������
        for(int i=0;i<input.size();i++)//inputʶ��������ң�Stack�Ǵ��������Ӧ
        {

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
                            output.push_back(Stack[top]);
                            tabs.push_back(StackTab[top]+1);
                            output.push_back("E");
                            tabs.push_back(StackTab[top]+2);
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

                            //������������£�����ͨ��������
                            //�ҵ�ջ����������ս������������input�������
                            //��������input[i-1]���λ���Ѿ�����������input[0]��Ԥ����󲻿��ܳ��ִ�������ֱ�ӽ����ս������input[i-1]��������
                            else
                            {
                                int ErrorPtr;
                                for(ErrorPtr=top;ErrorPtr>=0;ErrorPtr--)
                                {
                                        if(isTer(Stack[ErrorPtr]))
                                           break;

                                }
                                cout<<"�﷨����,��"<<TokenLine[i]<<"��,ȱ��\""<<Stack[ErrorPtr]<<"\""<<endl;
                                i--;
                                input[i]=Stack[ErrorPtr];
                                continue;
                            }

                        }

                        //Ӧ�ò���ʽ���������ս����ѹ�����ʽ
                        //���Ƴ���ǰҪ�滻�ķ��ս��
                        int NowTab=StackTab[top];
                        output.push_back(Stack[top]);
                        tabs.push_back(StackTab[top]+1);
                        top--;
                        //��ջ
                        for(int j=production.size()-1;j>=0;j--)//����ʽ������ջ
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                            while(top>StackTab.size()-1)StackTab.push_back(0);
                            Stack[top]=production[j];
                            StackTab[top]=NowTab+1;
                        }
                    }
            }
            //ջ����input���ƥ��
            output.push_back(Stack[top]);//���
            tabs.push_back(StackTab[top]+1);
            top--;//����

            //��������input
        }


        //���
        //ע��output���һ����$���������
        for(int i=0;i<output.size()-1;i++)
        {
            for(int k=0;k<tabs[i];k++)
                cout<<"\t";
            if(i!=output.size()-2)
                cout<<output[i]<<endl;
            else
                cout<<output[i];//���һ�����û�лس�
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
};


//����ģʽ��ض���
//�ķ�����
    //���ս��
    string NonTerminal[15]={"program","stmt","compoundstmt","stmts","ifstmt","whilestmt","assgstmt","boolexpr","boolop","arithexpr","arithexprprime",
     "multexpr","multexprprime","simpleexpr"};

    //�ս��
    //����<= >= �� ==��Ϊһ���ս��������>=��> <=��<��������ʽ
    string terminal[23]={"{","}","if","(",")","then","else","while","ID","=",">","<",">=","<=","==","+","-","*","/","NUM","E",";"};

    //������
    /*
    ��Ϊ���ս������Ϊ�ս����
    �У�program��0��stmt��1��compoundstmt(2) stmts(3) ifstmt(4) whilestmt(5) assgstmt(6) boolexpr(7)
        boolop(8) arithexpr(9) arithexprprime(10) multexpr(11) multexprprime(12) simpleexpr(13)
    �У�{��0�� }��1�� if(2) ((3) )(4) then(5) else(6) while(7) ID(8) =(9) >(10)
        <(11)  >=(12) <=(13) ==(14) +(15) -(16) *(17) /(18) NUM(19) E(20) ;(21)
    */
    string table[15][23];

//������������
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
    table[3][20]="1";//stmts��first������E

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
    table[10][20]="1";//arithexprprime first����E

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
    table[12][20]="1";//multexprprime first����E

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
	/* ɧ���� �뿪ʼ���ǵı��� */
    /********* Begin *********/
    bool fast=0;//����ģʽ��ʶ

    if(!fast)//�ǿ���ģʽ/�Զ�����ģʽ����չ��ǿ��Ч�ʵͣ���Ϊÿ�ζ�Ҫ���ɽ�����
    {
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
        LLCFG ExCFG=LLCFG(NonTerminal,terminal,ExPro);
        ExCFG.CreateByFirst();
        ExCFG.CreateByFollow();
        ExCFG.parsing(prog);
    }
    else//����ģʽ��ֱ������Ԥ�ȴ���õ��ķ����н���������������չ��
    {
        //�ķ�����ݹ顢���������ӣ�ֱ�ӹ��������
        //���⣬���ݸ��ķ�����Ľ�����û�г�ͻ��ķ���LL1�ķ�
        GetFirst();
        GetFollow();

        //��������Ϊtoken
        vector<string>input;//token����
        vector<int>TokenLine;//��ϣ��token�±�-token������
        int line=1;//��ǰ��

        //Ԥ����
        int StartPtr=0;
        for(int i=0;i<prog.length();i++)
        {
            //�������ַ�����¼��ǰ��
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
            cout<<"�﷨����,��"<<line<<"��,ȱ��\""<<'{'<<"\""<<endl;
        }


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

            //token��ǰ��
            TokenLine.push_back(line);
            //��tokenѹ��input
            input.push_back(token);
        }
        //�����ֹ��$
        input.push_back("$");


        //��������

        //ջ����
        vector<string>Stack;//����ģ��ջ
        int top=0;//ջ��
        Stack.push_back("$");//ѹ��$
        Stack.push_back("program");//ѹ�뿪ʼ����
        top++;

        //�������
        vector<string>output;//�������
        vector<int>tabs;//ÿ�����token�Ķ�Ӧtab����

        //��¼Stack�����е�token���ʱ��tab����
        vector<int>StackTab;
        StackTab.push_back(0);
        StackTab.push_back(-1);

        for(int i=0;i<input.size();i++)//inputʶ��������ң�Stack�Ǵ��������Ӧ
        {

            //��Ҫ��Stack���б仯����
            while(Stack[top]!=input[i])
            {
                int StackTopIndex=-1;//Stackջ�����ս����Ӧ���±�,ʵ������Ϊ������֪�����ϵ�NonTerminal
                int InputLeftIndex=-1;//Input��ǰ����δ���������ս����Ӧ���±�,ʵ������Ϊ������֪�����ϵ�Terminal

                //ƥ��StackTopIndex
                for(int j=0;j<14;j++)
                {
                    if(Stack[top]==NonTerminal[j])
                    {
                        StackTopIndex=j;
                        break;
                    }
                }

                    //ƥ��InputLeftIndex
                    for(int j=0;j<22;j++)
                    {
                        if(input[i]==terminal[j])
                            InputLeftIndex=j;
                    }
                    //��Ϊ�ղ���ʽ������ջ��������ղ���ʽ
                    //ע�⣬�ղ���ʽ�ǡ�E���������ǡ�����������ƥ�����
                    if(table[StackTopIndex][InputLeftIndex]=="E")
                        {
                            output.push_back(Stack[top]);
                            tabs.push_back(StackTab[top]+1);
                            output.push_back("E");
                            tabs.push_back(StackTab[top]+2);
                            top--;
                        }
                    //����
                    else
                    {
                        string production=table[StackTopIndex][InputLeftIndex];//���Ҳ���ʽ
                        string temp="";
                        vector<string>ProTokens;//��������ʽΪToken
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

                        //productionΪ�գ�ƥ�����������
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
                            cout<<"�﷨����,��"<<TokenLine[i]<<"��,ȱ��\""<<Stack[ErrorPtr]<<"\""<<endl;
                            i--;
                            input[i]=Stack[ErrorPtr];
                            continue;
                        }

                        //Ӧ�ò���ʽ���������ս����ѹ�����ʽ
                        //���Ƴ���ǰҪ�滻�ķ��ս��
                        int NowTab=StackTab[top];
                        output.push_back(Stack[top]);
                        tabs.push_back(StackTab[top]+1);
                        top--;
                        //��ջ
                        for(int j=ProTokens.size()-1;j>=0;j--)//����ʽ������ջ
                        {
                            top++;
                            while(top>Stack.size()-1)Stack.push_back("");//�����ջ��vector�������±���
                            while(top>StackTab.size()-1)StackTab.push_back(0);
                            Stack[top]=ProTokens[j];
                            StackTab[top]=NowTab+1;
                        }
                    }
            }
            //ջ����input���ƥ��
            output.push_back(Stack[top]);//���
            tabs.push_back(StackTab[top]+1);
            top--;//����

            //��������input
        }


        //���
        //ע��output���һ����$���������
        for(int i=0;i<output.size()-1;i++)
        {
            for(int k=0;k<tabs[i];k++)
                cout<<"\t";
            if(i!=output.size()-2)
                cout<<output[i]<<endl;
            else
                cout<<output[i];//���һ�����û�лس�
        }

    }
    /********* End *********/

}
