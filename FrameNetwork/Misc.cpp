#include "pch.h"
#include "misc.h"

#define DebugPrint printf


VOID RandomBytes(LPBYTE pbData, DWORD dwDataLen) {
	std::random_device rd;						// ����һ�� std::random_device ���� rd
	std::default_random_engine random(rd());	// �� rd ��ʼ��һ������������� random

	for (DWORD i = 0; i < dwDataLen; i += 4) {
		DWORD dwRandNum = random();
		for (DWORD j = 0; j < 4; j++) {
			if (i + j >= dwDataLen) {
				return;
			}
			pbData[i + j] = dwRandNum & 0xff;
			dwRandNum >>= 8;
		}
	}
}


VOID PrintBytes(LPBYTE pbPrintData, DWORD dwDataLen)
{
	for (DWORD dwCount = 0; dwCount < dwDataLen; dwCount++) {
		DebugPrint("0x%02x ", pbPrintData[dwCount]);
		if (0 == (dwCount + 1) % 0x10) {
			DebugPrint("\n");
		}
	}
	DebugPrint("\n");
}


VOID PrintChars(CHAR* pbPrintData, DWORD dwDataLen) {
	for (DWORD dwCount = 0; dwCount < dwDataLen; dwCount++) {
		DebugPrint("%c", pbPrintData[dwCount]);
	}
	DebugPrint("\n");
}


VOID PrintData(LPBYTE pbPrintData, DWORD dwDataLen)
{
	if (dwDataLen > 512) {
		DebugPrint("��%d�ֽڣ�����Ч�ʿ��ǣ�����ӡǰ512�ֽ�\n", dwDataLen);
		dwDataLen = 512;
	}

	DWORD dwRow = 0, dwColumn = 0;
	for (dwRow = 0; dwRow < dwDataLen / 16 + 1; dwRow++) {
		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			DebugPrint("0x%02x ", pbPrintData[dwRow * 16 + dwColumn]);
		}

		if (dwColumn != 16) {
			while (dwColumn < 16) {
				DebugPrint("     ");
				dwColumn++;
			}
		}
		DebugPrint("\t");

		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			DWORD dwIndex = dwRow * 16 + dwColumn;
			if (pbPrintData[dwIndex] >= 32 && pbPrintData[dwIndex] <= 126) {
				DebugPrint("%c", pbPrintData[dwIndex]);
			}
			else {
				DebugPrint(".");
			}
		}
		DebugPrint("\n");
	}
	DebugPrint("\n");
}


BOOL IsLittleEndding() {
	int i = 1;
	char c = *(char*)&i;
	return 1 ? true : false;
}


QWORD GetQwordFromBuffer(PBYTE pbData, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	return pbData2[0] + (pbData2[1] << 8) + (pbData2[2] << 16) + (pbData2[3] << 24) + (pbData2[4] << 32) + (pbData2[5] << 40) + (pbData2[6] << 48) + (pbData2[7] << 56);
}


// ��buffer��ƫ��dwPos��ȡ��һ��DWORD����ʱĬ��С�˴洢���Ժ������ưɡ�
DWORD GetDwordFromBuffer(PBYTE pbData, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	return pbData2[0] + (pbData2[1] << 8) + (pbData2[2] << 16) + (pbData2[3] << 24);
}


WORD GetWordFromBuffer(PBYTE pbData, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	return pbData2[0] + (pbData2[1] << 8);
}

BYTE GetByteFromBuffer(PBYTE pbData, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	return pbData2[0];
}


VOID WriteQwordToBuffer(PBYTE pbData, QWORD qwNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = qwNum & 0xff;
	pbData2[1] = (qwNum >> 8) & 0xff;
	pbData2[2] = (qwNum >> 16) & 0xff;
	pbData2[3] = (qwNum >> 24) & 0xff;
	pbData2[4] = (qwNum >> 32) & 0xff;
	pbData2[5] = (qwNum >> 40) & 0xff;
	pbData2[6] = (qwNum >> 48) & 0xff;
	pbData2[7] = (qwNum >> 56) & 0xff;
}



VOID WriteDwordToBuffer(PBYTE pbData, DWORD dwNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = dwNum & 0xff;
	pbData2[1] = (dwNum >> 8) & 0xff;
	pbData2[2] = (dwNum >> 16) & 0xff;
	pbData2[3] = (dwNum >> 24) & 0xff;
}


VOID WriteWordToBuffer(PBYTE pbData, WORD wNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = wNum & 0xff;
	pbData2[1] = (wNum >> 8) & 0xff;
}


VOID WriteByteToBuffer(PBYTE pbData, BYTE byNum, DWORD dwPos) {
	PBYTE pbData2 = pbData + dwPos;
	pbData2[0] = byNum;
}


CString ByteArray2HexCString(PBYTE pbData, DWORD dwDataBufLen) {
	CString csText = _T("");
	CString csTemp = _T("");
	for (int i = 0; i < dwDataBufLen; i++) {
		csTemp.Format(_T("%.2X "), (BYTE)(pbData[i]));
		csText += csTemp;
	}
	return csText;
}


// �����ʮ���������ݣ��Ҳ���������ݵĶ�Ӧ�ַ�
CString ByteArray2HexAndInfoCString(PBYTE pbData, DWORD dwDataBufLen, DWORD dwCharNumPerRow) {
	CString csText = _T("");
	CString csTemp = _T("");

	DWORD dwRow = 0, dwColumn = 0;
	for (dwRow = 0; dwRow < dwDataBufLen / dwCharNumPerRow + 1; dwRow++) {
		csTemp.Format(_T("0x%02x\t"), dwRow * dwCharNumPerRow);
		csText += csTemp;

		for (dwColumn = 0; (dwRow * dwCharNumPerRow + dwColumn < dwDataBufLen) && (dwColumn < dwCharNumPerRow); dwColumn++) {
			csTemp.Format(_T("%04x "), pbData[dwRow * dwCharNumPerRow + dwColumn]);
			csText += csTemp;
		}

		if (dwColumn != dwCharNumPerRow) {
			while (dwColumn < dwCharNumPerRow) {
				csText += _T("     ");
				dwColumn++;
			}
		}
		csText += _T("\t");

		for (dwColumn = 0; (dwRow * dwCharNumPerRow + dwColumn < dwDataBufLen) && (dwColumn < dwCharNumPerRow); dwColumn++) {
			DWORD dwIndex = dwRow * dwCharNumPerRow + dwColumn;
			if (pbData[dwIndex] >= 32 && pbData[dwIndex] <= 126) {
				csTemp.Format(_T("%c"), pbData[dwIndex]);
				csText += csTemp;
			}
			else {
				csText += _T(".");
			}
		}
		csText += _T("\r\n");
	}
	//csText += _T("\r\n");

	return csText;
}