#include "WinAPI.h"

void LoadResourcePng(Image &dest, HMODULE hModule, LPCWSTR lpName)
{
	Png png;
	// lock & load resource
	HRSRC hRsrc=FindResource(hModule,lpName,RT_RCDATA);
	u8* pngData=(u8*)LoadResource(hModule,hRsrc);
	// load as png object
	png.LoadPng(pngData,SizeofResource(hModule,hRsrc));
	// png->bmp
	dest.CreateFromPng(&png);
}
