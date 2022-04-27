#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"

#include "../InjectDll/EnumFunction.h"

#include "Misc.h"



struct _Data_WSASend {
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(sbuffer)
};

VOID ParsePacket(PBYTE pBuffer, DWORD dwBufferSize) {

	printf((char*)pBuffer);
	printf("\n");

	//PrintData(pBuffer, dwBufferSize);

	//if (dwBufferSize <= 4) {
	//	printf("接收到长度为%d的无效数据\n", dwBufferSize);
	//	return;
	//}

	//DWORD dwFuncId = GetDwordFromBuffer(pBuffer, 0);
	//PBYTE pMsgBuffer = pBuffer + 4;

	//switch (dwFuncId)
	//{
	//case ID_WSASend: 
	//{
	//	_Data_WSASend data = MsgUnpack<_Data_WSASend>(pMsgBuffer, dwBufferSize);
	//	PrintData((LPBYTE)data.sbuffer.ptr, data.sbuffer.size);
	//	break;
	//}

	//default:
	//	break;
	//}

	
}