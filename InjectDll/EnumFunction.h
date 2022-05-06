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
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(msgFilePath)
};

struct _Data_CreateFileW {
	msgpack::type::raw_ref		msgFilePath;
	MSGPACK_DEFINE(msgFilePath)
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

#pragma endregion