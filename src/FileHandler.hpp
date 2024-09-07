#pragma once
#include "HTNode.hpp"
#include <string>
#include <queue>
#include <fstream>
using namespace std;

class FileHandler {
private:
	class CMP {
	public:
		bool operator()(const HTNode* a, const HTNode* b) {
			return a->weight > b->weight;
		}
	};

	HTNode* root;
	string source_path;	//	源文件路径+文件名
	string target_path;	//	目标文件路径+目标文件名

private:
	void buildHaffumanTree(priority_queue<HTNode*, vector<HTNode*>, CMP>& pque);	//	构建哈夫曼树

	void destroyHaffumanTree(HTNode* root);	//	销毁哈夫曼树

	void generateHaffumanCode(HTNode* root, string& code, string haffumancodes[]);	//	生成哈夫曼编码

	string getFileName(const string& path) const;	//	获取文件名

	string getFileSuffix(const string& path) const;	//	获取文件后缀名

	string getFilePath(const string& path);	//	获取文件路径

public:
	FileHandler(const string& source, const string& target) {
		root = nullptr;
		source_path = source;
		target_path = target;
	}

	~FileHandler() { destroyHaffumanTree(root); }

	bool isValid() const;		//	判断文件转换是否合法

	string getSourceFileSuffix() const { return getFileSuffix(source_path); }	//	获取源文件后缀名
	
	void compressFile();		//	压缩文件

	void decompressFile();		//	解压文件
};