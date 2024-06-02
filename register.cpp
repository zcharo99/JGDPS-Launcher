#include <windows.h>
#include <shlwapi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>

#pragma comment(lib, "Shlwapi.lib")

bool IsRunningAsAdmin() {
    BOOL fIsRunAsAdmin = FALSE;
    PSID pAdministratorsGroup = NULL;
    
    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, 
                                  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, 
                                  &pAdministratorsGroup)) {
        return false;
    }

    // Determine whether the SID of administrators group is enabled in the primary access token.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin)) {
        fIsRunAsAdmin = FALSE;
    }

    // Free the SID.
    if (pAdministratorsGroup) {
        FreeSid(pAdministratorsGroup);
    }

    return fIsRunAsAdmin;
}

void RelaunchAsAdmin() {
    char szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath))) {
        // Launch itself as an admin.
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteEx(&sei)) {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED) {
                std::cout << "Please run this application as an administrator." << std::endl;
            } else {
                std::cout << "Failed to relaunch as an administrator. Error: " << dwError << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}

bool RegisterCustomUrlScheme(const std::string& appPath) {
    HKEY hKey;
    const char* urlScheme = "appname";

    // Create the registry key for the custom URL scheme
    LONG result = RegCreateKeyEx(HKEY_CLASSES_ROOT, urlScheme, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        return false;
    }
    RegSetValueEx(hKey, NULL, 0, REG_SZ, (const BYTE*)"URL:AppName Protocol", strlen("URL:AppName Protocol") + 1);
    RegSetValueEx(hKey, "URL Protocol", 0, REG_SZ, (const BYTE*)"", 1);

    HKEY hSubKey;
    result = RegCreateKeyEx(hKey, "shell\\open\\command", 0, NULL, 0, KEY_WRITE, NULL, &hSubKey, NULL);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }
    RegSetValueEx(hSubKey, NULL, 0, REG_SZ, (const BYTE*)appPath.c_str(), appPath.length() + 1);

    RegCloseKey(hSubKey);
    RegCloseKey(hKey);
    return true;
}

bool ValidateAppPath(const std::string& appPath) {
    std::string path = appPath;

    // Convert forward slashes to backslashes
    std::replace(path.begin(), path.end(), '/', '\\');

    // Check if the file has a valid extension (.exe or .scr)
    if (PathFindExtensionA(path.c_str()) != ".exe" && PathFindExtensionA(path.c_str()) != ".scr") {
        std::cout << "Invalid file extension. File must have .exe or .scr extension." << std::endl;
        return false;
    }

    // Check if the file exists
    if (!PathFileExistsA(path.c_str())) {
        std::cout << "File does not exist." << std::endl;
        return false;
    }

    return true;
}

int main() {
    std::string appPath;
    std::cout << "Enter the application path (e.g., \"C:/Path/To/YourApp.exe\" \"%1\"): ";
    std::getline(std::cin, appPath);

    if (!ValidateAppPath(appPath)) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (IsRunningAsAdmin()) {
        std::cout << "Running with administrator privileges." << std::endl;
        if (RegisterCustomUrlScheme(appPath)) {
            std::cout << "Success!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            std::cout << "Failed to register URL scheme." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    } else {
        std::cout << "Not running as an administrator, attempting to relaunch..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        RelaunchAsAdmin();
    }

    return 0;
}