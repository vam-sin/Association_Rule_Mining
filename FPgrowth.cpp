#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>
#include <set>
#include <string>
#define MAX 23000
using namespace std;

int nodes=0;

class Rule
{
public:
  vector<string> L,R;
  int S1,S2;
  float C;
  void Display();
};

class Node
{
public:
  string name;
  int support;
  vector<Node*> next;
  int sno;
  Node()
  {
    name = "";
    support = 0;
    next.clear();
    sno = nodes++;
  }
};

Node * FPtree = new Node;
vector<Node> Kemoy;

vector<vector<vector<string>>> Cand;
vector<vector<string>> ClosedItemset;
vector<vector<string>> MaximalItemset;

vector<vector<string>> Transactions,CBP,Freq,final,Freq_Transactions;
vector<Rule> ARules;
vector<string> Items,FreqItems;
vector<int> CBPsup,finalsup,sing_count;
int vis[MAX];
int minsupport=700;
float minconf=0.25;
vector<Rule> ARnongen;

// displays a id vector of strings
void display1D(vector<string> S)
{
  for(int i=0; i<S.size(); i++)
  {
    cout<<S[i]<<", ";
  }
  cout<<"\b\b";
}

// displays the rule object
void Rule::Display()
{
  display1D(L);
  cout<<" ("<<S1<<") ---> ";
  display1D(R);
  cout<<" ("<<S2<<") - conf("<<C<<")"<<endl;
}

// displays the vector of rules
void Display_Rules(vector<Rule> AR)
{
  for(int i=0;i<AR.size();i++)
  {
    AR[i].Display();
  }
}

// Reads the CSV file and puts it into 2D vector Transactions and generates
// unique Items
void read_csv(string file_name)
{
  fstream fin;

  vector<string> temp;
  fin.open(file_name, ios::in);
  string line, word;
  int i=0;
  while(!fin.eof())
  {
    temp.clear();
    getline(fin, line);
    //breaks down the line into words
    stringstream s(line);
    while (getline(s, word, ','))
    {
      temp.push_back(word);
      Items.push_back(word);
    }
    sort(temp.begin(), temp.end());
    //append transaction into transactions
    Transactions.push_back(temp);
    i++;
  }
  sort(Items.begin(), Items.end());
  Items.erase(unique(Items.begin(), Items.end()), Items.end());

}

// Gets the support of given Itemset
int Support(vector<string> Itemset)
{
  int count=0;
  for(int i=0; i<Transactions.size(); i++)
  {
    int c=0;
    for(int k=0; k<Itemset.size(); k++)
    {
      for(int j=0; j<Transactions[i].size(); j++)
      {
        if(Itemset[k]==Transactions[i][j])
          c++;
      }
    }
    if(c==Itemset.size())
      count++;
  }
  return count;
}

void Prune()
{
  int i;
  for(i=0; i<Kemoy.size(); i++)
  {
    vector<string> temp;
    temp.push_back(Kemoy[i].name);
    if(Support(temp)<minsupport)
    {
      break;
    }
    FreqItems.push_back(Kemoy[i].name);
  }
}

void sort()
{
  int i;
  for(i=0;i<Kemoy.size();i++)
  {
    for(int j=0;j<Kemoy.size()-i-1;j++)
    {
      if(Kemoy[j+1].support>Kemoy[j].support)
      {
        Node temp = Kemoy[j];
        Kemoy[j] = Kemoy[j+1];
        Kemoy[j+1] = temp;
      }
    }
  }
}

// inserts nodes to the fp_tree
void Insert(vector<string> row, int index, vector<Node*> &head, Node* add)
{
  int flag=0;
  if(head.size()!=0)
  {
    for(int i=0; i<head.size(); i++)
    {
      if(row[index]==head[i]->name)
      {
        flag=1;
        head[i]->support++;
        if(index+1<row.size())
        {
          Insert(row, index+1, head[i]->next, head[i]);
        }
      }
    }
  }
  if(!flag)
  {
    Node * temp = new Node;
    temp->name = row[index];
    temp->support = 1;
    temp->next.clear();
    // if(retHead(temp->name)==-1)
    // {
    //   HT(temp);
    // }
    // else
    // {
    //   while()
    // }
    if(index+1<row.size())
    {
      Insert(row, index+1, temp->next, temp);
    }

    head.push_back(temp);
  }
}

