#include<iostream>
#include<math.h>
#include<string>
#include<stack>
#include<vector>

using namespace std;

/*
  本题整体代码逻辑:
  1.将用户输入的中缀语句转化为后缀语句
  2.将后缀语句转化为便于解析的语法树
  3.通过遍历语法树获得真值并输出
 */

//定义运算符优先级对应的结构体
typedef struct oper_prio {
	char oper;
	int prio;
} oper_prio;

//存储运算符优先级的数组
const oper_prio ops[5] = {
	{'!', 4},
	{'&', 3},
	{'|', 2},
	{'-', 1},
	{'+', 0}
};


//用于存放操作符
const char opers[5] = {'!', '&', '|', '-', '+'};

//用于存放二元运算符
const char bin_opers[4] = {'&', '|', '-', '+'};

//用于存放变元
vector<char> argu;

//判断运算符优先级
//gr表示greater than缩写
bool priorityGR(char input1, char input2) {
	int num1 = 0;
	int num2 = 0;
	//将输入字符转化为对应的等级
	for (int i = 0; i < 5; i++) {
		if (input1 == ops[i].oper) {
			num1 = ops[i].prio;
		}
		if (input2 == ops[i].oper) {
			num2 = ops[i].prio;
		}
	}
	//返回等级比较结果
	return num1 > num2;
}

//判断是否为二元运算符
bool isBinOper(char input) {
	for (int i = 0; i < 4; i++) {
		if (input == bin_opers[i]) {
			return true;
		}
	}
	return false;
}

//判断是否为运算符
bool isOper(char input) {
	if (isBinOper(input) || input == '!') {
		return true;
	} else {
		return false;
	}
}


//将中缀语句生成为后缀语句
//缩写generate为gen,Reverse Polish为RP
string genRP(string input) {
	//1.创建一个空的栈,用于存储运算符
	stack<char> temp;
	//创建结果存储和对应长度
	string res;
	int length = input.size();
	//2.从左到右遍历输入的算式,读取每个字符
	for (int i = 0; i < length; i++) {
		/*
		  3.如果当前字符是一个运算符，那么比较其与栈顶运算符的优先级。
		  如果该运算符优先级较低，那么将栈顶运算符弹出并输出到输出队列中，
		  重复此步骤直到栈顶运算符优先级小于该运算符或栈为空。
		  然后将该运算符推入栈中。
		 */
		if (isOper(input[i])) {
			while ((!temp.empty()) && (!priorityGR(input[i], temp.top()))) {
				res.push_back(temp.top());
				temp.pop();
			}
			temp.push(input[i]);
		}
		//4.如果当前字符是一个左括号,那么将其推入栈中
		else if (input[i] == '(') {
			temp.push(input[i]);
		}
		//5.如果当前字符是一个右括号,那么将栈中的元素弹出
		//将其输出到输出队列中,直到遇到左括号
		else if (input[i] == ')') {
			while (temp.top() != '(') {
				res.push_back(temp.top());
				temp.pop();
			}
			temp.pop();
		}
		//6.如果当前字符是一个变元,那么将其输出到输出队列中
		else {
			argu.push_back(input[i]);
			res.push_back(input[i]);
		}
	}
	//将栈内剩余所有符号排空
	while (!temp.empty()) {
		res.push_back(temp.top());
		temp.pop();
	}
	return res;
}

//定义语法树数据结构
//缩写Abstract Syntax Tree为AST
typedef struct AST {
	char data;
	AST* leftChild;
	AST* rightChild;
}*treeNode;

//将后缀语句转化为语法树
treeNode genAST(string inputRP) {
	//1.创建一个空栈用于存放节点
	stack<treeNode> temp;
	//2.从左到右扫描逆波兰式
	int length = inputRP.size();
	for (int i = 0; i < length; i++) {
		//3.如果当前字符是运算符,则从栈中弹出两个节点,链接后推入栈中
		//左右孩子为弹出节点,节点数据为运算符
		//这里要对一元运算符单独判断,一元运算符则弹出一个节点
		if (isOper(inputRP[i])) {
			treeNode t = new AST;
			t->data = inputRP[i];
			treeNode leftchild = temp.top();
			t->leftChild = leftchild;
			t->rightChild = leftchild;
			temp.pop();
			if (inputRP[i] != '!') {
				treeNode rightchild = temp.top();
				t->rightChild = rightchild;
				temp.pop();
			}
			temp.push(t);
		}
		//4.如果当前字符是变元,则创建新节点并推入栈中
		//左右孩子皆空,节点数据为变元
		else {
			treeNode t = new AST;
			t->data = inputRP[i];
			t->leftChild = nullptr;
			t->rightChild = nullptr;
			temp.push(t);
		}
	}
	//5.遍历结束后栈中只剩下一个AST根节点,返回该节点
	return temp.top();
}

//输入运算符和两个变元,返回运算结果
//!表示非运算,&表示与运算,|表示或运算,-表示蕴含运算,+表示等价运算
bool calc(char oper, bool num1, bool num2) {
	switch (oper) {
		case '!':
			return !num1;
		case '&':
			return num1 && num2;
		case '|':
			return num1 || num2;
		case '-':
			return !num1 || num2;
		case '+':
			return (!num1 || num2) && (!num2 || num1);
	}
	return false;
}

