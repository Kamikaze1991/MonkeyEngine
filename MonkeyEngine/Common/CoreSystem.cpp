//*********************************************************
//
// Copyright (c) Rick Laboratories. All rights reserved.
// This code is licensed under the MIT License (MIT).
// Autor: Luis Borrero
//
//*********************************************************

#include "CoreSystem.h"

HWND CoreSystem::mCoreHwnd = nullptr;
CoreBase* CoreSystem::mCoreBase = nullptr;

/// <summary>
/// run main program
/// </summary>
/// <param name="mHinstance">core instance</param>
/// <param name="cmdShow">show windows?</param>
/// <returns>return result integer application</returns>
int CoreSystem::Run(CoreBase* CoreBase, HINSTANCE mHinstance, int cmdShow)
{
    mCoreBase = CoreBase;
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

    mCoreHwnd = CreateWindow(
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
    ShowWindow(mCoreHwnd, cmdShow);

    CoreBase->InitDirect3D(mCoreHwnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            mCoreBase->Loop();
        }
    }

    return static_cast<char>(msg.message);
}

HWND CoreSystem::GetHwnd()
{
    return mCoreHwnd;
}

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
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        mCoreBase->mClientWidth = LOWORD(lParam);
        mCoreBase->mClientHeight = HIWORD(lParam);
        return 0;
    case WM_EXITSIZEMOVE:
        mCoreBase->GetCoreGraphics()->OnReset();
        return 0;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
