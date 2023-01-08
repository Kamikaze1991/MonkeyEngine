//*********************************************************
//
// Copyright (c) Rick Laboratories. All rights reserved.
// This code is licensed under the MIT License (MIT).
// Autor: Luis Borrero
//
//*********************************************************

#include "Common/CoreSystem.h"
#include "Crate.h"



_Use_decl_annotations_
/// <summary>
/// Core function to call main program
/// </summary>
/// <param name="hInstance">instance generate by windows</param>
/// <param name=""></param>
/// <param name=""></param>
/// <param name="nCmdShow">show windows?</param>
/// <returns>return int response</returns>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{	
	Crate crate;
	return CoreSystem::Run(&crate,hInstance, nCmdShow);
}
