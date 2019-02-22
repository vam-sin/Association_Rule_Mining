#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
using namespace std;

class Rule
{
public:
  vector<string> L,R;
  int S1,S2;
  float C;
  void Display();
};

int minsupcount=200;

float minconf=0.2;
vector<vector<string>> transactions;
vector<string> Items;
vector<vector<vector<string>>> C;
vector<vector<vector<string>>> L;
vector<vector<string>> L1;
vector<vector<string>> ClosedItemset;
vector<vector<string>> MaximalItemset;
vector<Rule> AR;
vector<Rule> ARnongen;

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
    //append transaction into transactions
    transactions.push_back(temp);
    i++;
  }
  sort(Items.begin(), Items.end());
  Items.erase(unique(Items.begin(), Items.end()), Items.end());
}

// Removes Duplicate Items from the given vector
vector<vector<string>> rem_dup(vector<vector<string>> S)
{
  sort(S.begin(), S.end());
  S.erase(unique(S.begin(), S.end()), S.end());
  return S;
}

// Checks if string a is present in vector S
int present_in_vec(vector<string> S,string a)
{
  int flag=0; // not present
  for(int i=0;i<S.size();i++)
  {
    if(S[i]==a)
    flag=1;
  }
  return flag;
}

// gets L[k] from C[k-1] and L[1]
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
      // cout << row[0];
      if(!present_in_vec(row,S2[j][0]))
      {
        row.push_back(S2[j][0]);
        sort(row.begin(),row.end());
        P.push_back(row);
      }
    }
  }
  return P;
}

// Gets the support of given Itemset
int getSupport(vector<string> Itemset)
{
  int count=0;
  for(int i=0; i<transactions.size(); i++)
  {
    int c=0;
    for(int k=0; k<Itemset.size(); k++)
    {
      for(int j=0; j<transactions[i].size(); j++)
      {
        if(Itemset[k]==transactions[i][j])
          c++;
      }
    }
    if(c==Itemset.size())
      count++;
  }
  return count;
}


// Prunes L[k] to get C[k] wrt to min. support
vector<vector<string>> prune(vector<vector<string>> S)
{
  vector<vector<string>> Q;
  for(int i=0;i<S.size();i++)
  {
    int supportcount=getSupport(S[i]);
    if(supportcount>=minsupcount)
    {
      Q.push_back(S[i]);
    }
  }
  return Q;
}

// checks if A is a subset of B
int subset(vector<string> A, vector<string> B)
{
  int c=0;
  for(int i=0; i<A.size(); i++)
  {
    for(int j=0; j<B.size(); j++)
    {
      if(A[i]==B[j])
        c++;
    }
  }
  if(c==A.size())
    return 1;
  else
    return 0;
}

// gets closed itemsets
void closed_itemsets()
{
  for(int i=0; i<C.size()-1; i++)
  {
    for(int j=0; j<C[i].size(); j++)
    {
      int c=0;
      for(int k=0; k<C[i+1].size(); k++)
      {
        if(subset(C[i][j], C[i+1][k]))
        {
          if(getSupport(C[i][j])==getSupport(C[i+1][k]))
            c++;
        }
      }
      if(c==0)
      {
        ClosedItemset.push_back(C[i][j]);
      }
    }
  }
}

// gets maximal frequent itemsets
void max_itemsets()
{
  for(int i=0; i<C.size()-1; i++)
  {
    for(int j=0; j<C[i].size(); j++)
    {
      int c=0;
      for(int k=0; k<C[i+1].size(); k++)
      {
        if(subset(C[i][j], C[i+1][k]))
        {
          c++;
        }
      }
      if(c==0)
      {
        MaximalItemset.push_back(C[i][j]);
      }
    }
  }
}

// Confidence formula (2,5)->(3) is sup(2,5,3)/sup(2,5) || X->Y
float getConf(vector<string> X,vector<string> Y)
{
  vector<string> S;
  for(int i=0;i<X.size();i++)
  S.push_back(X[i]);
  for(int i=0;i<Y.size();i++)
  S.push_back(Y[i]);
  float val=(float)getSupport(S)/getSupport(X);
  return val;
}


