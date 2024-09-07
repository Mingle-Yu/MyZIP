#include "FileHandler.h"
#include <iostream>

int main(int argc, char* argv[]) {
	//	argv存储元素说明：argv[0]：运行程序的路径 argv[1]：运行程序名称
	//	argv[2]：源文件路径 + 源文件名 argv[3]：目标文件路径 + 目标问件名
	//	1.检查命令行参数是否合法
	if (argc <= 2 || argc >= 5) {
		if (argc <= 2)
			cout << "缺少命令行参数" << endl;
		if (argc >= 5)
			cout << "命令行参数冗余" << endl;
		return 1;
	}
	//	2.检查源文件是否存在
	ifstream ifs(argv[2], ios::in);
	if (!ifs.is_open()) {
		cout << argv[2] << "打开失败" << endl;
		return 2;
	}
	//	3.检查文件转换是否合法
	string source_path = argv[2];
	string target_path = argc == 4 ? argv[3] : "";
	FileHandler flhd(source_path, target_path);
	if (!flhd.isValid()) {
		cout << "文件压缩/解压失败" << endl;
		return 3;
	}
	//	4.文件压缩和解压
	if (flhd.getSourceFileSuffix() != ".myzip")
		flhd.compressFile();
	else
		flhd.decompressFile();

	return 0;
}