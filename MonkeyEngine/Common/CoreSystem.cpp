//*********************************************************
//
// Copyright (c) Rick Laboratories. All rights reserved.
// This code is licensed under the MIT License (MIT).
// Autor: Luis Borrero
//
//*********************************************************

#include "CoreSystem.h"

HWND CoreSystem::CoreHwnd = nullptr;
CoreEngine* CoreSystem::MainCoreEngine = nullptr;

/// <summary>
/// run main program
/// </summary>
/// <param name="mHinstance">core instance</param>
/// <param name="cmdShow">show windows?</param>
/// <returns>return result integer application</returns>
int CoreSystem::Run(CoreEngine* CoreEngine, HINSTANCE mHinstance, int cmdShow)
{
    MainCoreEngine = CoreEngine;
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"CoreWindows";
    wc.hInstance = mHinstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);

    RECT ClientRect = { 0,0,500,500 };
    AdjustWindowRect(&ClientRect, WS_OVERLAPPEDWINDOW, FALSE);

    CoreHwnd = CreateWindow(
        wc.lpszClassName,
        L"CoreWindows",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ClientRect.right - ClientRect.left,
        ClientRect.bottom - ClientRect.top,
        nullptr,
        nullptr,
        mHinstance,
        nullptr);
    ShowWindow(CoreHwnd, cmdShow);

    CoreEngine->InitDirect3D(CoreHwnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            MainCoreEngine->GetCoreTimer().Tick();
            MainCoreEngine->Loop(MainCoreEngine->GetCoreTimer());
        }
    }

    return static_cast<char>(msg.message);
}

HWND CoreSystem::GetHwnd()
{
    return CoreHwnd;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
/// <summary>
/// Windows procedures
/// </summary>
/// <param name="hWnd">handler</param>
/// <param name="msg">message</param>
/// <param name="wParam">param</param>
/// <param name="lParam">long param</param>
/// <returns>return result of the procesure</returns>
LRESULT CoreSystem::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        MainCoreEngine->WindowRedimention(LOWORD(lParam), HIWORD(lParam));    
        MainCoreEngine->ResetEngine();     
        return 0;
    case WM_EXITSIZEMOVE:
        MainCoreEngine->ResetEngine();
        return 0;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
