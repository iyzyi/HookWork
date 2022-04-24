#include "pch.h"
#include "DataLog.h"

//#pragma warning(disable : 4996)


CDataLog::CDataLog(PCHAR szLogFilePath) {
	strcpy(this->szLogFilePath, szLogFilePath);
	//bBuffer = new BYTE[BUFFER_MAX_LENGTH];			// 千万不要写成 new BYTE(BUFFER_MAX_LENGTH);这样写会crash.
	InitializeCriticalSection(&m_cs);
}

CDataLog::~CDataLog() {
	CloseLogFile();
	//delete[] bBuffer;
	DeleteCriticalSection(&m_cs);
}


VOID CDataLog::OpenLogFile() {
	if (fpLog == NULL) {
		EnterCriticalSection(&m_cs);
		fpLog = _fsopen(szLogFilePath, "a+b", _SH_DENYNO);
	}
}


VOID CDataLog::CloseLogFile() {
	if (fpLog != NULL) {
		fclose(fpLog);
		LeaveCriticalSection(&m_cs);
	}
	fpLog = NULL;
}


// piblic的log函数需要OpenLogFile()和CloseLogFile()

VOID CDataLog::LogString(PCHAR szData) {
	OpenLogFile();
	__LogString(szData);
	CloseLogFile();
}


VOID CDataLog::LogFormatString(DWORD dwDataLen, PCHAR format, ...) {
	OpenLogFile();

	PCHAR szData = new CHAR[dwDataLen + 16];

	va_list args;
	va_start(args, format);
	vsprintf(szData, format, args);
	va_end(args);

	__LogString(szData);

	CloseLogFile();
	delete[] szData;
}


VOID CDataLog::LogHexData(PCHAR szPreString, PBYTE pbData, DWORD dwDataLen) {
	OpenLogFile();

	__LogString(szPreString);

	DWORD dwRow = 0, dwColumn = 0;
	CHAR szTemp[16] = { 0 };
	for (dwRow = 0; dwRow < dwDataLen / 16 + 1; dwRow++) {
		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			sprintf((PCHAR)szTemp, "0x%02x ", pbData[dwRow * 16 + dwColumn]);
			__LogString((PCHAR)szTemp);
		}

		if (dwColumn != 16) {
			while (dwColumn < 16) {
				__LogString("     ");
				dwColumn++;
			}
		}
		__LogString("\t");

		for (dwColumn = 0; (dwRow * 16 + dwColumn < dwDataLen) && (dwColumn < 16); dwColumn++) {
			DWORD dwIndex = dwRow * 16 + dwColumn;
			if (pbData[dwIndex] >= 32 && pbData[dwIndex] <= 126) {
				__LogChar(pbData[dwIndex]);
			}
			else {
				__LogString(".");
			}
		}
		__LogString("\n");
	}

	CloseLogFile();
}



// private的log函数不能OpenLogFile()和CloseLogFile()

VOID CDataLog::__LogString(PCHAR szData) {
	fwrite(szData, strlen(szData), 1, fpLog);
}


VOID CDataLog::__LogChar(CHAR cData) {
	fwrite(&cData, 1, 1, fpLog);
}
