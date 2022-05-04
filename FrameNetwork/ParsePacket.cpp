#include "pch.h"

#include "../MyMessagePack/MyMessagePack.h"
#include "../InjectDll/EnumFunction.h"
#include "Misc.h"
#include "ListNetworkData.h"
#include "FrameNetworkDlg.h"



VOID ParsePacket(CFrameNetworkDlg* pMainDlg, PBYTE pBuffer, DWORD dwBufferSize) {
	if (pMainDlg == NULL)
		return;
	
	CListNetworkData* pListData = pMainDlg->m_pListData;
	
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
		pListData->AddRow(pMainDlg->m_dwIndex, _T("recv"), data.socket, data.dwIP, data.wPort, data.sbuffer.size, (PBYTE)data.sbuffer.ptr);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_send:
	{
		_Data_send data = MsgUnpack<_Data_send>(pMsgBuffer, dwMsgBufferSize);
		pListData->AddRow(pMainDlg->m_dwIndex, _T("send"), data.socket, data.dwIP, data.wPort, data.sbuffer.size, (PBYTE)data.sbuffer.ptr);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_sendto:
	{
		_Data_sendto data = MsgUnpack<_Data_sendto>(pMsgBuffer, dwMsgBufferSize);
		pListData->AddRow(pMainDlg->m_dwIndex, _T("sendto"), data.socket, data.dwIP, data.wPort, data.sbuffer.size, (PBYTE)data.sbuffer.ptr);
		pMainDlg->m_dwIndex++;
		break;
	}

	case ID_recvfrom:
	{
		_Data_recvfrom data = MsgUnpack<_Data_recvfrom>(pMsgBuffer, dwMsgBufferSize);
		pListData->AddRow(pMainDlg->m_dwIndex, _T("recvfrom"), data.socket, data.dwIP, data.wPort, data.sbuffer.size, (PBYTE)data.sbuffer.ptr);
		pMainDlg->m_dwIndex++;
		break;
	}

	default:
		break;
	}
}