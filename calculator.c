#include <windows.h>
#include <stdio.h>
#include <string.h>

// Calculator window dimensions
#define CALC_WIDTH 300
#define CALC_HEIGHT 400

// Button dimensions
#define BTN_WIDTH 60
#define BTN_HEIGHT 60
#define BTN_PADDING 10

// Calculator state
typedef struct {
    char display[256];         // Current display text
    double current_value;      // Current numerical value
    double stored_value;       // Stored value for operations
    char operator;             // Current operation (+, -, *, /)
    BOOL new_input;            // Flag to indicate if next digit starts a new number
    BOOL has_decimal;          // Flag to track if decimal point has been entered
} CalculatorState;

// Global variables
HWND g_display;                // Handle to display edit control
CalculatorState g_calc = {0};  // Calculator state

// Function to update the display text
void UpdateDisplay() {
    SetWindowText(g_display, g_calc.display);
}

// Function to process digit button clicks
void ProcessDigit(char digit) {
    // Start new input if needed
    if (g_calc.new_input) {
        g_calc.display[0] = '\0';
        g_calc.new_input = FALSE;
        g_calc.has_decimal = FALSE;
    }
    
    // Add digit to display if there's room
    if (strlen(g_calc.display) < 250) {
        int len = strlen(g_calc.display);
        g_calc.display[len] = digit;
        g_calc.display[len + 1] = '\0';
    }
    
    UpdateDisplay();
}

// Function to process decimal point
void ProcessDecimal() {
    // Start new input if needed
    if (g_calc.new_input) {
        strcpy(g_calc.display, "0");
        g_calc.new_input = FALSE;
        g_calc.has_decimal = FALSE;
    }
    
    // Add decimal point if it doesn't already exist
    if (!g_calc.has_decimal) {
        // If display is empty, add a leading zero
        if (strlen(g_calc.display) == 0) {
            strcpy(g_calc.display, "0");
        }
        
        int len = strlen(g_calc.display);
        g_calc.display[len] = '.';
        g_calc.display[len + 1] = '\0';
        g_calc.has_decimal = TRUE;
    }
    
    UpdateDisplay();
}

// Function to process operator buttons
void ProcessOperator(char op) {
    // Calculate result of previous operation if one exists
    if (strlen(g_calc.display) > 0) {
        double input_value = atof(g_calc.display);
        
        // Perform pending calculation if there is one
        if (g_calc.operator) {
            switch (g_calc.operator) {
                case '+':
                    g_calc.stored_value += input_value;
                    break;
                case '-':
                    g_calc.stored_value -= input_value;
                    break;
                case '*':
                    g_calc.stored_value *= input_value;
                    break;
                case '/':
                    if (input_value != 0) {
                        g_calc.stored_value /= input_value;
                    } else {
                        strcpy(g_calc.display, "Error: Div by 0");
                        UpdateDisplay();
                        return;
                    }
                    break;
            }
        } else {
            // No previous operator, just store the value
            g_calc.stored_value = input_value;
        }
    }
    
    // Set new operator
    g_calc.operator = op;
    g_calc.new_input = TRUE;
    
    // Update display with result
    sprintf(g_calc.display, "%.8g", g_calc.stored_value);
    UpdateDisplay();
}

// Function to process equals button
void ProcessEquals() {
    if (strlen(g_calc.display) > 0 && g_calc.operator) {
        double input_value = atof(g_calc.display);
        
        // Perform calculation based on current operator
        switch (g_calc.operator) {
            case '+':
                g_calc.stored_value += input_value;
                break;
            case '-':
                g_calc.stored_value -= input_value;
                break;
            case '*':
                g_calc.stored_value *= input_value;
                break;
            case '/':
                if (input_value != 0) {
                    g_calc.stored_value /= input_value;
                } else {
                    strcpy(g_calc.display, "Error: Div by 0");
                    UpdateDisplay();
                    return;
                }
                break;
        }
        
        // Clear operator and prepare for new input
        g_calc.operator = 0;
        g_calc.new_input = TRUE;
        
        // Display result
        sprintf(g_calc.display, "%.8g", g_calc.stored_value);
        UpdateDisplay();
    }
}

// Function to clear calculator
void ProcessClear() {
    g_calc.display[0] = '\0';
    g_calc.current_value = 0;
    g_calc.stored_value = 0;
    g_calc.operator = 0;
    g_calc.new_input = FALSE;
    g_calc.has_decimal = FALSE;
    
    UpdateDisplay();
}

