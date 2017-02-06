#include <bits/stdc++.h>

using namespace std;

struct rule {
	string before;
	vector<string> after;
};

struct rule *grammer[10];
static int countrules = 0;

set<string> nonterminals;
set<string> terminals;

map<string,int> number;
map<string,set<string> > first;
map<string,set<string> > follow;
set<string> firstSubstring; 

map<pair<string,string>,string> ptable;

void findNonterminals() {
	for(int i = 0; i < countrules; i++) {
		nonterminals.insert(grammer[i]->before);
		number[grammer[i]->before] = i;
	}
	return;
}

void findTerminals() {
	vector<string> currg;
	string temp;
	for(int i = 0; i < countrules; i++) {
		currg = grammer[i]->after;
		for(int j = 0; j < currg.size(); j++) {
			//cout << currg[j] << endl;
			for(int k = 0; k < currg[j].length(); k++) {
				if(currg[j][k] < 'A' || currg[j][k] > 'Z') {
					temp = currg[j][k];
					terminals.insert(temp);
				}
			}
		}
	}
	return;
}

void findFirstOfTerminals() {
	set<string> currg;
	for(set<string>::iterator it = terminals.begin(); it != terminals.end(); it++) {
		currg.insert(*it);
		first[*it] = currg;
		currg.clear();
	}
	return;
}

void findFirstOfNonTerminals(string key) {
	map<string,int>::iterator it = number.find(key);
	if(it == number.end() || first.find(key) != first.end())
		return;
	string temp;
	vector<string> currg;
	set<string> keyfirst;
	currg = grammer[it->second]->after;
	int flag = 1;
	for(int i = 0; i < currg.size(); i++) {
		for(int j = 0; j < currg[i].length(); j++) {
			temp = currg[i][j];
	//		cout << "here it" << endl;
			if(terminals.find(temp) != terminals.end()) {
				keyfirst.insert(temp);
				break;
			} else {
				findFirstOfNonTerminals(temp);
				if(flag) {
					for(set<string>::iterator it = first[temp].begin(); it != first[temp].end(); it++) {
						keyfirst.insert(*it);
					}
					if(first[temp].find("#") == first[temp].end()) 
						flag = 0;
				}
			}
		}
	}
	//cout << "here end" << endl;
	first[key] = keyfirst;
	for(set<string>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++) {
		//cout << "here end it" << endl;
		if(first.find(*it) == first.end()) {
			findFirstOfNonTerminals(*it);
		}
	}
	return;
}

void findFirstOfSubstring(string param) {
	string temp;
	int flag ,i;
	for(i=0; i < param.length(); i++) {
		temp = param[i];
		flag = 0;
		for(set<string>::iterator it = first[temp].begin(); it != first[temp].end() ;it++) {
			if(*it == "#") {
				flag = 1;
			} else {
				firstSubstring.insert(*it);
			}

		}
		if(flag == 0)
			break;
	}
	if(flag == 1 && i == param.length())
		firstSubstring.insert("#");
	return;
}

void findFollow(int index) {
	if(index >= countrules)
		return;
	int flag;
	string tempx;
	for(int i = 0; i < grammer[index]->after.size(); i++) {
		string nter = grammer[index]->after[i];
		int j;
		flag = 0;
		int count = 0;
		for(j = 0; j < nter.length()-1; j++) {
			if(nter[j] >= 'A' && nter[j] <= 'Z') {
				findFirstOfSubstring(nter.substr(j+1,nter.length()-(j+1) ) );
				for(set<string>::iterator it = firstSubstring.begin(); it != firstSubstring.end(); it++) {
					if(*it == "#") {
						flag = 1;
					} else {
						tempx = nter[j];
						if(follow[tempx].insert(*it).second == true)
							count++;
					}
				}
				firstSubstring.clear();
			}
			if(flag == 1) {
				for(set<string>::iterator it2 = follow[grammer[index]->before].begin(); it2 != follow[grammer[index]->before].end() ;it2++) {
					tempx = nter[j];
					if(follow[tempx].insert(*it2).second == true)
						count++;

				}
			}
		}
		if(nter[j] >= 'A' && nter[j] <= 'Z') {
			for(set<string>::iterator it2 = follow[grammer[index]->before].begin(); it2 != follow[grammer[index]->before].end() ;it2++) {
				tempx = nter[j];
				if(follow[tempx].insert(*it2).second == true)
					count++;

			}
		}
		tempx = nter[j];
		if((number[tempx] <= index) && count > 0)
			findFollow(number[tempx]);
	}
	findFollow(index+1);
	return;
}

void findParsingTable() {
	int flag;
	for(int i = 0; i < countrules; i++) {
		for(vector<string>::iterator it2 = grammer[i]->after.begin(); it2 != grammer[i]->after.end(); it2++) {
			findFirstOfSubstring(*it2);
			flag = 0;
			for(set<string>::iterator it = firstSubstring.begin(); it != firstSubstring.end(); it++) {
				if(*it == "#") {
					flag = 1;
				} else {
					ptable[make_pair(grammer[i]->before,*it)] = *it2;
				}
			}
			if(flag == 1) {
				//printf("here %d\n", follow[grammer[i]->before].size());
				for(set<string>::iterator it = follow[grammer[i]->before].begin(); it != follow[grammer[i]->before].end(); it++) {
					ptable[make_pair(grammer[i]->before,*it)] = *it2;
				}	
			}
			firstSubstring.clear();
		}
	}
	return;
}

int main() {
	int t;
	char  line[50];
	char *pch;
	scanf("%d", &t);
	getchar();
	while(t--) {
		grammer[countrules] = new rule;
		scanf("%[^\n]s", line);
		getchar();
		pch = strtok(line," >|");
		grammer[countrules]->before = pch;
		pch = strtok (NULL, " >|");
		pch = strtok (NULL, " >|");
		while (pch != NULL) {
			grammer[countrules]->after.push_back(pch);
			pch = strtok (NULL, " >|");
		}
		countrules++;
	}
	findNonterminals();
	findTerminals();
	/*for(set<string>::iterator it = terminals.begin(); it != terminals.end(); it++) {
		cout << *it << " ";
	}
	cout << endl;
	for(set<string>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++) {
		cout << *it << " ";
	}
	cout << endl;
	*/
	findFirstOfTerminals();
	findFirstOfNonTerminals(grammer[0]->before);
	/*for(map<string,set<string> >::iterator it = first.begin(); it != first.end(); it++) {
		cout << it->first << " -> ( ";
		for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << " )\n";
	}
	*/
	follow[grammer[0]->before].insert("$"); //insert $ in follow of starting symbol assume first rule contains startnig symbol
	findFollow(0);
	/*
	for(map<string,set<string> >::iterator it = follow.begin(); it != follow.end(); it++) {
		cout << it->first << " -> ( ";
		for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << " )\n";
	}
	*/
	findParsingTable();
	for(map<pair<string,string>,string>::iterator it = ptable.begin(); it != ptable.end(); it++) {
		cout << "M[" << it->first.first << "," << it->first.second << "] = " << it->first.first  << "->" << it->second << endl;
	} 
	for(int i =0; i < countrules; i++)
		delete(grammer[i]);
	return 0;
}
/*
5
E -> TB
B -> +TB | #
T -> FC
C -> *FC | #
F -> (E) | i

4
S -> A
A -> Bb | Cd
B -> aB | #
C -> cC | #

1
S -> aSbS | bSaS | # 

3
S -> iE+SQ | a
Q -> eS | #
E -> b

3
S -> aABb
A -> c | #
B -> d | #
*/