#include "pch.h"
#include "Function.h"
#include "FrameKeyboardMouse.h"
#include "FrameKeyboardMouseDlg.h"

#pragma  comment(lib, "user32")
#pragma  comment(lib, "advapi32")
#include <windows.h>
#include <fstream>
#include <ctime>fs
#include <csignal>
#include <chrono>


class CFrameKeyboardMouseDlg;


CFrameKeyboardMouseDlg* g_pMainWnd = NULL;

CFrameKeyboardMouseDlg* MainWnd() {
	if (g_pMainWnd == NULL) {
		g_pMainWnd = ((CFrameKeyboardMouseDlg*)(theApp.m_pMainWnd));
	}
	return g_pMainWnd;
}


// 返回毫秒世界戳
DWORD GetTimeStamp() {
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);
	return ms.count();
}


CString csBuffer = CString(_T(""));
BOOL bNewRowFirstChar = true;

BOOL bLastKeyIsCombinationKey = false;		// 上一个按键为ctrl alt win (TODO 以后再考虑shift 和 fn 和 三键组合键)
std::string sLastKey = "";
DWORD dwLastKeyTime = 0;

void Display(std::string data) {

	DWORD dwTime = GetTimeStamp();

	if (bLastKeyIsCombinationKey) {
		if (dwTime - dwLastKeyTime < 500) {
			if (data.length() == 1 && data[0] >= 32 && data[0] <= 126) {		// 若这一次的按键与上一次的按键构成组合键
				csBuffer = CString(CA2T(sLastKey.c_str()));
				csBuffer += _T(" + ");
				csBuffer += CString(CA2T(data.c_str()));
				MainWnd()->AddRow(dwLastKeyTime, csBuffer);

				csBuffer = CString(_T(""));
				bNewRowFirstChar = true;
				bLastKeyIsCombinationKey = false;
				sLastKey = "";
				dwLastKeyTime = 0;
				return;
			}
			else {
				MainWnd()->AddRow(dwLastKeyTime, CString(CA2T(sLastKey.c_str())));
			}
		}
		else {
			MainWnd()->AddRow(dwLastKeyTime, CString(CA2T(sLastKey.c_str())));
		}
	}
	
	// 处理这一次的按键
	if (data.length() == 1 && data[0] >= 32 && data[0] <= 126) {				// 可见字符
		csBuffer += CString(CA2T(data.c_str()));

		if (bNewRowFirstChar) {
			MainWnd()->AddRow(dwTime, csBuffer);
			bNewRowFirstChar = false;
		}
		else {
			MainWnd()->ChangeLastRow(csBuffer);
		}
	}
	else {																		// 不可见字符
		if (data == "[ALT]" || data == "[CTRL]" || data == "[WIN KEY]") {		// 若是alt ctrl win
			bLastKeyIsCombinationKey = true;
			sLastKey = data;
			dwLastKeyTime = dwTime;
		}
		else {																	// 若不是alt ctrl win
			bLastKeyIsCombinationKey = false;
			sLastKey = "";
			dwLastKeyTime = 0;

			MainWnd()->AddRow(dwTime, CString(CA2T(data.c_str())));

			csBuffer = CString(_T(""));
			bNewRowFirstChar = true;
		}
	}
}



