#pragma once


//void Length();


/// <summary>
/// ����inline hookʱָ����м��������ͷ����ָ��ĳ��ȡ�
/// </summary>
/// <param name="FuncAddr">������ַ</param>
/// <param name="Architecture">X86�ܹ�����0��X64�ܹ�����64</param>
/// <param name="LeastLength">���Ǻ�����ͷ��ָ�����NOPָ�����С����</param>
/// <returns></returns>
int LengthDisasm(void* FuncAddr, int Architecture, int LeastLength);