#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#define MENU_NEW_GAME 1
#define MENU_EXIT_TO_MENU 2
#define MENU_QUIT 3

#define GRID_SIZE 30
#define SCREEN_SIZE 800

#include <tchar.h>
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <tchar.h>
#include <sstream>
#include "resource.h"

void addToolbars(HWND hwnd);
void handleClicks(HWND hwnd, WPARAM wParam);
void addMenu(HWND hwnd);
void loadImage();
BOOL CALLBACK destroyChildCallback(HWND hwnd, LPARAM lParam);
void startGame(HWND hwnd);
void clearScreen(HWND hwnd);
void clearGrid();
void printGrid();
void generateGrid();
void generateButtons(HWND hwnd);
void cellClicked(int id, HWND hwnd);
void revealCell(int x, int y, HWND hwnd, bool mine);
int getNeighborsValue(int x, int y);
void clearRecursion(int x, int y, HWND hwnd);
void paintGrid (HWND hwnd);
void revealAllMines (HWND hwnd);
void markCell(HWND hwnd);
void checkIfAllMarked();
void setNumberOfMines();

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

HWND hPlay, hQuit, hLogo, hMines;
HBITMAP hLogoImage;
int grid[GRID_SIZE][GRID_SIZE];
HWND hGrid[GRID_SIZE][GRID_SIZE];
bool checkedGrid[GRID_SIZE][GRID_SIZE] = {0};
bool enabled = true;
bool marked[GRID_SIZE][GRID_SIZE] = {0};
int screenSize = (SCREEN_SIZE/GRID_SIZE) * GRID_SIZE;
int numberOfMines = 1;