//输入语法树和对应的赋值数组计算算式结果
bool parseAST(treeNode root, vector<bool> assign) {
	stack<treeNode> temp;
	treeNode t = root;
	//如果左右节点不均为变元,则持续压栈
	while (isBinOper(t->rightChild->data)) {
		temp.push(t);
		t = t->rightChild;
	}
	//定义计数器,按照指定赋值进行计算
	int counter = 0;
	//通过变元数,限制循环次数
	int ar_size = argu.size();
	//保存计算结果并作为下次计算初始值
	bool res = assign[counter++];
	if (t->rightChild->data == '!') {
		res = !res;
	}
	//如果左右节点均为变元到达树底,则开始计算
	while (counter < ar_size) {
		//输入两个初始值,上次计算结果语指定赋值
		bool ass = assign[counter++];
		if (t->leftChild->data == '!') {
			ass = !ass;
		}
		res = calc(t->data, res, ass);
		if (!temp.empty()) {
			t = temp.top();
			temp.pop();
		}
	}
	return res;
}

//生成赋值数组并计算真值表内容
//存储计算结果
vector<vector<bool>> truth_res;

//生成赋值数组并计算最终结果存储结果向量
void genResult(treeNode root) {
	//计算赋值种类数并其进行二进制解析
	int argu_size = argu.size();
	int assign_num = pow(2, argu.size());
	truth_res.resize(assign_num, vector<bool>(argu_size + 1));
	//对所有赋值种类进行遍历
	for (int i = 0; i < assign_num; i++) {
		//对每一种赋值情况拆解成二进制数
		//这样既可以保证内存安全,也可以高效遍历所有情况
		vector<bool> assign;
		int counter = 0;
		for (int j = 0; j < argu_size; j++) {
			//使用位运算拆解成二进制数作为赋值
			bool temp = i >> j & 1;
			truth_res[i][counter++] = temp;
			assign.push_back(temp);
		}
		bool res = parseAST(root, assign);
		truth_res[i][counter++] = res;
	}
}

/*
  通过genResult生成的二维向量来进行主析取范式和主合取范式的生成
  由于生成主析取范式和主合取范式十分相似
  这里只以生成主析取范式函数genDNF注释为例
 */
//根据结果数组生成主析取范式DNF
string genDNF() {
	string res ;
	int argu_size = argu.size();
	int assign_num = pow(2, argu.size());
	int counter = 0;
	for (int i = 0; i < assign_num; i++) {
		//查看真值表中所有输出为1的行
		if (truth_res[i][argu_size] == 1) {
			counter++;
			if (counter != 1) {
				res.push_back('|');
			}
			res.push_back('(');
			for (int j = 0; j < argu_size; j++) {
				if (j != 0) {
					res.push_back('&');
				}
				//如果某个输入变量为1,则直接写下这个变量
				//如果某个输入变量为0,则写下这个变量的非
				if (truth_res[i][j] != 1) {
					res.push_back('!');
				}
				res.push_back(argu[j]);
			}
			res.push_back(')');
		}
	}
	//将结果用|进行连接,形成主析取范式
	return res;
}

//根据结果数组生成主合取范式CNF
string genCNF() {
	string res ;
	int argu_size = argu.size();
	int assign_num = pow(2, argu.size());
	int counter = 0;
	for (int i = 0; i < assign_num; i++) {
		if (truth_res[i][argu_size] == 0) {
			counter++;
			if (counter != 1) {
				res.push_back('&');
			}
			res.push_back('(');
			for (int j = 0; j < argu_size; j++) {
				if (j != 0) {
					res.push_back('|');
				}
				if (truth_res[i][j] != 0) {
					res.push_back('!');
				}
				res.push_back(argu[j]);
			}
			res.push_back(')');
		}
	}
	return res;
}

//生成输入公式类型
//通过genResult生成的二维向量来进行公式类型判断
string genType(){
	int argu_size = argu.size();
	int assign_num = pow(2, argu.size());
	int counter = 0;
	string type;
	for (int i = 0; i < assign_num; i++) {
		if (truth_res[i][argu_size ] == 1) {
			counter++;
		}
	}
	if (counter == 0) {
		type.append("输入公式为永假式");
		
	}
	if (counter < assign_num && counter != 0) {
		if (counter == assign_num) {
			type.append("输入公式为可满足式中的永真式");
		}else{
			type.append("输入公式为可满足式");
		}
	}
	return type;
}

//以下为打印函数内容,用于输出指定内容
//打印真值表表头
void printHeader(string input) {
	int argu_size = argu.size();
	for (int i = 0; i < argu_size; i++) {
		cout << argu[i] << " | ";
	}
	cout << input << endl;
	// 打印分隔线
	for (int i = 0; i < argu_size; i++) {
		cout << "--|-";
	}
	int input_size = input.size();
	for (int i = 0; i < input_size; i++) {
		cout << "-" ;
	}
	cout << endl;
	return;
}

//根据结果数组打印真值表内容
void printTable() {
	//计算赋值种类数并其进行二进制解析
	int argu_size = argu.size();
	int assign_num = pow(2, argu.size());
	for (int i = 0; i < assign_num; i++) {
		for (int j = 0; j < argu_size; j++) {
			cout << truth_res[i][j] << " | ";
		}
		cout << truth_res[i][argu_size] << endl;
	}
	return;
}

//根据结果打印主析取范式
void printDNF() {
	cout << "主析取范式为:" << endl;
	cout << genDNF() << endl;
	return;
}
//根据结果打印主合取范式
void printCNF() {
	cout << "主合取范式为:" << endl;
	cout << genCNF() << endl;
	return;
}

//根据结果数组打印公式类型
void printType() {
	cout<<genType()<<endl;
}

int main() {
	cout << "请输入需要处理的公式:" << endl;
	string input;
	cin >> input;
	string inputRP = genRP(input);
	treeNode root = genAST(inputRP);
	genResult(root);
	printHeader(input);
	printTable();
	printDNF();
	printCNF();
	printType();
	return 0;
}
