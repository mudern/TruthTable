#include<iostream>
#include<algorithm>
#include<stack>
#include<vector>
#include<string>

using namespace std;

//以下为读取集合函数设计

/*
  解析集合数据,集合数据分为单元素和集合元素两种需要分别处理
  对于单元素,将字符数据读入字符串,遇到逗号停止读取并将字符串压入向量
  对于集合元素,编写针对非嵌套集合读取函数,通过递归拓展函数为读取嵌套集合
 */

//将只包含单元素数据的vector整合为集合形式string
string vecToString(vector<string> vec){
	string result;
	int vec_size=vec.size();
	result.push_back('{');
	for(int i=0;i<vec_size;i++){
		//针对首元素特殊处理
		if(i!=0){
			result.push_back(',');
		}
		result+=vec[i];
	}
	result.push_back('}');
	return result;
}

//实现单元素数据的处理并通过递归实现嵌套集合处理
//读取集合函数读取到左括号时调用该函数
string collectionToString(){
	vector<string> vec;
	string elem;
	string result;
	char temp;
	//未读到右括号时持续读取
	while(true){
		temp=getchar();
		//如果为左括号,则递归读取得到的字符串后作为元素加入向量
		//如果为右括号,则将最后部分结果集合加入向量,整理去重向量,将向量转化为字符串,结束读取
		//如果为逗号,则结束阶段读取,将部分结果集合作为元素加入向量,清空部分结果
		//如果为单元素,则读取作为部分结果
		if(temp=='{'){
			elem+=collectionToString();
		}else if(temp=='}'){
			vec.push_back(elem);
			sort(vec.begin(),vec.end());
			vec.erase(unique(vec.begin(),vec.end()),vec.end());
			result+=vecToString(vec);
			break;
		}else if(temp==','){
			vec.push_back(elem);
			elem={};
		}else{
			elem.push_back(temp);
		}
	}
	return result;
}

vector<string> getCollections(){
	vector<string> result;
	//如果输入有问题报错
	char temp=getchar();
	if(temp!='{'){
		cout<<"输入存在问题"<<endl;
		return result;
	}
	vector<string> vec;
	string elem;
	//未读到右括号时持续读取
	//如果为右括号,则调用集合读取函数
	//如果为右括号,则结束读取
	//如果为逗号,则结束阶段读取,将部分结果集合作为元素加入向量,清空部分结果
	//如果为单元素则读取作为部分结果
	while(true){
		temp=getchar();
		if(temp=='{'){
			elem+=collectionToString();
		}else if(temp=='}'){
			result.push_back(elem);
			sort(result.begin(),result.end());
			result.erase(unique(result.begin(),result.end()),result.end());
			break;
		}else if(temp==','){
			result.push_back(elem);
			elem={};
		}else{
			elem.push_back(temp);
		}
	}
	return result;
}

//以下为集合运算逻辑设计

//两个有序集合运算的核心为双指针
//核心逻辑为遍历针对不同情况采取不同措施,移动指针,最后对剩余情况进行处理

//求解两个集合交集
vector<string> calcInter(vector<string> a,vector<string> b){
	vector<string> res;
	int a_size=a.size(),b_size=b.size();
	int a_p=0,b_p=0;
	while(a_p<a_size&&b_p<b_size){
		if(a[a_p]<b[b_p]){
			a_p++;
		}
		if(b[b_p]<a[a_p]){
			b_p++;
		}
		if(a[a_p]==b[b_p]){
			res.push_back(a[a_p]);
			a_p++;
			b_p++;
		}
	}
	return res;
}

//求解两个集合并集
vector<string> calcUnion(vector<string> a,vector<string> b){
	vector<string> res;
	int a_size=a.size(),b_size=b.size();
	int a_p=0,b_p=0;
	while(a_p<a_size&&b_p<b_size){
		if(a[a_p]<b[b_p]){
			res.push_back(a[a_p]);
			a_p++;
		}
		if(b[b_p]<a[a_p]){
			res.push_back(b[b_p]);
			b_p++;
		}
		if(a[a_p]==b[b_p]){
			res.push_back(a[a_p]);
			a_p++;
			b_p++;
		}
	}
	while(a_p<a_size){
		res.push_back(a[a_p]);
		a_p++;
	}
	while(b_p<b_size){
		res.push_back(b[b_p]);
		b_p++;
	}
	return res;
}