// creates frequent transaction vector and inserts frequent transactions row
// by row
void CreateTree()
{
  vector<string> ordered_transaction;
  ordered_transaction.clear();
  for(int i=0;i<Transactions.size();i++)  //each transaction we're going thru
  {
      for(int j=0;j<FreqItems.size();j++)   //each line checking with kemoy in same order
      {
        for(int k=0;k<Transactions[i].size();k++)
        {
          if(FreqItems[j]==Transactions[i][k])
          {
            ordered_transaction.push_back(FreqItems[j]);
          }
        }
      }
      if(ordered_transaction.size()!=0)
        Freq_Transactions.push_back(ordered_transaction);
      ordered_transaction.clear();
  }
  for(int i=0; i<Freq_Transactions.size(); i++)
    Insert(Freq_Transactions[i], 0, FPtree->next, FPtree);
}

// displays the fp tree
void DisplayTree(Node * head, vector<string> Itemset)
{
  Itemset.push_back(head->name);
  if(head->next.size()==0)
  {
    for(int j=1; j<Itemset.size(); j++)
    {
      cout<<Itemset[j]<<", ";
    }
    cout<<"\b\b"<<head->support<<endl;
  }

  for(int i=0; i<head->next.size(); i++)
  {
    DisplayTree(head->next[i], Itemset);
  }
}

// checks if s is a subset of a
int present_in_vec(vector<string> a,vector<string> S)
{
   // present
  for(int i=0;i<S.size();i++)
  {
    int flag=0; // not present
    for(int j=0;j<a.size();j++)
    {
      if(S[i]==a[j])
      {
        flag=1;
      }
    }
    if(flag==0)
      return 0;
  }
  return 1;
}

// gets support of s from S
int getsupport(vector<vector<string>> S, vector<string> s)
{
  int count=0;
  int length=S.size();
  for(int i=0;i<length;i++)
  {
    if(present_in_vec(S[i],s))
    {
      count+=CBPsup[i];
    }
  }
  return count;
}

// removes duplicate strings
vector<string> rem_dup(vector<string> S)
{
  sort(S.begin(), S.end());
  S.erase(unique(S.begin(), S.end()), S.end());
  return S;
}

// removes duplicate vector of strings
vector<vector<string>> rem_dup2D(vector<vector<string>> S)
{
  sort(S.begin(), S.end());
  S.erase(unique(S.begin(), S.end()), S.end());
  return S;
}

// lexicographically sorts a vector of string
vector<string> lex_sort(vector<string> row)
{
  for(int i=0;i<row.size();i++)
  {
    for(int j=i+1;j<row.size();j++)
    {
      if(row[i]>row[j])
      {
        string temp=row[i];
        row[i]=row[j];
        row[j]=temp;
      }
    }
  }
  return row;
}

// checks if row2[0] is present in row1
int check_string(vector<string> row1, vector<string> row2)
{

  int flag=0; // Not present
  for(int i=0;i<row1.size();i++)
  {
    if(row1[i]==row2[0])
    {
      flag=1;
    }
  }
  return flag;
}

// gets product of S1 and S2
vector<vector<string>> product(vector<vector<string>> S1, vector<vector<string>> S2)
{
  vector<vector<string>> P;
  for(int i=0;i<S1.size();i++)
  {
    for(int j=i;j<S2.size();j++)
    {
      vector<string> row;
      for(int k=0;k<S1[i].size();k++)
      {
        row.push_back(S1[i][k]);
      }
      if(!check_string(row,S2[j]))
      {
        row.push_back(S2[j][0]);
        row=lex_sort(row);
        P.push_back(row);
      }
    }
  }
  return P;
}

// prunes S according to minsupport
vector<vector<string>> prune(vector<vector<string>> S)
{
  vector<vector<string>> Q;
  for(int i=0;i<S.size();i++)
  {
    vector<string> temp;
    for(int j=0;j<S[i].size();j++)
      temp.push_back(S[i][j]);
    int supportcount=getsupport(CBP,temp);
    if(supportcount>=minsupport)
    {
      Q.push_back(temp);
    }
  }
  return Q;
}

//displays a 2d frequent itemset with support
void display2D_Freq(vector<vector<string>> S)
{
  for(int i=0;i<S.size();i++)
  {
    for(int j=0;j<S[i].size();j++)
    {
      cout << S[i][j] ;
    }
    cout << " " << finalsup[i] << endl;
  }
}

