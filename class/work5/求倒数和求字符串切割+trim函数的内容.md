```cpp
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               #include<iostream>
#include<vector>
#include <iterator>
#include <algorithm>
using namespace std;
int main(){
	vector<int> QUO,REM; //两个向量 ，
	REM.push_back(1);
	int div  = 1; // 被除数
	int N = 0; // 输入的N
	cin>>N;
	 
	int flag = -1;
	while(1){
		if(div < N){
			div *= 10;
		}
		int q = div / N;
		QUO.push_back(q);
		int r = div % N;
		if(r == 0){
			flag = -1;
			break;
		}
		auto it = find(REM.begin(),REM.end(),r);  // iterator 迭代器。cousor // 游标 
		if(it != REM.end()){
			flag = distance(REM.begin(), it);
			break;
		}
		REM.push_back(r);
		div = r;

	}

	return 0;
} 
```

```cpp
vector<string> v;
void split(string domain,string mode){
	if(domain.size() == 0 || mode.size() == 0 ){
		return;
	}
	int dSize = domain.size();
	int mSize = mode.size();

	int index = domain.find(mode);
	if(index == -1){
		v.push_back(domain);
		return;
	}
	v.push_back(domain.substr(0,index)); // 777
	string s = domain.substr(index+mSize,dSize-index-mSize);//666*^88888*^6666*^ 
	split(s,mode);
}

trim()    "  123123 123123999  " => "123123123123999"

int main(){

	string str = "777*^666*^88888*^6666*^";

	split(str,"*^");


	return 0;
}
```
