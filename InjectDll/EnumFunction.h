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
	ID_RegOpenKeyExW,

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
struct _Data_RegOpenKeyExW {
	UINT_PTR					upFileHandle;		// x64下是64bit, x86下是32bit。由于MsgPack限制，不能用HANDLE，用的话得自己写convert()。我直接用UINT_PTR偷懒了。
	msgpack::type::raw_ref		msgPath;
	DWORD						dwRet;
	MSGPACK_DEFINE(upFileHandle, msgPath, dwRet)
};


#pragma endregion