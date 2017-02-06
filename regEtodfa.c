#include <bits/stdc++.h>

using namespace std;

struct node {
  char symbol;
  struct node *left;
  struct node *right;
  bool nullable;
  int position;
  set<int> firstpos;
  set<int> lastpos;
};

struct transition {
  int istate;
  char symbol;
  int fstate;
};

char postfixExpr[50];
char expr[50];
int followpos[40][40];
set<char> symbolSet;
struct node *allNodes[50];//all node from bottom up manner with count node Count
static int nodeCount = 0;
map<int,char> mp;//node correspond to which symbol
map<int,set<int> > states;
struct transition transTable[50];
static int transCount = 0;

void preprocess() {
  char intermediate[40];
  int s = 0,i;
  for(i = 0; i < strlen(expr)-1; i++) {
    if('a' <= expr[i] && expr[i] <= 'z') {
      intermediate[s++] = expr[i];
      if('a' <= expr[i+1] && expr[i+1] <= 'z') {
        intermediate[s++] = '.';
      }
    } else if(expr[i] == '(' || expr[i] == '|') {
      intermediate[s++] = expr[i]; 
    } else if(expr[i] == ')' || expr[i] == '*') {
      intermediate[s++] = expr[i];
      if(('a' <= expr[i+1] && expr[i+1] <= 'z') || expr[i+1] == '(') {
        intermediate[s++] = '.';
      }
    }
  }
  intermediate[s++] = expr[i];
  intermediate[s++] = '\0';
  strcpy(expr,intermediate);
  return;
}

void toPostfix() {
  stack<char> stk;
  int s = 0;
  char x;
  for(int i=0; i < strlen(expr); i++) {
    if(expr[i] == '(') {
      stk.push(expr[i]);
    } else if(expr[i] == ')') {
      while(stk.top() != '(') {
        x = stk.top();
        stk.pop();
        postfixExpr[s++] = x;
      }
      stk.pop();
    } else if(expr[i] == '.') {
      if(stk.empty() || stk.top() == '(') {
        stk.push('.');
      } else if(stk.top() == '.') {
        postfixExpr[s++] = '.';
      } else if(stk.top() == '|') {
        postfixExpr[s++] = '|';
        stk.pop();
        stk.push('.');
      }
    } else if(expr[i] == '|') {
      if(stk.empty() || stk.top() == '(') {
        stk.push('|');
      } else if(stk.top() == '|') {
        postfixExpr[s++] = '|';
      } else if(stk.top() == '.') {
        postfixExpr[s++] = '.';
        stk.pop();
        stk.push('|');
      }
    } else if(expr[i] == '*') {
      postfixExpr[s++] = '*';
    } else {
      postfixExpr[s++] = expr[i];
    }
  }
  while(!(stk.empty())) {
    x = stk.top();
    stk.pop();
    postfixExpr[s++] = x;
  }
  stk.pop();
  postfixExpr[s++] = '#';
  postfixExpr[s++] = '.';
  postfixExpr[s++] = '\0';
  return;
}

int countSymbol() {
  int count = 0;
  for(int i = 0; i < strlen(expr); i++) {
    if((expr[i]  >= 'a' && expr[i] <= 'z') || expr[i] == '#') {
      count++;
      if(expr[i] != '#')symbolSet.insert(expr[i]);
    }
  }
  return count;
}

struct node * createTree(int count) {
  struct node *head[count], *root, *temp;
  int j = 0, k;
  for(int i = 0; i < strlen(postfixExpr); i++) {
    if((postfixExpr[i]  >= 'a' && postfixExpr[i] <= 'z') || postfixExpr[i] == '#') {
      head[j] = new node;
      head[j]->symbol = postfixExpr[i];
      head[j]->position = j+1;
      head[j]->nullable = false;
      head[j]->right = NULL;
      head[j]->left = NULL;
      head[j]->firstpos.insert(j+1);
      head[j]->lastpos.insert(j+1);
      j++;
    }
  }
  j = 0, k = 1;
  for(int i = 0; i < strlen(postfixExpr); i++) {
      if(postfixExpr[i] == '|' ) { 
        root = new node;
        root->symbol = postfixExpr[i];
        root->position = -1;
        root->nullable = false;
        root->right = head[k];
        root->left = head[j];
        head[k] = root;
        head[j] = NULL;
        j = k;
        k++;
        if(root->left != NULL) {
          root->firstpos.insert(root->left->firstpos.begin(),root->left->firstpos.end());
          root->lastpos.insert(root->left->lastpos.begin(),root->left->lastpos.end());
        }
        if(root->right != NULL) {
          root->firstpos.insert(root->right->firstpos.begin(),root->right->firstpos.end());
          root->lastpos.insert(root->right->lastpos.begin(),root->right->lastpos.end());
        }
        root->nullable = root->left->nullable || root->right->nullable;
        //printf("%d or done\n", i);
      } else if(postfixExpr[i] == '.') {
        root = new node;
        root->symbol = postfixExpr[i];
        root->position = -1;
        root->nullable = false;
        root->right = head[k];
        root->left = head[j];
        head[k] = root;
        head[j] = NULL;
        j = k;
        k++;
        root->nullable = root->left->nullable && root->right->nullable;
        if(root->left->nullable == true) {
          if(root->left != NULL) {
            root->firstpos.insert(root->left->firstpos.begin(),root->left->firstpos.end());
          }
          if(root->right != NULL) {
            root->firstpos.insert(root->right->firstpos.begin(),root->right->firstpos.end());
          }
        } else {
          if(root->left != NULL) {
            root->firstpos.insert(root->left->firstpos.begin(),root->left->firstpos.end());
           }
        }
        if(root->right->nullable == true) {
          if(root->left != NULL) {
            root->lastpos.insert(root->left->lastpos.begin(),root->left->lastpos.end());
          }
          if(root->right != NULL) {
            root->lastpos.insert(root->right->lastpos.begin(),root->right->lastpos.end());
          }
        } else {
          if(root->right != NULL) {
            root->lastpos.insert(root->right->lastpos.begin(),root->right->lastpos.end());
          }
        }
        //printf("%d and done\n", i);
      } else if(postfixExpr[i] == '*') {
        root = new node;
        root->symbol = postfixExpr[i];
        root->position = -1;
        root->nullable = true;
        root->right = NULL;
        root->left = head[j];
        head[j] = root;
        root->firstpos.insert(root->left->firstpos.begin(),root->left->firstpos.end());
        root->lastpos.insert(root->left->lastpos.begin(),root->left->lastpos.end());
        //printf("%d * done\n", i);
      }
  }
  return root;
}