bool created = false;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    HICON hWinIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(IDI_WINICON));
    wincl.hIcon = hWinIcon;
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

    // default cursor
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */

    // bg color
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Minesweeper"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VISIBLE, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           screenSize + 15,                 /* The programs width */
           screenSize + 60,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
            // handle button clicks by id
            handleClicks(hwnd, wParam);
            break;
        case WM_CREATE:

            addToolbars(hwnd);
            addMenu(hwnd);
            break;
        case WM_CONTEXTMENU:
            markCell((HWND) wParam);
        case WM_PAINT:
            paintGrid(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

void addToolbars(HWND hwnd){

    HICON hWinIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WINICON));
    HICON hWinIconBig = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_WINICONBIG));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hWinIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hWinIconBig);
    HMENU hMenu = CreateMenu();
    HMENU fileHMenu = CreateMenu();
    AppendMenu(fileHMenu, MF_STRING, MENU_NEW_GAME, "New Game");
    AppendMenu(fileHMenu, MF_STRING, MENU_EXIT_TO_MENU, "Main Menu");
    AppendMenu(fileHMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(fileHMenu, MF_STRING, MENU_QUIT, "Quit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR) fileHMenu, "Options");
    SetMenu(hwnd, hMenu);
}

void handleClicks(HWND hwnd, WPARAM wParam) {

    switch (wParam)
    {
        case MENU_NEW_GAME:
            startGame(hwnd);
            break;
        case MENU_EXIT_TO_MENU:
            clearScreen(hwnd);
            addMenu(hwnd);
            break;
        case MENU_QUIT:
            DestroyWindow(hwnd);
            break;
        default:
            if(wParam >= 1000 && wParam < 1000000) cellClicked((int) wParam, hwnd);
            break;
    }
}

void addMenu(HWND hwnd) {

    HANDLE hFile;
    hFile = CreateFile("data.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(GetLastError() == 2){
        hFile = CreateFile("data.txt", GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    DWORD nRead;
    char readBuffer[20] = {0};
    ReadFile(hFile, readBuffer, 20, &nRead, NULL);
    if(nRead > 0) {
        numberOfMines = atoi(readBuffer);
    }
    CloseHandle(hFile);
    if(numberOfMines <= 0) {
        numberOfMines = 1;
    }

    TCHAR buf[20];
    _stprintf(buf, _T("%d"), numberOfMines);

    loadImage();
    hLogo = CreateWindowW(L"static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, screenSize, screenSize, hwnd, NULL, NULL, NULL);
    hPlay = CreateWindowW(L"button", L"Play", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, screenSize/2 - 50, screenSize/2 - 115, 100, 30, hwnd, (HMENU) MENU_NEW_GAME, NULL, NULL);
    hQuit = CreateWindowW(L"button", L"Quit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, screenSize/2 - 30, screenSize/2 - 15, 60, 30, hwnd, (HMENU) MENU_QUIT, NULL, NULL);
    CreateWindowW(L"static", L"Made by Dovydas Puluikis", WS_CHILD | WS_VISIBLE, screenSize - 180, screenSize - 20, 180, 20, hwnd, NULL, NULL, NULL);
    SendMessageW(hLogo, STM_SETIMAGE, IMAGE_BITMAP,(LPARAM) hLogoImage);

    CreateWindowW(L"static", L"Mine count", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER, screenSize/2 - 50, screenSize/2 - 65, 100, 20, hwnd, NULL, NULL, NULL);
    hMines = CreateWindow("edit", buf,WS_VISIBLE | WS_CHILD | WS_BORDER, screenSize/2 - 50, screenSize/2 - 45, 100, 20, hwnd, NULL, NULL, NULL);
}

void loadImage() {
    hLogoImage = (HBITMAP) LoadImageW(NULL, L"images\\menu.bmp", IMAGE_BITMAP, screenSize, screenSize, LR_LOADFROMFILE | LR_DEFAULTSIZE);
}

void startGame(HWND hwnd) {
    enabled = true;
    setNumberOfMines();
    clearScreen(hwnd);
    clearGrid();
    generateGrid();
    //printGrid();
    generateButtons(hwnd);
}

void clearScreen(HWND hwnd) {
    EnumChildWindows(hwnd, destroyChildCallback,(LPARAM) NULL);
}

BOOL CALLBACK destroyChildCallback(HWND hwnd, LPARAM lParam){
    if(hwnd != NULL) {
        DestroyWindow(hwnd);
    }
    return true;
}

void clearGrid() {
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            grid[i][j] = 0;
            checkedGrid[i][j] = false;
            marked[i][j] = false;
        }
    }
}

void printGrid() {
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void generateGrid() {
    srand (time(NULL));
    for(int i = 0; i < numberOfMines; i++){
        int mineX = rand() % GRID_SIZE;
        int mineY = rand() % GRID_SIZE;
        int k = 0;
        // 10 kartu bandom mina idet, paskui pasiduodam :(
        while(grid[mineX][mineY] < 0 && k < 10){
            mineX = rand() % GRID_SIZE;
            mineY = rand() % GRID_SIZE;
        }
        if(grid[mineX][mineY] > -1){
            for(int x = -1; x <= 1; x++){
                for(int y = -1; y <= 1; y++){
                    if(x == 0 && y == 0){
                        grid[mineX][mineY] = -20;
                    }
                    else if(grid[mineX + x][mineY + y] > -1 && mineX + x < GRID_SIZE && mineY + y < GRID_SIZE && mineX + x >= 0 && mineY + y >= 0) grid[mineX + x][mineY + y] += 1;
                }
            }
        }
    }
}

void generateButtons(HWND hwnd) {
    int grid_length = sizeof(GRID_SIZE) - 2;
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            int x_length = std::to_string(i).length();
            int y_length = std::to_string(j).length();
            std::string x_string = std::string(grid_length - std::min(grid_length, x_length), '0') + std::to_string(i);
            std::string y_string = std::string(grid_length - std::min(grid_length, y_length), '0') + std::to_string(j);
            int identifier = std::stoi("1" + x_string + y_string);
            hGrid[i][j] = CreateWindowW(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON, screenSize/GRID_SIZE*i, screenSize/GRID_SIZE*j, screenSize/GRID_SIZE, screenSize/GRID_SIZE, hwnd, (HMENU) identifier, NULL, NULL);
        }
    }
}

void cellClicked(int id, HWND hwnd) {
    if(enabled){
        std::string id_string = std::to_string(id);
        id_string.erase(0, 1);
        std::string x_string = id_string.substr(0, id_string.length()/2);
        std::string y_string = id_string.substr(id_string.length()/2);
        int x = std::stoi(x_string);
        int y = std::stoi(y_string);
        if(!marked[x][y]){
            if(grid[x][y] > 0) {
                DestroyWindow(hGrid[x][y]);
                hGrid[x][y] = 0;
                revealCell(x, y, hwnd, false);
            } else if (grid[x][y] == 0){
                clearRecursion(x, y, hwnd);
            } else {
                enabled = false;
                DestroyWindow(hGrid[x][y]);
                hGrid[x][y] = 0;
                revealCell(x, y, hwnd, true);
                revealAllMines(hwnd);
            }
        }
    }
}

void revealCell(int x, int y, HWND hwnd, bool mine) {
    if(!checkedGrid[x][y]){
        std::string path;
        if(grid[x][y] == 0 && hGrid[x][y] != 0) {
            DestroyWindow(hGrid[x][y]);
            hGrid[x][y] = 0;
            return;
        }
        else if(grid[x][y] < 0 && mine) path = "images\\mine-death.bmp";
        else if(grid[x][y] < 0) path = "images\\mine-ceil.bmp";
        else path = "images\\open" + std::to_string(grid[x][y]) + ".bmp";
        std::wstring widePath =  std::wstring(path.begin(), path.end());
        const wchar_t* cPath = widePath.c_str();
        HBITMAP hImage = (HBITMAP) LoadImageW(NULL, cPath, IMAGE_BITMAP, (screenSize/GRID_SIZE) - 2, (screenSize/GRID_SIZE) - 2, LR_LOADFROMFILE);
        HWND wImage = CreateWindowW(L"static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, screenSize/GRID_SIZE*x + 1, screenSize/GRID_SIZE*y + 1, (screenSize/GRID_SIZE) - 2, (screenSize/GRID_SIZE) - 2, hwnd, NULL, NULL, NULL);
        SendMessageW(wImage, STM_SETIMAGE, IMAGE_BITMAP,(LPARAM) hImage);
    }
}

int getNeighborsValue(int x, int y){
    int sum = 0;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            sum += grid[x + i][y + j];
        }
    }
    return sum;
}

void clearRecursion(int x, int y, HWND hwnd) {
    if(x < GRID_SIZE && y < GRID_SIZE && checkedGrid[x][y] == false && x >= 0 && y >=0 && grid[x][y] >= 0) {
        marked[x][y] = false;
        DestroyWindow(hGrid[x][y]);
        revealCell(x, y, hwnd, false);
        checkedGrid[x][y] = true;
        if(getNeighborsValue(x, y) >= 0) {
            for(int i = - 1; i <=1; i++){
                for(int j = -1; j<=1; j++){
                    clearRecursion(x+i, y+j, hwnd);
                }
            }
        }
    }
}

void paintGrid(HWND hwnd) {
    int axisLength = (screenSize/GRID_SIZE) * GRID_SIZE;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, Pen);
    for(int i = 0; i < GRID_SIZE+1; i++){
        MoveToEx(hdc, screenSize/GRID_SIZE*i, 0, (LPPOINT) NULL);
        LineTo(hdc, screenSize/GRID_SIZE*i, axisLength);
    }
    for(int i = 0; i < GRID_SIZE+1; i++){
        MoveToEx(hdc, 0, screenSize/GRID_SIZE*i, (LPPOINT) NULL);
        LineTo(hdc, axisLength, screenSize/GRID_SIZE*i);
    }
    EndPaint(hwnd, &ps);
}

