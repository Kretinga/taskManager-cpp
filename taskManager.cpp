#include <iostream>
#include <string>
#include <windows.h>
#include <psapi.h>
#include <tchar.h>

using namespace std;

std::wstring ConvertStringToWString(const std::string& str) {
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}

void listProcess(DWORD processID) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess != NULL) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }

        _tprintf(TEXT("%s (PID: %u)\n"), szProcessName, processID);
        CloseHandle(hProcess);
    }
    else {
        _tprintf(TEXT("<unknown> (PID: %u)\n"), processID);
    }
}

int main() {
    std::string option;
    DWORD aProcesses[1024];
    DWORD cbNeeded;
    unsigned int i;

    while (true) {
        cout << "\n\n[+] Select an option:\n[1] - list (list all processes)\n[2] - kill *pid* (kill a process through its PID)\n[3] - start *app* (start an app through its name)\n[4] - exit (exit the program)\n[0] - examples - Show examples\n>> ";
        getline(cin, option);

        if (option == "examples") {
            cout << "\n[+] List of examples:\n[1] - list\n[2] - kill 14321\n[3] - start calc.exe\n";
        }
        else if (option == "list") {
            memset(aProcesses, 0, sizeof(aProcesses));

            if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
                unsigned int cProcesses = cbNeeded / sizeof(DWORD);
                cout << "Number of processes: " << cProcesses << endl;

                for (i = 0; i < cProcesses; i++) {
                    if (aProcesses[i] != 0) {
                        listProcess(aProcesses[i]);
                    }
                }
            }
            else {
                cout << "Error enumerating processes." << endl;
            }
        }
        else if (option.substr(0, 4) == "kill") {
            string pid_str = option.substr(5);
            DWORD pid = stoul(pid_str);

            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess == NULL) {
                cout << "[!] ERROR trying to kill process!" << endl;
            }
            else {
                if (TerminateProcess(hProcess, 0)) {
                    cout << "[+] Process killed successfully!" << endl;
                }
                else {
                    cout << "[!] ERROR trying to kill process!" << endl;
                }
                CloseHandle(hProcess);
            }
        }
        else if (option.substr(0, 5) == "start") {
            string app_str = option.substr(6);
            std::wstring app_wstr = ConvertStringToWString(app_str);

            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            LPWSTR lpwstrAppName = &app_wstr[0];

            if (CreateProcess(NULL, lpwstrAppName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                cout << "[+] Application started successfully! PID: " << pi.dwProcessId << endl;
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                cout << "[!] ERROR: Failed to start the application." << endl;
            }
        }
        else if (option == "exit") {
            break;
        }
    }

    return 0;
}
