#pragma once


//void Length();


/// <summary>
/// 计算inline hook时指令碎屑（函数开头几条指令）的长度。
/// </summary>
/// <param name="FuncAddr">函数地址</param>
/// <param name="Architecture">X86架构传入0，X64架构传入64</param>
/// <param name="LeastLength">覆盖函数开头的指令（不含NOP指令）的最小长度</param>
/// <returns></returns>
int LengthDisasm(void* FuncAddr, int Architecture, int LeastLength);