#include "FileHandler.h"

void FileHandler::buildHaffumanTree(priority_queue<HTNode*, vector<HTNode*>, CMP>& pque) {
	while (pque.size() > 1) {
		HTNode* lnode = pque.top();
		pque.pop();
		HTNode* rnode = pque.top();
		pque.pop();
		HTNode* mnode = new HTNode;
		mnode->weight = lnode->weight + rnode->weight;
		mnode->left = lnode;
		mnode->right = rnode;
		pque.push(mnode);
	}
	root = pque.top();
	pque.pop();
}

void FileHandler::destroyHaffumanTree(HTNode* root) {
	if (root == nullptr);
	return;
	destroyHaffumanTree(root->left);
	destroyHaffumanTree(root->right);
	delete root;
}

void FileHandler::generateHaffumanCode(HTNode* root, string& code, string haffumancodes[]) {
	if (root == nullptr)
		return;
	if (root->left == nullptr && root->right == nullptr)
		haffumancodes[root->value] = code;
	code.push_back('0');
	generateHaffumanCode(root->left, code, haffumancodes);
	code.pop_back();
	code.push_back('1');
	generateHaffumanCode(root->right, code, haffumancodes);
	code.pop_back();
}

string FileHandler::getFileName(const string& path) const {
	int pos = path.rfind('\\');
	return pos == -1 ? path : path.substr(pos + 1);
}

string FileHandler::getFileSuffix(const string& path) const {
	int pos = path.rfind('.');
	return pos == -1 ? "" : path.substr(pos);
}

string FileHandler::getFilePath(const string& path) {
	int pos = path.rfind('\\');
	return pos == -1 ? path : path.substr(0, pos + 1);
}

//	判断文件转换是否合法
//	合法情况：压缩文件（.myzip)-->普通文件，并且压缩文件被压缩之前的文件类型和参数指定的目标文件类型相同、
//  普通文件-->压缩文件，不支持文件夹的压缩
bool FileHandler::isValid() const {
	string suffix1 = getFileSuffix(source_path);
	string suffix2 = getFileSuffix(target_path);

	if (suffix1 == "")	//	源文件是文件夹
		return false;
	if (suffix1 == ".myzip") {	//	源文件是压缩文件
		if (target_path == "")
			return true;
		ifstream ifs(source_path, ios::in);
		string path;
		ifs >> path;
		ifs.close();
		if (getFileSuffix(path) == getFileSuffix(target_path))
			return true;
	}
	else {							//	源文件是除文件夹和压缩文件之外的文件类型
		if (target_path == "")
			return true;
		if (getFileSuffix(target_path) == ".myzip")
			return true;
	}

	return false;
}

void FileHandler::compressFile() {

	if (target_path == "") {
		int pos = source_path.rfind('.');
		target_path = source_path.substr(0, pos);
		target_path += ".myzip";
	}

	//	1.读取源文件，统计字节值及其权重
	ifstream ifs(source_path, ios::in | ios::binary);
	ull weights[256] = { 0 };
	char temp;
	byte index;
	while (ifs.get(temp)) {
		index = temp;
		++weights[index];
	}
	ifs.close();
	//	2.字节值及对应权重放入优先对列，以备构造哈夫曼树
	priority_queue<HTNode*, vector<HTNode*>, CMP> pque;
	for (int i = 0; i < 256; ++i) {
		if (weights[i] != 0)
			pque.push(new HTNode(i, weights[i]));
	}
	//	3.构造哈夫曼树
	buildHaffumanTree(pque);
	//	4.��������������
	string haffumancode;
	string haffumancodes[256];
	generateHaffumanCode(root, haffumancode, haffumancodes);
	//	5.开始压缩文件
	//	（1）向目标文件写入源文件的文件名
	ofstream ofs(target_path, ios::out | ios::binary);
	ofs << getFileName(source_path) << endl;
	//  （2）向目标文件写入字节值及权重记录
	for (int i = 0; i < 256; ++i)
		ofs << weights[i] << " ";
	ofs << endl;
	//	（3）向目标文件写入哈夫曼编码的字节数
	ull num = 0;	//	哈夫曼编码的字节数
	ull rest = 0;	//	最后一个字节有效哈夫曼编码的位数
	for (int i = 0; i < 256; ++i) {		
		rest += weights[i] * haffumancodes[i].size();
		while (rest >= 8) {
			rest -= 8;
			++num;
		}
	}
	if (rest > 0)
		++num;
	ofs << num << " " << rest << endl;
	//	（4）向目标文件写入哈夫曼编码
	ifs.open(source_path, ios::in | ios::binary);
	haffumancode = "";
	while (ifs.get(temp)) {
		index = temp;
		haffumancode += haffumancodes[index];
		while (haffumancode.size() >= 8) {
			byte code = 0;
			for (int i = 0; i < 8; ++i) {
				code = (code << 1) | (haffumancode[i] - '0');
			}
			haffumancode = haffumancode.substr(8);
			ofs << code;
		}
	}

	//	处理剩余哈夫曼编码
	if (haffumancode.size() > 0) {
		byte code = 0;
		for (int i = 0; i < haffumancode.size(); ++i) {
			code = (code << 1) | (haffumancode[i] - '0');
		}
		rest = 8 - haffumancode.size();
		for (int i = 0; i < rest; ++i) {
			code <<= 1;
		}
		ofs << code;
	}

	ifs.close();
	ofs.close();
}

void FileHandler::decompressFile() {
	//	打开源文件
	ifstream ifs(source_path, ios::in | ios::binary);
	//	1.读取文件名
	string filename;
	ifs >> filename;
	if (target_path == "") {
		target_path = getFilePath(source_path);
		target_path += filename;
	}
	//	2.读取字节值对应的权重
	ull weights[256] = { 0 };
	for (int i = 0; i < 256; ++i) {
		ifs >> weights[i];
	}
	//	3.字节值及对应权重放入优先对列，以备构造哈夫曼树
	priority_queue<HTNode*, vector<HTNode*>, CMP> pque;
	for (int i = 0; i < 256; ++i) {
		if (weights[i] != 0)
			pque.push(new HTNode(i, weights[i]));
	}
	//	4.构造哈夫曼树
	buildHaffumanTree(pque);
	//	5.读取哈夫曼编码字节数
	ull num, rest;
	ifs >> num >> rest;
	//	6.根据哈夫曼编码还原字节值
	ofstream ofs(target_path, ios::out | ios::binary);
	char temp;
	byte code;
	ull counter = 0;
	HTNode* cur = root; //	遍历指针在哈夫曼树中的当前位置
	ifs.get(temp);	//	读取换行符
	while (ifs.get(temp)) {
		code = temp;
		++counter;
		if (num == counter) {
			for (int i = 0; i < rest; ++i) {
				byte path = code & (byte)128;
				code <<= 1;
				if (path == (byte)0)
					cur = cur->left;
				if (path == (byte)128)
					cur = cur->right;
				if (cur->left == nullptr && cur->right == nullptr) {
					ofs << cur->value;
					cur = root;
				}
			}
			break;
		}
		for (int i = 0; i < 8; ++i) {
			byte path = code & (byte)128;
			code <<= 1;
			if (path == (byte)0)
				cur = cur->left;
			if (path == (byte)128)
				cur = cur->right;
			if (cur->left == nullptr && cur->right == nullptr) {
				ofs << cur->value;
				cur = root;
			}
		}
	}

	ofs.close();
	ifs.close();
}