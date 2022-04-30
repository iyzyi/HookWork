#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"


VOID ParsePacket(PBYTE pBuffer, DWORD dwBufferSize) {

	if (dwBufferSize <= 4) {
		printf("接收到长度为%d的无效数据\n", dwBufferSize);
		return;
	}

	DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	PBYTE pMsgBuffer = pBuffer + 4;
	DWORD dwMsgBufferSize = dwBufferSize - 4;

	switch (dwFuncId)
	{
	case ID_recv: 
	{
		_Data_recv data = MsgUnpack<_Data_recv>(pMsgBuffer, dwMsgBufferSize);
		printf("recv socket = 0x%p:\n", data.socket);
		PrintData((LPBYTE)data.sbuffer.ptr, data.sbuffer.size);
		break;
	}
	case ID_send:
	{
		_Data_send data = MsgUnpack<_Data_send>(pMsgBuffer, dwMsgBufferSize);
		printf("send socketB = 0x%p:\n", data.socket);
		PrintData((LPBYTE)data.sbuffer.ptr, data.sbuffer.size);
		break;
	}

	default:
		break;
	}

	
}