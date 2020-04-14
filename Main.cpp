#include <d3d9.h>
#include <d3dx9.h>
#include "D3DFont.h"
#include "color.h"
#include "detour.h"
#include "int.h"
#include "Hacks.h"
#include "d3dmenu.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

D3D9Menu *pMenu = NULL;
CD3DFont *pFont;

INT WallHacker, DisableFog;
INT ChamsColors = TRUE;

bool Color = true;
bool Create;

char *folder[]	= { "[+]", "[-]" };
char *onoff[]	= { "[OFF]", "[ON]" };
char *Chams[]	= { "[OFF]", "[WallHack]", "[Chams]" };

LPDIRECT3DTEXTURE9 pRed, pGreen;

void RebuildMenu(void)
{
	pMenu->AddFolder("[D3D Options]", folder, &folder1, 2);

    if(folder1)
    {
		pMenu->AddItem("WallHack", Chams, &WallHacker, 3);
		pMenu->AddItem("Disable Fog", onoff, &DisableFog, 2);
	}
}

HRESULT WINAPI myPresent ( LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	pFont = new CD3DFont("Arial", 10, DT_SHADOW);
	pFont->InitDeviceObjects(pDevice);
	pFont->RestoreDeviceObjects();

	if(pMenu == NULL) {
		pMenu = new D3D9Menu("BlackShot D3D v1.0");
	} else {
		pMenu->MenuShow(12, 10, pFont, pDevice);
		if(pMenu->Mmax == 0) RebuildMenu();
		pMenu->MenuNav();
		
	}
	Hacks();

	if(ChamsColors)
	{
		GenerateTexture(pDevice, &pRed, RED);
		GenerateTexture(pDevice, &pGreen, GREEN);
	}

	delete pFont;
	return pPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI DrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE A, INT B, UINT C, UINT D, UINT E, UINT F)
{
	if(pDevice->GetStreamSource(NULL, &Stream_Data, &OffsetsInBytes, &m_Stride) == D3D_OK) { Stream_Data->Release(); }

	if(WallHacker)
	{
		if(m_Stride == 36)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			oDrawIndexedPrimitive(pDevice, A, B, C, D, E, F);
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		}
	}
	if(WallHacker == 2)
	{
		if(m_Stride == 36)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			pDevice->SetTexture(NULL, pGreen);
			oDrawIndexedPrimitive(pDevice, A, B, C, D, E, F);
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			pDevice->SetTexture(NULL, pRed);
		}
	}
	if(DisableFog)
	{
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
	return oDrawIndexedPrimitive(pDevice, A, B, C, D, E, F);
}

INT D3D(VOID)
{
	HINSTANCE hD3D; DWORD vTable[105];
	hD3D = NULL;

	do {
		hD3D = GetModuleHandle("d3d9.dll");
		if(!hD3D) Sleep(10);
	} while(!hD3D);

	if(D3Ddiscover((PVOID) &vTable[0], 420) == 0) return NULL;
	{
		pPresent = (oPresent)DetourCreate((DWORD)vTable[17], (DWORD)myPresent,12);
		//oDrawIndexedPrimitive = (tDrawIndexedPrimitive)DetourCreate((DWORD)vTable[82], (DWORD)&DrawIndexedPrimitive, 5);

		Sleep(100);
	}
	return FALSE;
}


BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH) {
	    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)D3D, NULL, NULL, NULL);
	}
	return TRUE;
}