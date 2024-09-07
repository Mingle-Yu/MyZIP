#pragma once
typedef unsigned char byte;
typedef unsigned long long ull;

struct HTNode {
	byte value;	//	字节值
	ull weight;	//	权重值
	HTNode* left, * right;
	HTNode(const byte& val = 0, const ull& wei = 0) {
		value = val;
		weight = wei;
		left = nullptr;
		right = nullptr;
	}
};