// traverses the tree depth wise to find S and forms CBPs
void dfs(Node * root, string S, vector<string> path, int t)
{
  // to get all the conditional base pairs
  // cout << "Hi" << t <<root->name <<endl;
  if(root->name==S)
  {
    sing_count[t]+=root->support;
    CBPsup.push_back(root->support);
    CBP.push_back(path);
  }
  if(root!=NULL && root->name!=S)
  {
    path.push_back(root->name);
    vis[root->sno]=1;
    for(int i=0;i<root->next.size();i++)
      if(vis[root->next[i]->sno]!=1)
      {
        dfs(root->next[i],S,path,t);
      }
  }
}

// generates Frequent itemsets
void FPgen()
{
  reverse(FreqItems.begin(),FreqItems.end());
  memset(vis,0,4*(MAX));
  for(int t=0;t<FreqItems.size();t++)
  {
    vector<string> Freqtemp;
    vector<vector<vector<string>>> C;
    sing_count.push_back(0);
    vector<string> path;
    string l=FreqItems[t];
    // Makes Conditional Base Pairs
    for(int i=0; i<FPtree->next.size(); i++)
    {
      dfs(FPtree->next[i],l,path,t);
      // dfs(FPtree->next[i], l, path, t);
    }
    memset(vis,0,4*(MAX));
    // display2D(CBP);
    // Creating Conditional FP Tree
    for(int i=0;i<CBP.size();i++)
    {
      for(int j=0;j<CBP[i].size();j++)
      {
        vector<string> ty;
        ty.push_back(CBP[i][j]);
        if(getsupport(CBP,ty)>=minsupport)
        {
          Freqtemp.push_back(CBP[i][j]);
        }
      }
    }

    Freqtemp=rem_dup(Freqtemp);
    // display1D(Freqtemp);
    for(int i=0;i<Freqtemp.size();i++)
    {
      vector<string> temp;
      temp.push_back(Freqtemp[i]);
      Freq.push_back(temp);
    }
    C.push_back(Freq);
    int max=0;
    for(int i=0;i<CBP.size();i++)
    {
      if(max<CBP[i].size())
        max=CBP[i].size();
    }
    // Finds all permutations of single freq items in CBP
    for(int i=1;i<max;i++)
    {
      C.push_back(prune(rem_dup2D(product(C[i-1],C[0]))));
      for(int h=0;h<C[i].size();h++)
        Freq.push_back(C[i][h]);
      // cout << "C" << i+1 << endl;
    //  display2D(C[i]);
    }
    for(int i=0;i<Freq.size();i++)
    {
      finalsup.push_back(getsupport(CBP,Freq[i]));
      Freq[i].push_back(l);
      final.push_back(Freq[i]);
    }

    // display2D(Freq);
    CBP.clear();
    CBPsup.clear();
    Freq.clear();
  }

  for(int i=0;i<FreqItems.size();i++)
  {
    vector<string> temp;
    temp.push_back(FreqItems[i]);
    final.push_back(temp);
    finalsup.push_back(sing_count[i]);
  }
}

// checks if a is present in S
int present_in_vec2(vector<string> S,string a)
{
  int flag=0; // not present
  for(int i=0;i<S.size();i++)
  {
    if(S[i]==a)
    flag=1;
  }
  return flag;
}

// returns index of s in final
int index_in_final(vector<string> s)
{
  for(int i=0;i<final.size();i++)
  {
    int count=0;

    for(int j=0; j<s.size(); j++)
    {
      if(present_in_vec2(final[i], s[j]))
        count++;
    }
    if(count==s.size() && s.size()==final[i].size())
    {
      return i;
    }
  }
}

// returns confidence of X->Y
float getConf(vector<string> X,vector<string> Y)
{
  vector<string> S;
  for(int i=0;i<X.size();i++)
  S.push_back(X[i]);
  for(int i=0;i<Y.size();i++)
  S.push_back(Y[i]);
  float val=(float)finalsup[index_in_final(S)]/finalsup[index_in_final(X)];
  return val;
}

