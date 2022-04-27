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
/// 将MsgData序列化。
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="MsgData">自定义的struct</param>
/// <param name="pOutBuffer">输出缓冲区。注意使用完后要手动delete这个缓冲区</param>
/// <returns>输出缓冲区的大小</returns>
template <class T>
DWORD MsgPack(T MsgData, PBYTE& pOutBuffer) {
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, MsgData);

    pOutBuffer = new BYTE[sbuffer.size()];
    memcpy(pOutBuffer, sbuffer.data(), sbuffer.size());

    return sbuffer.size();
}


/// <summary>
/// 将MsgData序列化。
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="MsgData">自定义的struct</param>
/// <param name="pOutBuffer">输出缓冲区。注意使用完后要手动delete这个缓冲区</param>
/// <param name="dwFuncId">HOOK的函数的ID，写在输出缓冲区的前4个字节。然后往后才是MsgPack的数据</param>
/// <returns>输出缓冲区的大小</returns>
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
/// 反序列化成MsgData
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="pBuffer">将要进行反序列化的数据缓冲区</param>
/// <param name="dwBufSize">数据缓冲区的大小</param>
/// <returns>MsgData，是自定义类型的struct</returns>
template <class T>
T MsgUnpack(PBYTE pBuffer, DWORD dwBufSize)
{
    // 反序列化
    msgpack::unpacked  unpack;
    msgpack::unpack(&unpack, (char*)pBuffer, dwBufSize);

    msgpack::object obj = unpack.get();

    T MsgData;
    obj.convert(&MsgData);

    return MsgData;
}