// Window procedure to handle messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create calculator display
            g_display = CreateWindow(
                "EDIT", "0",
                WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
                BTN_PADDING, BTN_PADDING, 
                CALC_WIDTH - 2 * BTN_PADDING, 40,
                hwnd, (HMENU)100, NULL, NULL
            );
            
            // Set display font
            HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                   DEFAULT_PITCH | FF_SWISS, "Arial");
            SendMessage(g_display, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            // Create number buttons (0-9)
            int startX = BTN_PADDING;
            int startY = 60;
            
            // Create buttons 1-9 in a grid
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                    char btnText[2];
                    sprintf(btnText, "%d", 7 - row * 3 + col);
                    
                    CreateWindow(
                        "BUTTON", btnText,
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        startX + col * (BTN_WIDTH + BTN_PADDING),
                        startY + row * (BTN_HEIGHT + BTN_PADDING),
                        BTN_WIDTH, BTN_HEIGHT,
                        hwnd, (HMENU)(7 - row * 3 + col), NULL, NULL
                    );
                }
            }
            
            // Create button 0 (wider than others)
            CreateWindow(
                "BUTTON", "0",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                startX,
                startY + 3 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH * 2 + BTN_PADDING, BTN_HEIGHT,
                hwnd, (HMENU)0, NULL, NULL
            );
            
            // Create decimal point button
            CreateWindow(
                "BUTTON", ".",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                startX + 2 * (BTN_WIDTH + BTN_PADDING),
                startY + 3 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH, BTN_HEIGHT,
                hwnd, (HMENU)10, NULL, NULL
            );
            
            // Create operator buttons
            int opStartX = startX + 3 * (BTN_WIDTH + BTN_PADDING);
            
            // + button
            CreateWindow(
                "BUTTON", "+",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                opStartX,
                startY,
                BTN_WIDTH, BTN_HEIGHT,
                hwnd, (HMENU)11, NULL, NULL
            );
            
            // - button
            CreateWindow(
                "BUTTON", "-",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                opStartX,
                startY + (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH, BTN_HEIGHT,
                hwnd, (HMENU)12, NULL, NULL
            );
            
            // * button
            CreateWindow(
                "BUTTON", "*",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                opStartX,
                startY + 2 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH, BTN_HEIGHT,
                hwnd, (HMENU)13, NULL, NULL
            );
            
            // / button
            CreateWindow(
                "BUTTON", "/",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                opStartX,
                startY + 3 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH, BTN_HEIGHT,
                hwnd, (HMENU)14, NULL, NULL
            );
            
            // = button
            CreateWindow(
                "BUTTON", "=",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                startX,
                startY + 4 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH * 2 + BTN_PADDING, BTN_HEIGHT,
                hwnd, (HMENU)15, NULL, NULL
            );
            
            // Clear button
            CreateWindow(
                "BUTTON", "C",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                startX + 2 * (BTN_WIDTH + BTN_PADDING),
                startY + 4 * (BTN_HEIGHT + BTN_PADDING),
                BTN_WIDTH * 2 + BTN_PADDING, BTN_HEIGHT,
                hwnd, (HMENU)16, NULL, NULL
            );
            
            break;
        }
        
        case WM_COMMAND: {
            // Handle button clicks
            int id = LOWORD(wParam);
            
            // Number buttons (0-9)
            if (id >= 0 && id <= 9) {
                char digit = '0' + id;
                ProcessDigit(digit);
            }
            // Decimal point
            else if (id == 10) {
                ProcessDecimal();
            }
            // Operators
            else if (id == 11) {  // +
                ProcessOperator('+');
            }
            else if (id == 12) {  // -
                ProcessOperator('-');
            }
            else if (id == 13) {  // *
                ProcessOperator('*');
            }
            else if (id == 14) {  // /
                ProcessOperator('/');
            }
            // Equals
            else if (id == 15) {
                ProcessEquals();
            }
            // Clear
            else if (id == 16) {
                ProcessClear();
            }
            
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    return 0;
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "CalculatorClass";
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create main window
    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "CalculatorClass",
        "Win32 Calculator",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CALC_WIDTH, CALC_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Initialize calculator state
    memset(&g_calc, 0, sizeof(g_calc));
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}