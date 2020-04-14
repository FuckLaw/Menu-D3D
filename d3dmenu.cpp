#include "D3DFont.h"
#include "d3dmenu.h"
#include "color.h"
#include "stdio.h"

void D3D9Menu::AddFolder(char *txt, char **opt, int *var, int maxvalue)
{
	MENU[Mmax]->typ=MENUFOLDER;
	MENU[Mmax]->txt=txt;
    MENU[Mmax]->opt=opt;
	MENU[Mmax]->var=var;
	MENU[Mmax]->maxvalue=maxvalue;
	Mmax++;
}

void D3D9Menu::AddCategory(char *txt, char **opt, int *var, int maxvalue)
{
	MENU[Mmax]->typ=MENUCAT;
	MENU[Mmax]->txt=txt;
	MENU[Mmax]->opt=opt;
	MENU[Mmax]->var=var;
	MENU[Mmax]->maxvalue=maxvalue;
	Mmax++;
}

void D3D9Menu::AddItem(char *txt, char **opt, int *var, int maxvalue)
{
	MENU[Mmax]->typ=MENUITEM;
	MENU[Mmax]->txt=txt;
	MENU[Mmax]->opt=opt;
	MENU[Mmax]->var=var;
	MENU[Mmax]->maxvalue=maxvalue;
	Mmax++;
}

void D3D9Menu::DrawTextR(int x,int y,DWORD color,char *text, CD3DFont*  pFont1)
{
	pFont1->DrawTextA((float)x, (float)y, color, text, DT_RIGHT|DT_NOCLIP);  						
}

void D3D9Menu::DrawTextL(int x,int y,DWORD color,char *text, CD3DFont*  pFont1)
{
	pFont1->DrawTextA((float)x, (float)y, color, text, DT_LEFT|DT_NOCLIP);  
}

void D3D9Menu::DrawTextC(int x,int y,DWORD color,char *text, CD3DFont*  pFont1)
{
	pFont1->DrawTextA((float)x, (float)y, color, text, DT_CENTER|DT_NOCLIP);  
}

VOID __stdcall CMenu_DrawCronersBox(INT xx,INT yy,INT hh,INT ww,LPDIRECT3DDEVICE9 pDevice)
{
	D3DRECT rec;
	FLOAT x,y,w,h;
	    
	struct vertex 
	{
		FLOAT x,y,z,t;
		DWORD Color;
	};

	vertex V[8];

	x = (float)xx; y = (float)yy; h = (float)hh; w = (float)ww;

	V[0].Color = V[1].Color = V[2].Color = V[3].Color = BLACK;
	V[4].Color = V[5].Color = V[6].Color = V[7].Color = BLACK;

	V[0].z  = V[1].z = V[2].z  = V[3].z  = 0;
	V[4].z  = V[5].z = V[6].z  = V[7].z  = 0;
	V[0].t  = V[1].t = V[2].t  = V[3].t  = 0;
	V[4].t  = V[5].t = V[6].t  = V[7].t  = 0;

	V[0].x = V[1].x = x;
	V[0].y = V[2].y = y + h;
	V[1].y = V[3].y = y;
	V[2].x = V[3].x = x + w;
	V[4].x = V[5].x = x;
	V[4].y = V[6].y = y + h;
	V[5].y = V[7].y = y;
	V[6].x = V[7].x = x + w;

	pDevice->SetTexture(0, NULL);
	pDevice->SetPixelShader( NULL );
	pDevice->SetRenderState(D3DRS_CLIPPING,FALSE); 
	pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,V,sizeof(vertex));

	rec.x1 = xx;rec.y1 = yy;rec.x2 = xx+ww;rec.y2 = yy+1;
	pDevice->Clear(1,&rec,D3DCLEAR_TARGET,GREY,0,0);
	rec.x1 = xx;rec.y1 = hh+yy;rec.x2 = xx+ww;rec.y2 = hh+yy+1;
	pDevice->Clear(1,&rec,D3DCLEAR_TARGET,GREY,0,0);
	rec.x1 = xx;rec.y1 = yy;rec.x2 = xx+1;rec.y2 = yy+hh;
	pDevice->Clear(1,&rec,D3DCLEAR_TARGET,GREY,0,0);
	rec.x1 = xx+ww;rec.y1 = yy;rec.x2 = xx+ww+1;rec.y2 = yy+hh;
	pDevice->Clear(1,&rec,D3DCLEAR_TARGET,GREY,0,0);

	return;
}

