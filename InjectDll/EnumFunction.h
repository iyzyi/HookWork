#pragma once
#include "../MyMessagePack/MyMessagePack.h"
#include <string>

enum {

// 网络通信相关函数
	ID_send,
	ID_sendto,
	ID_WSASend,
	ID_WSASendTo,
	ID_WSASendMsg,
	ID_recv,
	ID_recvfrom,
	ID_WSARecv,
	ID_WSARecvFrom,

// 文件系统相关函数
	ID_CreateFileA,
	ID_CreateFileW,
	ID_ReadFile,
	ID_ReadFileEx,
	ID_WriteFile,
	ID_WriteFileEx,
	ID_CreateDirectoryA,
	ID_CreateDirectoryW,

// 注册表相关函数
	ID_RegCreateKeyExA,
	ID_RegCreateKeyExW,
	ID_RegOpenKeyExA,
	ID_RegOpenKeyExW,
	ID_RegDeleteKeyExA,
	ID_RegDeleteKeyExW,
	ID_RegCloseKey,
	ID_RegSetValueExA,
	ID_RegSetValueExW,
	ID_RegQueryValueA,
	ID_RegQueryValueW,
	ID_RegGetValueA,
	ID_RegGetValueW,
	ID_RegEnumKeyExA,
	ID_RegEnumKeyExW,
	ID_RegEnumValueA,
	ID_RegEnumValueW,
};


#pragma region 网络通信相关函数-数据结构

// **************** 网络通信相关函数-数据结构 ****************
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



#pragma region 文件系统相关函数-数据结构

// **************** 文件系统相关函数-数据结构 ****************

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
	// HANDLE类型没法自适应MSGPACK_DEFINE，会报错。
	// 我又不想按照下面的教程来自己做适配，就直接把句柄用DWORD来表示了
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


#pragma region 文件系统相关函数-数据结构

// **************** 文件系统相关函数-数据结构 ****************

struct _Data_RegCreateKeyExW {
	UINT_PTR					upKeyHandle;		// x64下是64bit, x86下是32bit。由于MsgPack限制，不能用HANDLE，用的话得自己写convert()。我直接用UINT_PTR偷懒了。
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


#pragma endregion