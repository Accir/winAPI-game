#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#define MENU_NEW_GAME 1
#define MENU_EXIT_TO_MENU 2
#define MENU_QUIT 3

#include <tchar.h>
#include <windows.h>
#include <cstdio>
#include <iostream>

void addToolbars(HWND hwnd);
void handleToolbarClicks(HWND hwnd, WPARAM wParam);
void addMenu(HWND hwnd);
void addBackground(HWND hwnd);

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

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
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
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
           _T("Windows API Game"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VISIBLE, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           500,                 /* The programs width */
           500,                 /* and height in pixels */
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
            handleToolbarClicks(hwnd, wParam);
            break;
        case WM_CREATE:
            addToolbars(hwnd);
            addMenu(hwnd);
            //addBackground(hwnd);
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

    HMENU hMenu = CreateMenu();
    HMENU fileHMenu = CreateMenu();

    AppendMenu(fileHMenu, MF_STRING, MENU_NEW_GAME, "New Game");
    AppendMenu(fileHMenu, MF_STRING, MENU_EXIT_TO_MENU, "Main Menu");
    AppendMenu(fileHMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(fileHMenu, MF_STRING, MENU_QUIT, "Quit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR) fileHMenu, "Options");
    SetMenu(hwnd, hMenu);
}

void handleToolbarClicks(HWND hwnd, WPARAM wParam) {

    switch (wParam)
    {
        case MENU_NEW_GAME:
            break;
        case MENU_EXIT_TO_MENU:
            break;
        case MENU_QUIT:
            DestroyWindow(hwnd);
            break;
        default:
            break;
    }
}

void addMenu(HWND hwnd) {
    HWND playButton = CreateWindowW(L"button", L"Play", WS_CHILD | WS_VISIBLE, 190, 180, 100, 30, hwnd, NULL, NULL, NULL);
    HWND quitButton = CreateWindowW(L"button", L"Quit", WS_CHILD | WS_VISIBLE, 210, 240, 60, 30, hwnd, NULL, NULL, NULL);
    HWND credits = CreateWindowW(L"static", L"Made by Dovydas Puluikis", WS_CHILD | WS_VISIBLE, 310, 420, 180, 20, hwnd, NULL, NULL, NULL);
}

void addBackground(HWND hwnd) {

    //HBITMAP hImage = (HBITMAP)LoadImage(NULL, L"menu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //HWND hImageView = CreateWindowEx(NULL, L"STATIC", NULL, SS_BITMAP | WS_VISIBLE | WS_CHILD, 0, 00, 500, 244, hwnd, (HMENU)IMAGE_VIEW, GetModuleHandle(NULL), NULL);
    //SendMessage(hImageView, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);

}
