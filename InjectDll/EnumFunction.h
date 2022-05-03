#pragma once
#include "../MyMessagePack/MyMessagePack.h"
#include <string>

enum {
	ID_send,
	ID_sendto,
	ID_WSASend,
	ID_WSASendTo,
	ID_WSASendMsg,

	ID_recv,
	ID_recvfrom,
	ID_WSARecv,
	ID_WSARecvFrom
};



struct _Data_recv {
	SOCKET						socket;
	DWORD						dwIP;
	WORD						wPort;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, dwIP, wPort, sbuffer)
};


struct _Data_send {
	SOCKET						socket;
	msgpack::type::raw_ref		sbuffer;
	MSGPACK_DEFINE(socket, sbuffer)
};