//求解两个集合差集
vector<string> calcDiff(vector<string> a,vector<string> b){
	vector<string> res;
	int a_size=a.size(),b_size=b.size();
	int a_p=0,b_p=0;
	while(a_p<a_size&&b_p<b_size){
		if(a[a_p]<b[b_p]){
			res.push_back(a[a_p]);
			a_p++;
		}
		if(b[b_p]<a[a_p]){
			b_p++;
		}
		if(a[a_p]==b[b_p]){
			a_p++;
			b_p++;
		}
	}
	while(a_p<a_size){
		res.push_back(a[a_p]);
		a_p++;
	}
	return res;
}

//多个有序集合运算的核心为对两集合运算代码复用

vector<string> calcMulti(vector<vector<string>> vecs,vector<string> (*oper)(vector<string>,vector<string>)){
	int vecs_size=vecs.size();
	vector<string> temp=vecs[0];
	for(int i=1;i<vecs_size;i++){
		temp=oper(temp,vecs[i]);
	}
	return temp;
}

//求解幂集和m元子集
//求解m元子集的核心在于递归,求解幂集的核心在于位运算

string M_Subset_res={};
//m表示m元,n表示已找到元素,start标定集合范围,M_Subset_res表示结果
void genM_Subset(int m,int start,vector<string> origin_vec,vector<string> current_vec){
	if(int(current_vec.size())==m){
		M_Subset_res+=vecToString(current_vec);
		M_Subset_res+='\n';
		return;
	}
	
	int origin_vec_size=origin_vec.size();
	for(int i=start;i<origin_vec_size;i++){
		current_vec.push_back(origin_vec[i]);
		genM_Subset(m,i+1,origin_vec,current_vec);
		current_vec.pop_back();
	}
}

void calcM_Subset(int m,vector<string> vec){
	vector<string> temp;
	genM_Subset(m,0,vec,temp);
	return;
}

//使用位运算对生成数进行拆解
string calcPowerset(vector<string> vec){
	int vec_size=vec.size();
	int powerset_num=1<<vec_size;
	string result={};
	for(int i=0;i<powerset_num;i++){
		vector<string> elem;
		for(int j=0;j<vec_size;j++){
			bool temp = i >> j & 1;
			if(temp){
				elem.push_back(vec[j]);
			}
		}
		result+=vecToString(elem);
		result+='\n';
	}
	return result;
}

//输入与输出函数

void calcSetGuide(){
	cout<<"请输入所需计算的集合数量:";
	int num;
	cin>>num;
	vector<vector<string>> vecs;
	vecs.resize(num);
	vecs.clear();
	cout<<"请输入需要计算的集合"<<endl;
	for(int i=1;i<=num;i++){
		cout<<"集合"<<i<<':';
		getchar();
		vecs.push_back(getCollections());
	}
	cout<<"集合的交集为:"<<endl;
	cout<<vecToString(calcMulti(vecs,calcInter))<<endl;
	cout<<"集合的并集为:"<<endl;
	cout<<vecToString(calcMulti(vecs,calcUnion))<<endl;
	if(num==2){
		cout<<"集合差交集为:"<<endl;
		cout<<vecToString(calcMulti(vecs,calcDiff));
	}else{
		cout<<"集合差集只适用于2个集合"<<endl;
	}
	
}

void powerSetGuide(){
	vector<string> temp;
	getchar();
	cout<<"请输入需要计算的集合:";
	temp=getCollections();
	cout<<"集合的幂集为:"<<endl;
	cout<<calcPowerset(temp);
}

void M_SubSetGuide(){
	int n;
	cout<<"请输入需要计算m元子集的m:";
	cin>>n;
	getchar();
	vector<string> temp;
	cout<<"请输入需要计算的集合:";
	temp=getCollections();
	cout<<"集合的m元子集为:"<<endl;
	calcM_Subset(n,temp);
	cout<<M_Subset_res;
}

void mainGuide(){
	cout<<"请输入对应数字进行对应运算"<<endl;
	cout<<"1 计算交集并集与差集"<<endl;
	cout<<"2 计算M元子集"<<endl;
	cout<<"3 计算集合幂集"<<endl;
	int num;
	cin>>num;
	switch (num) {
	case 1:
		calcSetGuide();
		break;
	case 2:
		M_SubSetGuide();
		break;
	case 3:
		powerSetGuide();
		break;
	}
}

int main(){
	mainGuide();
}
