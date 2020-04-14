
LPDIRECT3DVERTEXBUFFER9 Stream_Data;
UINT m_Stride; UINT OffsetsInBytes;

typedef HRESULT(WINAPI *tDrawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE A, INT B, UINT C, UINT D, UINT E, UINT F); 
tDrawIndexedPrimitive oDrawIndexedPrimitive;

typedef HRESULT(WINAPI *oPresent) (LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
oPresent pPresent;

DWORD DetourCreate(DWORD SrcVA, DWORD DstVA, DWORD Size)
{
	#define SIZEOF_JMP_NEAR  5

	if(SrcVA && DstVA && Size > 0)
	{
		DWORD DetourVA = (DWORD) 
		VirtualAlloc( NULL, Size + SIZEOF_JMP_NEAR, 
		MEM_COMMIT | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE );
		
		if(DetourVA == 0)
			return(0);

		DWORD dwProtect;

		if(VirtualProtect((PVOID)SrcVA, Size, PAGE_EXECUTE_READWRITE, &dwProtect ))
		{
			for(DWORD i = 0; i < Size; i++) {
				*(PBYTE)(DetourVA + i) = *(PBYTE)(SrcVA + i);
			}

			*(PBYTE)(DetourVA + Size + 0) = 0xE9;
			*(PDWORD)(DetourVA + Size + 1) = (SrcVA - DetourVA - SIZEOF_JMP_NEAR);

	    	*(PBYTE)(SrcVA + 0) = 0xE9;
			*(PDWORD)(SrcVA + 1) = (DstVA - SrcVA - SIZEOF_JMP_NEAR);

			VirtualProtect((PVOID)SrcVA, Size, dwProtect, &dwProtect );

			VirtualProtect((PVOID)DetourVA, Size + SIZEOF_JMP_NEAR, PAGE_EXECUTE_READ, &dwProtect);

	    	return DetourVA;
		}
	}
	return(NULL);
}

PVOID D3Ddiscover(void *tbl, int size)
{
	HWND hWnd;
	VOID *pInterface = NULL;
	D3DPRESENT_PARAMETERS d3dpp; 

	if((hWnd = CreateWindowEx(NULL, WC_DIALOG, "", WS_OVERLAPPED, 0, 0, 50, 50, NULL, NULL, NULL, NULL)) == NULL)
		return FALSE;

	ShowWindow(hWnd, SW_HIDE);
	LPDIRECT3D9	pD3D;
	LPDIRECT3DDEVICE9 pD3Ddev;

	if((pD3D = Direct3DCreate9(D3D_SDK_VERSION))!=NULL) 
	{
	    ZeroMemory(&d3dpp, sizeof(d3dpp));
	    d3dpp.Windowed         = TRUE;
		d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	    d3dpp.hDeviceWindow    = hWnd;
	    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	    d3dpp.BackBufferWidth  = d3dpp.BackBufferHeight = 600;
		pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&pD3Ddev);
		if (pD3Ddev)  {
		    pInterface = (PDWORD)*(DWORD *)pD3Ddev;
			memcpy(tbl,(void *)pInterface,size);
			pD3Ddev->Release();
		}
		pD3D->Release();
	}
	DestroyWindow(hWnd);
	return pInterface;
}

HRESULT GenerateTexture(LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 *D3DTexture, DWORD D3DColor)
{
	if(FAILED(pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, D3DTexture, NULL)))
		return E_FAIL;

	WORD Color = ((WORD)((D3DColor >> 28) & 0xF) << 12)
				|(WORD)(((D3DColor >> 20) & 0xF) <<  8)
				|(WORD)(((D3DColor >> 12) & 0xF) << 04)
				|(WORD)(((D3DColor >> 04) & 0xF) << 00);

	D3DLOCKED_RECT pRect;
	(*D3DTexture)->LockRect(NULL, &pRect, NULL, NULL);
	WORD *pBits = (PWORD)pRect.pBits;

	for(INT i=0; i<8*8; i++)
		*pBits++ = Color;

	(*D3DTexture)->UnlockRect(NULL);

	return S_OK;
}