// generates association rules w.r.t to minconf
void AssociationRulesGen()
{
  int rulecount=0;
  // looping through all the C's
  for(int t=0;t<final.size();t++)
  {
      for(int k=1;k<final[t].size();k++)
      {
        // for 3 itemset you run this twice
        for(int i=0;i<final[t].size();i++)
        {
          vector<string> X,Y;
          Rule temp;
          for(int j=0;j<k;j++)
          {
            if((i+j)>(final[t].size()-1))
            X.push_back(final[t][i+j-final[t].size()]);
            else
            X.push_back(final[t][j+i]);
          }
          for(int j=0;j<final[t].size();j++)
          {
            if(!present_in_vec2(X,final[t][j]))
            {
              Y.push_back(final[t][j]);
            }
          }
          float conf=getConf(X,Y);

            if(conf>=minconf)
            {
              sort(X.begin(),X.end());
              temp.L=X;
              temp.R=Y;
              temp.S1 = finalsup[index_in_final(X)];
              temp.S2 = finalsup[index_in_final(Y)];
              temp.C = conf;
              ARules.push_back(temp);
            }
            else
            {
              sort(X.begin(),X.end());
              temp.L=X;
              temp.R=Y;
              temp.S1 = finalsup[index_in_final(X)];
              temp.S2 = finalsup[index_in_final(Y)];
              temp.C = conf;
              ARnongen.push_back(temp);
            }

          X.clear();
          Y.clear();
        }
      }
  }
}

// creates a 3d vector of candidates
void Candidate()
{
  for(int i=0;i<FreqItems.size();i++)
  {
    vector<vector<string>> tmp;
    tmp.clear();
    for(int j=0;j<final.size();j++)
    {
      int length = final[j].size();
      if(length==i+1)
      {
        tmp.push_back(final[j]);
      }
    }
    Cand.push_back(tmp);
  }
}

//finds closed frequent itemsets
void closed_itemsets()
{
  for(int i=0; i<Cand.size()-1; i++)
  {
    for(int j=0; j<Cand[i].size(); j++)
    {
      int c=0;
      for(int k=0; k<Cand[i+1].size(); k++)
      {
        if(present_in_vec(Cand[i+1][k], Cand[i][j]))
        {
          if(Support(Cand[i][j])==Support(Cand[i+1][k]))
            c++;
        }
      }
      if(c==0)
      {
        ClosedItemset.push_back(Cand[i][j]);
      }
    }
  }
}

// gets maximal frequent itemsets
void max_itemsets()
{
  for(int i=0; i<Cand.size()-1; i++)
  {
    for(int j=0; j<Cand[i].size(); j++)
    {
      int c=0;
      for(int k=0; k<Cand[i+1].size(); k++)
      {
        if(present_in_vec(Cand[i+1][k], Cand[i][j]))
        {
          c++;
        }
      }
      if(c==0)
      {
        MaximalItemset.push_back(Cand[i][j]);
      }
    }
  }
}

// displays a 2d vector of strings
void display_itemset(vector<vector<string>> Itemset)
{
  for(int i=0; i<Itemset.size(); i++)
  {
    for(int j=0; j<Itemset[i].size(); j++)
      cout<<Itemset[i][j]<<",";

    cout<<"\b ("<<Support(Itemset[i])<<")";
    cout<<endl;
  }
  cout<<Itemset.size();
}


int main()
{
  read_csv("groceries.csv");
  cout<<"Enter minimum support count: ";
  cin>>minsupport;
  cout<<"\n\nEnter minimum confidence: ";
  cin>>minconf;
  cout<<endl;
  for(int i=0; i<Items.size(); i++)
  {
    vector<string> t;
    t.push_back(Items[i]);
    if(Support(t)>=minsupport)
    {
      Node temp;
      temp.support = Support(t);
      temp.name = Items[i];
      Kemoy.push_back(temp);
    }
  }
  // Step 1: Order items in descending order of Support
  cout<<"Sorting...";
  sort();
  // Prune items below minsupport
  cout<<"\nPruning";
  Prune();

  //display1D(FreqItems);
  //cout<<FreqItems.size();
  cout<<"\nCreating Tree\n";
  CreateTree();

  // vector<string> Itemset;
  // Itemset.clear();
  // cout<<"Displaying Tree\n";
  // DisplayTree(FPtree, Itemset);
  // cout << nodes;

  // Generate Freq itemsets using Conditional FP Tree
  FPgen();
  cout << "\n\nFrequent Itemsets\n" << endl;
  display2D_Freq(final);
  cout<<final.size()<<endl;

  Candidate();

  cout<<"closed frequent itemsets...\n";
  closed_itemsets();
  display_itemset(ClosedItemset);

  cout<<"maximal frequent itemsets...\n";
  max_itemsets();
  display_itemset(MaximalItemset);

  // Association rules
  AssociationRulesGen();
  cout<<"\n\nAssociation Rules\n";
  Display_Rules(ARules);
  cout<<ARules.size();

  cout<<"\n\nNon generated Rules\n";
  Display_Rules(ARnongen);
  cout<<ARnongen.size();
  return 0;
}