// Display 1D vector of strings
void display1D(vector<string> S)
{
  for(int i=0; i<S.size(); i++)
  {
    cout<<S[i]<<", ";
  }
  cout<<"\b\b";
}

// Display a rule
void Rule::Display()
{
  display1D(L);
  cout<<" ("<<S1<<") ---> ";
  display1D(R);
  cout<<" ("<<S2<<") - conf("<<C<<")"<<endl;
}


// Display Itemset with support
void display_itemset(vector<vector<string>> Itemset)
{
  for(int i=0; i<Itemset.size(); i++)
  {
    for(int j=0; j<Itemset[i].size(); j++)
      cout<<Itemset[i][j]<<",";

    cout<<"\b ("<<getSupport(Itemset[i])<<")";
    cout<<endl;
  }
  cout<<Itemset.size();
}

void display_rules(vector<Rule> AR)
{
  for(int i=0;i<AR.size();i++)
  {
    AR[i].Display();
  }
  cout<<AR.size();
}

// Association rules
void get_rules()
{
  // looping through all the C's
  for(int t=0;t<C.size();t++)
  {
    for(int w=0;w<C[t].size();w++)
    {
      for(int k=1;k<C[t][w].size();k++)
      {
        // for 3 itemset you run this twice
        for(int i=0;i<C[t][w].size();i++)
        {
          vector<string> X,Y ;
          Rule temp;
          for(int j=0;j<k;j++)
          {
            if((i+j)>(C[t][w].size()-1))
            X.push_back(C[t][w][i+j-C[t][w].size()]);
            else
            X.push_back(C[t][w][j+i]);
          }
          for(int j=0;j<C[t][w].size();j++)
          {
            if(!present_in_vec(X,C[t][w][j]))
            {
              Y.push_back(C[t][w][j]);
            }
          }
          float conf=getConf(X,Y);

          if(conf>=minconf)
          {
            sort(X.begin(),X.end());
            temp.L=X;
            temp.R=Y;
            temp.S1 = getSupport(X);
            temp.S2 = getSupport(Y);
            temp.C = conf;
            AR.push_back(temp);
          }
          else
          {
            sort(X.begin(),X.end());
            temp.L=X;
            temp.R=Y;
            temp.S1 = getSupport(X);
            temp.S2 = getSupport(Y);
            temp.C = conf;
            ARnongen.push_back(temp);
          }
          X.clear();
          Y.clear();
        }
      }
    }
  }
}

void pruning()
{

  L.push_back(L1); // L1 stored in L[0]
  C.push_back(prune(L1)); // C1 stored in C[0]
  cout<<endl;
  cout << "C1" << endl;
  display_itemset(C[0]);
  cout<<endl;

  for(int i=1;i<L1.size();i++)
  {
    L.push_back(product(C[i-1],C[0]));
    L[i]=rem_dup(L[i]);
    cout << endl;
    cout << "C" << i+1 << endl;
    C.push_back(prune(L[i]));
    display_itemset(C[i]);
  }
}

int main()
{

  read_csv("groceries.csv");
  cout<<"Enter minimum support count: ";
  cin>>minsupcount;
  cout<<"\n\nEnter minimum confidence: ";
  cin>>minconf;
  cout<<endl;
  // Gets L1
  for(int i=0;i<Items.size();i++)
  {
    vector<string> temp;
    temp.push_back(Items[i]);
    L1.push_back(temp);
  }

  // Loop through cout,rem_dup and display max_column_size times
  pruning();

  closed_itemsets();
  max_itemsets();
  get_rules();

  cout<<"\nClosed\n\n";
  display_itemset(ClosedItemset);
  cout<<"\nMaximal\n\n";
  display_itemset(MaximalItemset);
  cout<<"\nRules\n\n";
  display_rules(AR);
  cout<<"\nNon gen Rules\n";
  display_rules(ARnongen);

}
