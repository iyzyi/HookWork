// 模板函数的实现必须放在头文件中
// 原因：https://blog.csdn.net/sinat_27382047/article/details/107739562


#include <windows.h>

VOID WriteDwordToBuffer(PBYTE pbData, DWORD dwNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = dwNum & 0xff;
	pbData2[1] = (dwNum >> 8) & 0xff;
	pbData2[2] = (dwNum >> 16) & 0xff;
	pbData2[3] = (dwNum >> 24) & 0xff;
}