bool capital = false, numLock = false, shift = false;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		PKBDLLHOOKSTRUCT keystroke = (PKBDLLHOOKSTRUCT)lParam;
		if (keystroke->vkCode == VK_LSHIFT || keystroke->vkCode == VK_RSHIFT) {
			shift = wParam == WM_KEYDOWN ? true : false;
		}
		else if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN) {
			switch (keystroke->vkCode) {
			case 0x41: { Display(capital ? (shift ? "a" : "A") : (shift ? "A" : "a")); break; }
			case 0x42: { Display(capital ? (shift ? "b" : "B") : (shift ? "B" : "b")); break; }
			case 0x43: { Display(capital ? (shift ? "c" : "C") : (shift ? "C" : "c")); break; }
			case 0x44: { Display(capital ? (shift ? "d" : "D") : (shift ? "D" : "d")); break; }
			case 0x45: { Display(capital ? (shift ? "e" : "E") : (shift ? "E" : "e")); break; }
			case 0x46: { Display(capital ? (shift ? "f" : "F") : (shift ? "F" : "f")); break; }
			case 0x47: { Display(capital ? (shift ? "g" : "G") : (shift ? "G" : "g")); break; }
			case 0x48: { Display(capital ? (shift ? "h" : "H") : (shift ? "H" : "h")); break; }
			case 0x49: { Display(capital ? (shift ? "i" : "I") : (shift ? "I" : "i")); break; }
			case 0x4A: { Display(capital ? (shift ? "j" : "J") : (shift ? "J" : "j")); break; }
			case 0x4B: { Display(capital ? (shift ? "k" : "K") : (shift ? "K" : "k")); break; }
			case 0x4C: { Display(capital ? (shift ? "l" : "L") : (shift ? "L" : "l")); break; }
			case 0x4D: { Display(capital ? (shift ? "m" : "M") : (shift ? "M" : "m")); break; }
			case 0x4E: { Display(capital ? (shift ? "n" : "N") : (shift ? "N" : "n")); break; }
			case 0x4F: { Display(capital ? (shift ? "o" : "O") : (shift ? "O" : "o")); break; }
			case 0x50: { Display(capital ? (shift ? "p" : "P") : (shift ? "P" : "p")); break; }
			case 0x51: { Display(capital ? (shift ? "q" : "Q") : (shift ? "Q" : "q")); break; }
			case 0x52: { Display(capital ? (shift ? "r" : "R") : (shift ? "R" : "r")); break; }
			case 0x53: { Display(capital ? (shift ? "s" : "S") : (shift ? "S" : "s")); break; }
			case 0x54: { Display(capital ? (shift ? "t" : "T") : (shift ? "T" : "t")); break; }
			case 0x55: { Display(capital ? (shift ? "u" : "U") : (shift ? "U" : "u")); break; }
			case 0x56: { Display(capital ? (shift ? "v" : "V") : (shift ? "V" : "v")); break; }
			case 0x57: { Display(capital ? (shift ? "w" : "W") : (shift ? "W" : "w")); break; }
			case 0x58: { Display(capital ? (shift ? "x" : "X") : (shift ? "X" : "x")); break; }
			case 0x59: { Display(capital ? (shift ? "y" : "Y") : (shift ? "Y" : "y")); break; }
			case 0x5A: { Display(capital ? (shift ? "z" : "Z") : (shift ? "Z" : "z")); break; }
			case 0x30: { Display(shift ? ")" : "0"); break; }
			case 0x31: { Display(shift ? "!" : "1"); break; }
			case 0x32: { Display(shift ? "@" : "2"); break; }
			case 0x33: { Display(shift ? "#" : "3"); break; }
			case 0x34: { Display(shift ? "$" : "4"); break; }
			case 0x35: { Display(shift ? "%" : "5"); break; }
			case 0x36: { Display(shift ? "^" : "6"); break; }
			case 0x37: { Display(shift ? "&" : "7"); break; }
			case 0x38: { Display(shift ? "*" : "8"); break; }
			case 0x39: { Display(shift ? "(" : "9"); break; }
			case VK_OEM_1: { Display(shift ? ":" : ";"); break; }
			case VK_OEM_2: { Display(shift ? "?" : "/"); break; }
			case VK_OEM_3: { Display(shift ? "~" : "`"); break; }
			case VK_OEM_4: { Display(shift ? "{" : "["); break; }
			case VK_OEM_5: { Display(shift ? "|" : "\\"); break; }
			case VK_OEM_6: { Display(shift ? "}" : "]"); break; }
			case VK_OEM_7: { Display(shift ? "\"" : "'"); break; }
			case VK_OEM_PLUS: { Display(shift ? "+" : "="); break; }
			case VK_OEM_COMMA: { Display(shift ? "<" : ","); break; }
			case VK_OEM_MINUS: { Display(shift ? "_" : "-"); break; }
			case VK_OEM_PERIOD: { Display(shift ? ">" : "."); break; }
			case VK_SPACE: { Display(" "); break; }
			case VK_NUMPAD0: { Display("0"); break; }
			case VK_NUMPAD1: { Display("1"); break; }
			case VK_NUMPAD2: { Display("2"); break; }
			case VK_NUMPAD3: { Display("3"); break; }
			case VK_NUMPAD4: { Display("4"); break; }
			case VK_NUMPAD5: { Display("5"); break; }
			case VK_NUMPAD6: { Display("6"); break; }
			case VK_NUMPAD7: { Display("7"); break; }
			case VK_NUMPAD8: { Display("8"); break; }
			case VK_NUMPAD9: { Display("9"); break; }
			case VK_MULTIPLY: { Display("*"); break; }
			case VK_ADD: { Display("+"); break; }
			case VK_SUBTRACT: { Display("-"); break; }
			case VK_DECIMAL: { Display(","); break; }
			case VK_DIVIDE: { Display("/"); break; }
			case VK_BACK: { Display("[BACKSPACE]"); break; }
			case VK_TAB: { Display("[TAB]"); break; }
			case VK_RETURN: { Display("[ENTER]"); break; }
			case VK_MENU: { Display("[ALT]"); break; }
			case VK_LMENU: { Display("[ALT]"); break; }
			case VK_RMENU: { Display("[ALT]"); break; }
			case VK_ESCAPE: { Display("[ESC]"); break; }
			case VK_PRIOR: { Display("[PG UP]"); break; }
			case VK_NEXT: { Display("[PG DN]"); break; }
			case VK_END: { Display("[END]"); break; }
			case VK_HOME: { Display("[HOME]"); break; }
			case VK_LEFT: { Display("[LEFT]"); break; }
			case VK_UP: { Display("[RIGHT]"); break; }
			case VK_RIGHT: { Display("[RIGHT]"); break; }
			case VK_DOWN: { Display("[DOWN]"); break; }
			case VK_PRINT: { Display("[PRINT]"); break; }
			case VK_SNAPSHOT: { Display("[PRT SC]"); break; }
			case VK_INSERT: { Display("[INSERT]"); break; }
			case VK_DELETE: { Display("[DELETE]"); break; }
			case VK_LWIN: { Display("[WIN KEY]"); break; }
			case VK_RWIN: { Display("[WIN KEY]"); break; }
			case VK_CAPITAL: { capital = !capital;   break; }
			case VK_NUMLOCK: { numLock = !numLock;   break; }
			case VK_LCONTROL: { if (wParam == WM_KEYDOWN) { Display("[CTRL]"); } break; }
			case VK_RCONTROL: { if (wParam == WM_KEYDOWN) { Display("[CTRL]"); } break; }
			case VK_F1: { Display("[F1]"); break; }
			case VK_F2: { Display("[F2]"); break; }
			case VK_F3: { Display("[F3]"); break; }
			case VK_F4: { Display("[F4]"); break; }
			case VK_F5: { Display("[F5]"); break; }
			case VK_F6: { Display("[F6]"); break; }
			case VK_F7: { Display("[F7]"); break; }
			case VK_F8: { Display("[F8]"); break; }
			case VK_F9: { Display("[F9]"); break; }
			case VK_F10: { Display("[F10]"); break; }
			case VK_F11: { Display("[F11]"); break; }
			case VK_F12: { Display("[F12]"); break; }
			default: {
				DWORD dWord = keystroke->scanCode << 16;
				dWord += keystroke->flags << 24;
				char otherKey[16] = "";
				if (GetKeyNameTextA(dWord, otherKey, sizeof(otherKey)) != 0) {
					Display(otherKey);
				}
			}
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD tid = 0;

void HookJob() {
	HHOOK hHook = SetWindowsHookExA(WH_KEYBOARD_LL, HookProc, NULL, 0);
	if (hHook == NULL) {
		MainWnd()->ShowInfo("无法安装HOOK");
	}
	else {
		MainWnd()->ShowInfo("HOOK安装成功，正在进行键鼠消息记录......");

		capital = GetKeyState(VK_CAPITAL);
		numLock = GetKeyState(VK_NUMLOCK);
		MSG msg = { };
		while (GetMessageA(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		if (UnhookWindowsHookEx(hHook) == 0) {
			MainWnd()->ShowInfo("HOOK卸载失败");
		}
		else {
			MainWnd()->ShowInfo("HOOK卸载成功");
		}
		
		//CloseHandle(hHook);
	}
}

void RemoveHookThread(int code) {
	if (PostThreadMessageA(tid, WM_QUIT, NULL, NULL) == 0) {
		MainWnd()->ShowInfo("无法向HOOK进程发送WM_QUIT消息，移除HOOK失败");
		//exit(EXIT_FAILURE);
	}
}

void CreateHookThread() {
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookJob, NULL, 0, &tid);
	if (hThread == NULL) {
		MainWnd()->ShowInfo("无法创建HOOK进程");
	}
	//else {
	//	signal(SIGINT, RemoveHookThread);
	//	WaitForSingleObject(hThread, INFINITE);
	//	signal(SIGINT, SIG_DFL);
	//	CloseHandle(hThread);
	//}
}