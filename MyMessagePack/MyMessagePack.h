#pragma once


#ifdef _WIN64
    #ifdef _MT
        #ifdef _DLL
            #ifdef _DEBUG
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x64_MDd.lib")
            #else
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x64_MD.lib")
            #endif
        #else
            #ifdef _DEBUG
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x64_MTd.lib")
            #else
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x64_MT.lib")
            #endif
        #endif
    #endif
#elif _WIN32
    #ifdef _MT
        #ifdef _DLL
            #ifdef _DEBUG
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x86_MDd.lib")
            #else
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x86_MD.lib")
            #endif
        #else
            #ifdef _DEBUG
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x86_MTd.lib")
            #else
                #pragma comment(lib, "../MyMessagePack/lib/msgpack_x86_MT.lib")
            #endif
        #endif
    #endif
#endif


#include "include/msgpack.hpp"


VOID WriteDwordToBuffer(PBYTE pbData, DWORD dwNum, DWORD dwPos);


/// <summary>
/// ��MsgData���л���
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="MsgData">�Զ����struct</param>
/// <param name="pOutBuffer">�����������ע��ʹ�����Ҫ�ֶ�delete���������</param>
/// <returns>����������Ĵ�С</returns>
template <class T>
DWORD MsgPack(T MsgData, PBYTE& pOutBuffer) {
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, MsgData);

    pOutBuffer = new BYTE[sbuffer.size()];
    memcpy(pOutBuffer, sbuffer.data(), sbuffer.size());

    return sbuffer.size();
}


/// <summary>
/// ��MsgData���л���
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="MsgData">�Զ����struct</param>
/// <param name="pOutBuffer">�����������ע��ʹ�����Ҫ�ֶ�delete���������</param>
/// <param name="dwFuncId">HOOK�ĺ�����ID��д�������������ǰ4���ֽڡ�Ȼ���������MsgPack������</param>
/// <returns>����������Ĵ�С</returns>
template <class T>
DWORD MsgPackWithFuncId(T MsgData, PBYTE& pOutBuffer, DWORD dwFuncId) {
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, MsgData);

    pOutBuffer = new BYTE[sbuffer.size() + 4];
    WriteDwordToBuffer(pOutBuffer, dwFuncId, 0);
    memcpy(pOutBuffer + 4, sbuffer.data(), sbuffer.size());

    return sbuffer.size() + 4;
}



/// <summary>
/// �����л���MsgData
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="pBuffer">��Ҫ���з����л������ݻ�����</param>
/// <param name="dwBufSize">���ݻ������Ĵ�С</param>
/// <returns>MsgData�����Զ������͵�struct</returns>
template <class T>
T MsgUnpack(PBYTE pBuffer, DWORD dwBufSize)
{
    // �����л�
    msgpack::unpacked  unpack;
    msgpack::unpack(&unpack, (char*)pBuffer, dwBufSize);

    msgpack::object obj = unpack.get();

    T MsgData;
    obj.convert(&MsgData);

    return MsgData;
}