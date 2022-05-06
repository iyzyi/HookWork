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
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(msgFilePath)
};

struct _Data_CreateFileW {
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(msgFilePath)
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

#pragma endregion