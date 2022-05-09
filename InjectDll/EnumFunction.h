#pragma once
#include "../MyMessagePack/MyMessagePack.h"
#include <string>

enum {

// ����ͨ����غ���
	ID_send,
	ID_sendto,
	ID_WSASend,
	ID_WSASendTo,
	ID_WSASendMsg,
	ID_recv,
	ID_recvfrom,
	ID_WSARecv,
	ID_WSARecvFrom,

// �ļ�ϵͳ��غ���
	ID_CreateFileA,
	ID_CreateFileW,
	ID_ReadFile,
	ID_ReadFileEx,
	ID_WriteFile,
	ID_WriteFileEx,
	ID_CreateDirectoryA,
	ID_CreateDirectoryW,

// ע�����غ���
	ID_RegCreateKeyExA,
	ID_RegCreateKeyExW,
	ID_RegOpenKeyExA,
	ID_RegOpenKeyExW,
	ID_RegDeleteKeyExA,
	ID_RegDeleteKeyExW,
	ID_RegCloseKey,
	ID_RegSetValueExA,
	ID_RegSetValueExW,
	//ID_RegQueryValueA,
	//ID_RegQueryValueW,
	ID_RegQueryValueExA,
	ID_RegQueryValueExW,
	ID_RegGetValueA,
	ID_RegGetValueW,
	ID_RegEnumKeyExA,
	ID_RegEnumKeyExW,
	//ID_RegEnumValueA,
	//ID_RegEnumValueW,

// �����߳���غ���
	

	ID_CreateProcessA,
	ID_CreateProcessW,
	ID_CreateProcessAsUserA,
	ID_CreateProcessAsUserW,
	ID_CreateThread,
	ID_CreateRemoteThread,
	ID_CreateRemoteThreadEx,
	ID_ExitProcess,
	ID_ExitThread,
};




#pragma region ����ͨ����غ���-���ݽṹ

// **************** ����ͨ����غ���-���ݽṹ ****************
struct _Data_recv {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};

struct _Data_recvfrom {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};


struct _Data_send {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};


struct _Data_sendto {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};

struct _Data_WSASend {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};

struct _Data_WSARecv {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};
#pragma endregion



#pragma region �ļ�ϵͳ��غ���-���ݽṹ

// **************** �ļ�ϵͳ��غ���-���ݽṹ ****************

struct _Data_CreateFileA {
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(msgFilePath)
};

struct _Data_CreateFileW {
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(dwFileHandle, msgFilePath)
};

struct _Data_ReadFile {
	// HANDLE����û������ӦMSGPACK_DEFINE���ᱨ��
	// ���ֲ��밴������Ľ̳����Լ������䣬��ֱ�ӰѾ����DWORD����ʾ��
	// https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_adaptor 
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(dwFileHandle, msgFilePath, sbuffer)
};

struct _Data_ReadFileEx {
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(dwFileHandle, msgFilePath, sbuffer)
};

struct _Data_WriteFile {
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(dwFileHandle, msgFilePath, sbuffer)
};

struct _Data_WriteFileEx {
	DWORD						dwFileHandle;
	msgpack::type::raw_ref		msgFilePath;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(dwFileHandle, msgFilePath, sbuffer)
};

struct _Data_CreateDirectoryA {
	msgpack::type::raw_ref		msgPathName;
	MSGPACK_DEFINE(msgPathName)
};

struct _Data_CreateDirectoryW {
	msgpack::type::raw_ref		msgPathName;
	MSGPACK_DEFINE(msgPathName)
};

#pragma endregion


#pragma region ע�����غ���-���ݽṹ

// **************** ע�����غ���-���ݽṹ ****************

struct _Data_RegCreateKeyExW {
	UINT_PTR					upKeyHandle;		// x64����64bit, x86����32bit������MsgPack���ƣ�������HANDLE���õĻ����Լ�дconvert()����ֱ����UINT_PTR͵���ˡ�
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet)
};

struct _Data_RegOpenKeyExW {
	UINT_PTR					upKeyHandle;	
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet)
};

struct _Data_RegDeleteKeyExW {
	UINT_PTR					upKeyHandle;
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet)
};

struct _Data_RegCloseKey {
	UINT_PTR					upKeyHandle;
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet)
};

struct _Data_RegSetValueExW {
	UINT_PTR					upKeyHandle;
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	msgpack::type::raw_ref		msgValueName;
	DWORD						dwType;
	msgpack::type::raw_ref		msgData;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet, msgValueName, dwType, msgData)
};

struct _Data_RegQueryValueExW {
	UINT_PTR					upKeyHandle;
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	msgpack::type::raw_ref		msgValueName;
	DWORD						dwType;
	msgpack::type::raw_ref		msgData;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet, msgValueName, dwType, msgData)
};

struct _Data_RegGetValueW {
	UINT_PTR					upKeyHandle;
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	msgpack::type::raw_ref		msgValueName;
	DWORD						dwType;
	msgpack::type::raw_ref		msgData;
	MSGPACK_DEFINE(upKeyHandle, msgPath, dwRet, msgValueName, dwType, msgData)
};

#pragma endregion



struct _Data_CreateProcessW {
	msgpack::type::raw_ref		msgAppName;
	msgpack::type::raw_ref		msgCmdLine;
	msgpack::type::raw_ref		msgCurrentDir;
	DWORD						dwErrorCode;
	MSGPACK_DEFINE(msgAppName, msgCmdLine, msgCurrentDir, dwErrorCode)
};


struct _Data_CreateThread {
	UINT_PTR					upThreadHandle;
	UINT_PTR					upStartAddress;
	UINT_PTR					upParameterAddress;
	UINT_PTR					upThreadId;
	DWORD						dwErrorCode;
	MSGPACK_DEFINE(upThreadHandle, upStartAddress, upParameterAddress, upThreadId, dwErrorCode)
};


struct _Data_CreateRemoteThread {
	UINT_PTR					upThreadHandle;
	UINT_PTR					upProcessHandle;
	UINT_PTR					upStartAddress;
	UINT_PTR					upParameterAddress;
	UINT_PTR					upThreadId;
	DWORD						dwErrorCode;
	MSGPACK_DEFINE(upThreadHandle, upProcessHandle, upStartAddress, upParameterAddress, upThreadId, dwErrorCode)
};


struct _Data_CreateRemoteThreadEx {
	UINT_PTR					upThreadHandle;
	UINT_PTR					upProcessHandle;
	UINT_PTR					upStartAddress;
	UINT_PTR					upParameterAddress;
	UINT_PTR					upThreadId;
	DWORD						dwErrorCode;
	MSGPACK_DEFINE(upThreadHandle, upProcessHandle, upStartAddress, upParameterAddress, upThreadId, dwErrorCode)
};


struct _Data_ExitProcess {
	UINT_PTR					upExitCode;
	MSGPACK_DEFINE(upExitCode)
};


struct _Data_ExitThread {
	UINT_PTR					upExitCode;
	MSGPACK_DEFINE(upExitCode)
};