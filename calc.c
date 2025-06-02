#include <windows.h>
#include <stdio.h>

#define ID_BUTTON_0 100
#define ID_BUTTON_1 101
#define ID_BUTTON_2 102
#define ID_BUTTON_3 103
#define ID_BUTTON_4 104
#define ID_BUTTON_5 105
#define ID_BUTTON_6 106
#define ID_BUTTON_7 107
#define ID_BUTTON_8 108
#define ID_BUTTON_9 109
#define ID_BUTTON_ADD 110
#define ID_BUTTON_SUB 111
#define ID_BUTTON_MUL 112
#define ID_BUTTON_DIV 113
#define ID_BUTTON_EQ 114
#define ID_EDIT_DISPLAY 115
#define ID_BUTTON_CLEAR 116

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static char input[256] = "";
    static int operand1 = 0, operand2 = 0;
    static char operator= '\0';

    switch (uMsg) {
        case WM_CREATE: {
            CreateWindowEx(0, "EDIT", "",
                          WS_CHILD | WS_VISIBLE | ES_READONLY | ES_RIGHT,
                          10, 10, 200, 30, hwnd, (HMENU)ID_EDIT_DISPLAY, GetModuleHandle(NULL), NULL);
            
            int x = 10, y = 50, width = 40, height = 40;
            for (int i = 0; i < 10; ++i) {
                char buttonText[2] = { '0' + i, '\0' };
                CreateWindowEx(0, "BUTTON", buttonText,
                              WS_CHILD | WS_VISIBLE,
                              x + (i % 3) * width, y + (i / 3) * height, width, height,
                              hwnd, (HMENU)(ID_BUTTON_0 + i), GetModuleHandle(NULL), NULL);
            }

            CreateWindowEx(0, "BUTTON", "+", WS_CHILD | WS_VISIBLE, 130, 50, 40, 40, hwnd, (HMENU)ID_BUTTON_ADD, GetModuleHandle(NULL), NULL);
            CreateWindowEx(0, "BUTTON", "-", WS_CHILD | WS_VISIBLE, 130, 90, 40, 40, hwnd, (HMENU)ID_BUTTON_SUB, GetModuleHandle(NULL), NULL);
            CreateWindowEx(0, "BUTTON", "*", WS_CHILD | WS_VISIBLE, 130, 130, 40, 40, hwnd, (HMENU)ID_BUTTON_MUL, GetModuleHandle(NULL), NULL);
            CreateWindowEx(0, "BUTTON", "/", WS_CHILD | WS_VISIBLE, 130, 170, 40, 40, hwnd, (HMENU)ID_BUTTON_DIV, GetModuleHandle(NULL), NULL);
            CreateWindowEx(0, "BUTTON", "=", WS_CHILD | WS_VISIBLE, 10, 190, 120, 40, hwnd, (HMENU)ID_BUTTON_EQ, GetModuleHandle(NULL), NULL);
            CreateWindowEx(0, "BUTTON", "C", WS_CHILD | WS_VISIBLE, 130, 190, 40, 40, hwnd, (HMENU)ID_BUTTON_CLEAR, GetModuleHandle(NULL), NULL);
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (wmId >= ID_BUTTON_0 && wmId <= ID_BUTTON_9) {
                char num[2] = { '0' + (wmId - ID_BUTTON_0), '\0' };
                strcat(input, num);
                SetWindowText(GetDlgItem(hwnd, ID_EDIT_DISPLAY), input);
            } else if (wmId == ID_BUTTON_ADD || wmId == ID_BUTTON_SUB || wmId == ID_BUTTON_MUL || wmId == ID_BUTTON_DIV) {
                operand1 = atoi(input);
                operator= wmId;
                input[0] = '\0';
                SetWindowText(GetDlgItem(hwnd, ID_EDIT_DISPLAY), "");
            } else if (wmId == ID_BUTTON_EQ) {
                operand2 = atoi(input);
                int result = 0;
                switch (operator) {
                    case ID_BUTTON_ADD: result = operand1 + operand2; break;
                    case ID_BUTTON_SUB: result = operand1 - operand2; break;
                    case ID_BUTTON_MUL: result = operand1 * operand2; break;
                    case ID_BUTTON_DIV: result = (operand2 != 0) ? (operand1 / operand2) : 0; break;
                }
                sprintf(input, "%d", result);
                SetWindowText(GetDlgItem(hwnd, ID_EDIT_DISPLAY), input);
            } else if (wmId == ID_BUTTON_CLEAR) {
                input[0] = '\0';
                SetWindowText(GetDlgItem(hwnd, ID_EDIT_DISPLAY), "");
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "CalculatorClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "Calculator",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                               250, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}