void calculateFollowPos(struct node *root) {
  if(root == NULL)
    return;
  set<int>::iterator it, it2;
  if(root->symbol == '.') {
    for(it = root->left->lastpos.begin(); it != root->left->lastpos.end(); it++) {
      for(it2 = root->right->firstpos.begin(); it2 != root->right->firstpos.end(); it2++) {
        followpos[*it][*it2] = 1;
      }
    }
  } else if(root->symbol == '*') {
    for(it = root->left->lastpos.begin(); it != root->left->lastpos.end(); it++) {
      for(it2 = root->left->firstpos.begin(); it2 != root->left->firstpos.end(); it2++) {
        followpos[*it][*it2] = 1;
      }
    }
  }
  calculateFollowPos(root->left);
  calculateFollowPos(root->right);
  allNodes[nodeCount++] = root;
  if(root->position != -1) 
    mp[root->position] = root->symbol;
}

void makeDFA(struct node *root, int count) {
  int t = 0;
  states[t] = root->firstpos;
  map<int,int> unamrked;
  map<int,int>::iterator itm;
  unamrked[t] = 1;
  while(!(unamrked.empty())) {
    itm = unamrked.begin();
    for(set<char>::iterator it = symbolSet.begin(); it != symbolSet.end(); it++) {
      set<int> temp, tmp2;
      tmp2 = states[itm->first];
      for(set<int>::iterator it2 = tmp2.begin(); it2 != tmp2.end(); it2++) {
        if(mp[*it2] == *it) {
          for(int i = 1; i <= count; i++) {
            if(followpos[*it2][i]) 
              temp.insert(i);
          }
        }
      }
      bool flag = true;
      for(map<int,set<int> >::iterator it2 = states.begin(); it2 != states.end(); it2++) {
        if(it2->second == temp) {
          transTable[transCount].istate = itm->first;
          transTable[transCount].symbol = *it;
          transTable[transCount].fstate = it2->first;
          transCount++;
          flag = false;
          break;
        }    
      }

      if(flag && !(temp.empty())) {
        t++;
        states[t] = temp;
        unamrked[t] = 1;
        transTable[transCount].istate = itm->first;
        transTable[transCount].symbol = *it;
        transTable[transCount].fstate = t;
        transCount++;
      }
    }
    unamrked.erase(itm);
  }
  
  return;
}

int main() {
  int count;
  printf("Enter regular expression with small english alphabets only:");
  scanf("%s", expr); 
  count = strlen(expr);
  preprocess();
  printf("%s\n", expr);
  toPostfix();
  count = countSymbol();
  count++;
  printf("%s\n", postfixExpr);
  struct node *temp = createTree(count);
  memset(followpos,0,sizeof(followpos));
  calculateFollowPos(temp);
  makeDFA(temp,count);
  for(int i = 0; i < nodeCount; i++) {
    cout << allNodes[i]->symbol << " " << allNodes[i]->position << endl << "Firstpos: ";
    for(set<int>::iterator it2 = allNodes[i]->firstpos.begin(); it2 != allNodes[i]->firstpos.end(); it2++) {
      cout << *it2 << ' ';
    }
    cout << endl << "Lastpos: ";
    for(set<int>::iterator it2 = allNodes[i]->lastpos.begin(); it2 != allNodes[i]->lastpos.end(); it2++) {
      cout << *it2 << ' ';
    }
    cout << endl;
  }
  cout << "Follwpos" << endl;
  for(int i = 1; i <= count; i++) {
    cout << i << " --> ";
    for(int j = 1; j <= count ;j++) {
      if(followpos[i][j]) cout <<  j << " ";
    }
    cout << endl;
  }
  cout << "Node set" << endl;
  for(map<int,set<int> >::iterator it2 = states.begin(); it2 != states.end(); it2++) {
    cout << it2->first << " -> ";
    for(set<int>::iterator it = it2->second.begin(); it != it2->second.end(); it++) {
      cout << *it << " ";
    } 
    cout << endl;    
  }
  printf("%d\n",transCount);
  for(int i = 0; i < transCount; i++) {
    printf("move(%d,%c) = %d\n", transTable[i].istate, transTable[i].symbol, transTable[i].fstate);
  }
  return 0;
}