void revealAllMines (HWND hwnd) {
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            if(grid[i][j] < 0 && !marked[i][j]){
                DestroyWindow(hGrid[i][j]);
                revealCell(i, j, hwnd, false);
            }
        }
    }
}

void markCell(HWND hwnd) {
    if(enabled){
        int id = GetDlgCtrlID(hwnd);
        int firstDigit = id;
        while(firstDigit >= 10)
            firstDigit /= 10;
        if(firstDigit == 1){
            std::string id_string = std::to_string(id);
            id_string.erase(0, 1);
            std::string x_string = id_string.substr(0, id_string.length()/2);
            std::string y_string = id_string.substr(id_string.length()/2);
            int x = std::stoi(x_string);
            int y = std::stoi(y_string);
            HWND parentHandle = GetParent(hwnd);
            HBITMAP hMarkedImage = (HBITMAP) LoadImageW(NULL, L"images\\flag.bmp", IMAGE_BITMAP, (screenSize/GRID_SIZE) - 2, (screenSize/GRID_SIZE) - 2, LR_LOADFROMFILE);
            DestroyWindow(hwnd);
            if(!marked[x][y]){
                hGrid[x][y] = CreateWindowW(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_FLAT | BS_BITMAP, screenSize/GRID_SIZE*x, screenSize/GRID_SIZE*y, screenSize/GRID_SIZE, screenSize/GRID_SIZE, parentHandle, (HMENU) id, NULL, NULL);
                SendMessageW(hGrid[x][y], BM_SETIMAGE, IMAGE_BITMAP,(LPARAM) hMarkedImage);
                marked[x][y] = true;
            } else {
                hGrid[x][y] = CreateWindowW(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON, screenSize/GRID_SIZE*x, screenSize/GRID_SIZE*y, screenSize/GRID_SIZE, screenSize/GRID_SIZE, parentHandle, (HMENU) id, NULL, NULL);
                marked[x][y] = false;
            }
            checkIfAllMarked();
        }
    }
}

void checkIfAllMarked(){
    bool markedAll = true;
    for(int i = 0; i < GRID_SIZE; i++){
        for(int j = 0; j < GRID_SIZE; j++){
            if(grid[i][j] < 0){
                if(!marked[i][j]) markedAll = false;
            } else {
                if(marked[i][j]) markedAll = false;
            }
        }
    }
    if(markedAll){
        enabled = false;
        MessageBoxW(NULL, L"You Win! :)         ", L"Congratulations!       ", MB_OK);
    }
}

void setNumberOfMines() {
    wchar_t text[100];
    int length = GetWindowTextW(hMines, text, 100);
    bool isNumber = true;
    if(length > 0) {
        for(int i = 0; i < length; i++) {
            if(!iswdigit(text[i])) isNumber = false;
        }
        if(isNumber){
            numberOfMines = _wtoi(text);
            if(numberOfMines <= 0) {
                numberOfMines = 1;
            }
        }
    }

    HANDLE hFile;
    hFile = CreateFile("data.txt", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    std::string str = std::to_string(numberOfMines);
    WriteFile(hFile, str.c_str(), str.size(), NULL, NULL);
    CloseHandle(hFile);
}