void FillARGB( int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice ) 
{
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	struct QuadVertex {
        float x,y,z,rhw;
        DWORD dwColor;
    };
    QuadVertex qV[4];

    qV[0].dwColor = qV[1].dwColor = qV[2].dwColor = qV[3].dwColor = color;
    qV[0].z   = qV[1].z   = qV[2].z   = qV[3].z   = 0.0f;
    qV[0].rhw = qV[1].rhw = qV[2].rhw = qV[3].rhw = 0.0f;

    qV[0].x = (float)x;
    qV[0].y = (float)(y + h);
    qV[1].x = (float)x;
    qV[1].y = (float)y;
    qV[2].x = (float)(x + w);
    qV[2].y = (float)(y + h);
    qV[3].x = (float)(x + w);
    qV[3].y = (float)y;

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,true );
    pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
    pDevice->SetFVF(D3D_FVF); 
    pDevice->SetTexture(0, NULL);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,qV,sizeof(QuadVertex));
}

void D3D9Menu::MenuShow(int x, int y, CD3DFont* pFont1, LPDIRECT3DDEVICE9 pDevice)
{
	if (!Mvisible) return;
	SetRect(&rect, x + 70, y - 2, x + 70, y - 2);
	CMenu_DrawCronersBox((INT)x - 2, (INT)y - 3, 18, 154, pDevice);//Top Box
	CMenu_DrawCronersBox((INT)x - 2, (INT)y + 22, 10 + Mmax * 16, 154, pDevice);//Middle Box

	if (Mtitle[0]) {
		DrawTextC((x * 7) + 5, y, YELLOW, Mtitle, pFont1);
		DrawTextC((x * 7) + 5, y + 10, MENUCOLOR_TITLE, "", pFont1);
		y += (Mysize + 8);
	}
	for (i = 0; i < Mmax; i++) {
		val = (MENU[i]->var) ? (*MENU[i]->var) : 0;
		sprintf_s(text, "%s", MENU[i]->txt);
		if (i == Mpos) {
			color = MCOLOR_CURRENT;
			sprintf_s(text, "%s", MENU[i]->txt);
		}
		else if (MENU[i]->typ == MENUFOLDER)
			color = MCOLOR_FOLDER;
		else if (MENU[i]->typ == MENUTEXT)
			color = MCOLOR_TEXT;
		else
			color = (val) ? MCOLOR_ACTIVE : MCOLOR_INACTIVE;

		if (MENU[i]->opt)
		{
			if (MENU[i]->typ == MENUFOLDER) {
				DrawTextL(x + 3, y, color, text, pFont1);
				y = y + 2;
			}
			else {
				DrawTextL(x + 3, y, color, text, pFont1);
			}
		}

		if (MENU[i]->opt) {
			if (MENU[i]->typ == MENUTEXT)
				DrawTextR((x + Mxofs), y, color, (char *)MENU[i]->opt, pFont1);
			else
				DrawTextR((x + Mxofs), y, color, (char *)MENU[i]->opt[val], pFont1);
		}

		y += Mysize;
	}
	YPOS = (int)y;
}

void D3D9Menu::MenuNav(void)
{
  if (GetAsyncKeyState(VK_INSERT)&1) Mvisible=(!Mvisible);
  if (!Mvisible) return;
 

  if (GetAsyncKeyState(VK_UP)&1) {
		do {
			Mpos--;
			if (Mpos<0)  Mpos=Mmax-1;
		} while (MENU[Mpos]->typ==MENUTEXT);
		
  } else if (GetAsyncKeyState(VK_DOWN)&1) {
		do {
			Mpos++;
		    if (Mpos==Mmax) Mpos=0;
		} while (MENU[Mpos]->typ==MENUTEXT);
			
  } else if (MENU[Mpos]->var) {
		int dir=0;

		if (GetAsyncKeyState(VK_LEFT )&1 && *MENU[Mpos]->var > 0) dir=-1;
		if (GetAsyncKeyState(VK_RIGHT)&1 && *MENU[Mpos]->var < (MENU[Mpos]->maxvalue-1)) dir=1;
		if (dir) {
			*MENU[Mpos]->var += dir;
			if (MENU[Mpos]->typ==MENUFOLDER)	Mmax=0;
			if (MENU[Mpos]->typ==MENUCAT)		Mmax=0;  
		}
	}
}