#ifndef __SHADOWWINDOW_H__
#define __SHADOWWINDOW_H__


/*
在窗体异形的时候，会有问题。窗口不规则，仍然画的是方形的阴影
使用方法：
CShadowWindow::Initialize(GetPaintManager()->GetInstance());
m_WndShadow.Create(m_hWnd);
m_WndShadow.SetSize(5);
m_WndShadow.SetPosition(0, 0);

*/

#include <windows.h>
#include <math.h>

namespace DuiLib
{
#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif

	// Vista aero related message
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

	typedef BOOL(WINAPI *pfnUpdateLayeredWindow)(HWND hWnd,HDC hdcDst,POINT *pptDst,
		SIZE *psize,HDC hdcSrc,POINT *pptSrc,COLORREF crKey,
		BLENDFUNCTION *pblend,DWORD dwFlags);
	typedef HRESULT(WINAPI *pfnDwmIsCompositionEnabled)(BOOL *pfEnabled);
	typedef void(__stdcall *pfnAdjustShadows)(void * pCallbackData,LPBITMAPINFO pBmpinfo,UINT32 *pShadBits,long lxShadowSize,long lcxOffset,long lcyOffset);



	class CShadowWindow
	{
	public:
		static CShadowWindow * pCShadowWindow_;
		static HWND hParent_;
	public:
		enum ShadowStatus
		{
			SS_ENABLED=1,	// Shadow is enabled, if not, the following one is always FALSE
			SS_VISABLE=1<<1,	// Shadow window is visible
			SS_PARENTVISIBLE=1<<2,	// Parent window is visible, if not, the above one is always FALSE
			SS_DISABLEDBYAERO=1<<3	// Shadow is enabled, but do not show because areo is enabled
		};
	protected:
#pragma warning(push)
#pragma warning(disable:4786)

#pragma warning(pop) 
		static HINSTANCE s_hInstance;
		static pfnUpdateLayeredWindow s_UpdateLayeredWindow;

		static BOOL s_bVista;	// Whether running on Win Vista
		static pfnDwmIsCompositionEnabled s_DwmIsCompositionEnabled;

		LONG m_OriParentProc;	// Original WndProc of parent window
		BYTE m_Status;
		pfnAdjustShadows m_fAdjustShadows;
		void * m_pCallbackData;
		unsigned char m_nDarkness;	// Darkness, transparency of blurred area
		unsigned char m_nSharpness;	// Sharpness, width of blurred border of shadow window
		signed char m_nSize;	// Shadow window size, relative to parent window size

		// The X and Y offsets of shadow window,
		// relative to the parent window, at center of both windows (not top-left corner), signed
		signed char m_nxOffset;
		signed char m_nyOffset;
		// Restore last parent window size, used to determine the update strategy when parent window is resized
		LPARAM m_WndSize;
		// Set this to TRUE if the shadow should not be update until next WM_PAINT is received
		BOOL m_bUpdate;
		COLORREF m_Color;	// Color of shadow
	public:
		HWND m_hWnd;
	protected:
		//static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK ParentProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
		{
			CShadowWindow *pThis=NULL;
			if (hParent_==hwnd)
			{
				pThis=pCShadowWindow_;
			}
#pragma warning(push)
#pragma warning(disable: 4312)
			WNDPROC pDefProc=(WNDPROC)pThis->m_OriParentProc;
#pragma warning(pop)

			switch(uMsg)
			{
			case WM_MOVE:
				if(pThis->m_Status & SS_VISABLE)
				{
					RECT WndRect;
					GetWindowRect(hwnd,&WndRect);
					SetWindowPos(pThis->m_hWnd,0,
						WndRect.left+pThis->m_nxOffset-pThis->m_nSize,WndRect.top+pThis->m_nyOffset-pThis->m_nSize,
						0,0,SWP_NOSIZE|SWP_NOACTIVATE);
				}
				break;

			case WM_SIZE:
				if(pThis->m_Status & SS_ENABLED && !(pThis->m_Status & SS_DISABLEDBYAERO))
				{
					if(SIZE_MAXIMIZED==wParam||SIZE_MINIMIZED==wParam)
					{
						ShowWindow(pThis->m_hWnd,SW_HIDE);
						pThis->m_Status&=~SS_VISABLE;
					}
					else
					{
						LONG lParentStyle=GetWindowLong(hwnd,GWL_STYLE);
						if(WS_VISIBLE & lParentStyle)	// Parent may be resized even if invisible
						{
							pThis->m_Status|=SS_PARENTVISIBLE;
							if(!(pThis->m_Status & SS_VISABLE))
							{
								pThis->m_Status|=SS_VISABLE;
								// Update before show, because if not, restore from maximized will
								// see a glance misplaced shadow
								pThis->Update(hwnd);
								ShowWindow(pThis->m_hWnd,SW_SHOWNA);
								// If restore from minimized, the window region will not be updated until WM_PAINT:(
								pThis->m_bUpdate=TRUE;
							}
							// Awful! It seems that if the window size was not decreased
							// the window region would never be updated until WM_PAINT was sent.
							// So do not Update() until next WM_PAINT is received in this case
							else if(LOWORD(lParam)>LOWORD(pThis->m_WndSize)||HIWORD(lParam)>HIWORD(pThis->m_WndSize))
								pThis->m_bUpdate=TRUE;
							else
								pThis->Update(hwnd);
						}
					}
					pThis->m_WndSize=lParam;
				}
				break;
			case WM_PAINT:
				{
					if(pThis->m_bUpdate)
					{
						pThis->Update(hwnd);
						pThis->m_bUpdate=FALSE;
					}
					break;
				}
			case WM_EXITSIZEMOVE:
				if(pThis->m_Status & SS_VISABLE)
				{
					pThis->Update(hwnd);
				}
				break;

			case WM_SHOWWINDOW:
				if(pThis->m_Status & SS_ENABLED && !(pThis->m_Status & SS_DISABLEDBYAERO))
				{
					LRESULT lResult=pDefProc(hwnd,uMsg,wParam,lParam);
					if(!wParam)	// the window is being hidden
					{
						ShowWindow(pThis->m_hWnd,SW_HIDE);
						pThis->m_Status&=~(SS_VISABLE|SS_PARENTVISIBLE);
					}
					else
					{
						// 				pThis->m_Status |= SS_VISABLE | SS_PARENTVISIBLE;
						// 				ShowWindow(pThis->m_hWnd, SW_SHOWNA);
						// 				pThis->Update(hwnd);
						pThis->m_bUpdate=TRUE;
						pThis->Show(hwnd);
					}
					return lResult;
				}
				break;

			case WM_DESTROY:
				DestroyWindow(pThis->m_hWnd);
				break;

			case WM_NCDESTROY:
				pCShadowWindow_=NULL;
				break;

			case WM_DWMCOMPOSITIONCHANGED:
				{
					BOOL bAero=FALSE;
					if(s_DwmIsCompositionEnabled)	// "if" is actually not necessary here :P
						s_DwmIsCompositionEnabled(&bAero);
					if(bAero)
						pThis->m_Status|=SS_DISABLEDBYAERO;
					else
						pThis->m_Status&=~SS_DISABLEDBYAERO;

					pThis->Show(hwnd);
				}
				break;
			}
			// Call the default(original) window procedure for other messages or messages processed but not returned
			return pDefProc(hwnd,uMsg,wParam,lParam);
		}
		void Update(HWND hParent)
		{
			RECT WndRect;
			GetWindowRect(hParent,&WndRect);
			int nShadWndWid=WndRect.right-WndRect.left+m_nSize*2;
			int nShadWndHei=WndRect.bottom-WndRect.top+m_nSize*2;

			// Create the alpha blending bitmap
			BITMAPINFO bmi;        // bitmap header

			ZeroMemory(&bmi,sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth=nShadWndWid;
			bmi.bmiHeader.biHeight=nShadWndHei;
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biBitCount=32;         // four 8-bit components
			bmi.bmiHeader.biCompression=BI_RGB;
			bmi.bmiHeader.biSizeImage=nShadWndWid * nShadWndHei*4;

			BYTE *pvBits;          // pointer to DIB section
			HBITMAP hbitmap=CreateDIBSection(NULL,&bmi,DIB_RGB_COLORS,(void **)&pvBits,NULL,0);

			ZeroMemory(pvBits,bmi.bmiHeader.biSizeImage);
			MakeShadow((UINT32 *)pvBits,hParent,&WndRect);
			if(m_fAdjustShadows)
				m_fAdjustShadows(m_pCallbackData,&bmi,(UINT32 *)pvBits,m_nSize,m_nxOffset,m_nyOffset);
			HDC hMemDC=CreateCompatibleDC(NULL);
			HBITMAP hOriBmp=(HBITMAP)SelectObject(hMemDC,hbitmap);

			POINT ptDst={WndRect.left+m_nxOffset-m_nSize,WndRect.top+m_nyOffset-m_nSize};
			POINT ptSrc={0,0};
			SIZE WndSize={nShadWndWid,nShadWndHei};
			BLENDFUNCTION blendPixelFunction={AC_SRC_OVER,0,255,AC_SRC_ALPHA};

			MoveWindow(m_hWnd,ptDst.x,ptDst.y,nShadWndWid,nShadWndHei,FALSE);

			BOOL bRet=s_UpdateLayeredWindow(m_hWnd,NULL,&ptDst,&WndSize,hMemDC,
				&ptSrc,0,&blendPixelFunction,ULW_ALPHA);

			_ASSERT(bRet); // something was wrong....

			// Delete used resources
			SelectObject(hMemDC,hOriBmp);
			DeleteObject(hbitmap);
			DeleteDC(hMemDC);
		}

		// Fill in the shadow window alpha blend bitmap with shadow image pixels
		void MakeShadow(UINT32 *pShadBits,HWND hParent,RECT *rcParent)
		{
			// The shadow algorithm:
			// Get the region of parent window,
			// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
			// Apply modified (with blur effect) morphologic dilation to make the blurred border
			// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

			// Get the region of parent window,
			// Create a full rectangle region in case of the window region is not defined
			HRGN hParentRgn=CreateRectRgn(0,0,rcParent->right-rcParent->left,rcParent->bottom-rcParent->top);
			GetWindowRgn(hParent,hParentRgn);

			// Determine the Start and end point of each horizontal scan line
			SIZE szParent={rcParent->right-rcParent->left,rcParent->bottom-rcParent->top};
			SIZE szShadow={szParent.cx+2*m_nSize,szParent.cy+2*m_nSize};
			// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
			int nAnchors=max(szParent.cy,szShadow.cy);	// # of anchor points pares
			int(*ptAnchors)[2]=new int[nAnchors+2][2];
			int(*ptAnchorsOri)[2]=new int[szParent.cy][2];	// anchor points, will not modify during erosion
			ptAnchors[0][0]=szParent.cx;
			ptAnchors[0][1]=0;
			ptAnchors[nAnchors+1][0]=szParent.cx;
			ptAnchors[nAnchors+1][1]=0;
			if(m_nSize>0)
			{
				// Put the parent window anchors at the center
				for(int i=0; i<m_nSize; i++)
				{
					ptAnchors[i+1][0]=szParent.cx;
					ptAnchors[i+1][1]=0;
					ptAnchors[szShadow.cy-i][0]=szParent.cx;
					ptAnchors[szShadow.cy-i][1]=0;
				}
				ptAnchors+=m_nSize;
			}
			for(int i=0; i<szParent.cy; i++)
			{
				// find start point
				int j;
				for(j=0; j<szParent.cx; j++)
				{
					if(PtInRegion(hParentRgn,j,i))
					{
						ptAnchors[i+1][0]=j+m_nSize;
						ptAnchorsOri[i][0]=j;
						break;
					}
				}

				if(j>=szParent.cx)	// Start point not found
				{
					ptAnchors[i+1][0]=szParent.cx;
					ptAnchorsOri[i][1]=0;
					ptAnchors[i+1][0]=szParent.cx;
					ptAnchorsOri[i][1]=0;
				}
				else
				{
					// find end point
					for(j=szParent.cx-1; j>=ptAnchors[i+1][0]; j--)
					{
						if(PtInRegion(hParentRgn,j,i))
						{
							ptAnchors[i+1][1]=j+1+m_nSize;
							ptAnchorsOri[i][1]=j+1;
							break;
						}
					}
				}
				// 		if(0 != ptAnchorsOri[i][1])
				// 			_RPTF2(_CRT_WARN, "%d %d\n", ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
			}

			if(m_nSize>0)
				ptAnchors-=m_nSize;	// Restore pos of ptAnchors for erosion
			int(*ptAnchorsTmp)[2]=new int[nAnchors+2][2];	// Store the result of erosion
			// First and last line should be empty
			ptAnchorsTmp[0][0]=szParent.cx;
			ptAnchorsTmp[0][1]=0;
			ptAnchorsTmp[nAnchors+1][0]=szParent.cx;
			ptAnchorsTmp[nAnchors+1][1]=0;
			int nEroTimes=0;
			// morphologic erosion
			for(int i=0; i<m_nSharpness-m_nSize; i++)
			{
				nEroTimes++;
				//ptAnchorsTmp[1][0] = szParent.cx;
				//ptAnchorsTmp[1][1] = 0;
				//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
				//ptAnchorsTmp[szParent.cy + 1][1] = 0;
				for(int j=1; j < nAnchors+1; j++)
				{
					ptAnchorsTmp[j][0]=max(ptAnchors[j-1][0],max(ptAnchors[j][0],ptAnchors[j+1][0]))+1;
					ptAnchorsTmp[j][1]=min(ptAnchors[j-1][1],min(ptAnchors[j][1],ptAnchors[j+1][1]))-1;
				}
				// Exchange ptAnchors and ptAnchorsTmp;
				int(*ptAnchorsXange)[2]=ptAnchorsTmp;
				ptAnchorsTmp=ptAnchors;
				ptAnchors=ptAnchorsXange;
			}

			// morphologic dilation
			ptAnchors+=(m_nSize<0?-m_nSize:0)+1;	// now coordinates in ptAnchors are same as in shadow window
			// Generate the kernel
			int nKernelSize=m_nSize>m_nSharpness?m_nSize:m_nSharpness;
			int nCenterSize=m_nSize > m_nSharpness?(m_nSize-m_nSharpness):0;
			UINT32 *pKernel=new UINT32[(2*nKernelSize+1) * (2*nKernelSize+1)];
			UINT32 *pKernelIter=pKernel;
			for(int i=0; i<=2*nKernelSize; i++)
			{
				for(int j=0; j<=2*nKernelSize; j++)
				{
					double dLength=sqrt((i-nKernelSize) * (i-nKernelSize)+(j-nKernelSize) * (double)(j-nKernelSize));
					if(dLength<nCenterSize)
						*pKernelIter=m_nDarkness<<24|PreMultiply(m_Color,m_nDarkness);
					else if(dLength<=nKernelSize)
					{
						UINT32 nFactor=((UINT32)((1-(dLength-nCenterSize)/(m_nSharpness+1)) * m_nDarkness));
						*pKernelIter=nFactor<<24|PreMultiply(m_Color,nFactor);
					}
					else
						*pKernelIter=0;
					//TRACE("%d ", *pKernelIter >> 24);
					pKernelIter++;
				}
				//TRACE("\n");
			}
			// Generate blurred border
			for(int i=nKernelSize; i<szShadow.cy-nKernelSize; i++)
			{
				int j;
				if(ptAnchors[i][0]<ptAnchors[i][1])
				{

					// Start of line
					for(j=ptAnchors[i][0];
						j<min(max(ptAnchors[i-1][0],ptAnchors[i+1][0])+1,ptAnchors[i][1]);
						j++)
					{
						for(int k=0; k<=2*nKernelSize; k++)
						{
							UINT32 *pPixel=pShadBits+
								(szShadow.cy-i-1+nKernelSize-k) * szShadow.cx+j-nKernelSize;
							UINT32 *pKernelPixel=pKernel+k * (2*nKernelSize+1);
							for(int l=0; l<=2*nKernelSize; l++)
							{
								if(*pPixel<*pKernelPixel)
									*pPixel=*pKernelPixel;
								pPixel++;
								pKernelPixel++;
							}
						}
					}	// for() start of line

					// End of line
					for(j=max(j,min(ptAnchors[i-1][1],ptAnchors[i+1][1])-1);
						j<ptAnchors[i][1];
						j++)
					{
						for(int k=0; k<=2*nKernelSize; k++)
						{
							UINT32 *pPixel=pShadBits+
								(szShadow.cy-i-1+nKernelSize-k) * szShadow.cx+j-nKernelSize;
							UINT32 *pKernelPixel=pKernel+k * (2*nKernelSize+1);
							for(int l=0; l<=2*nKernelSize; l++)
							{
								if(*pPixel<*pKernelPixel)
									*pPixel=*pKernelPixel;
								pPixel++;
								pKernelPixel++;
							}
						}
					}	// for() end of line

				}
			}	// for() Generate blurred border

			// Erase unwanted parts and complement missing
			UINT32 clCenter=m_nDarkness<<24|PreMultiply(m_Color,m_nDarkness);
			for(int i=min(nKernelSize,max(m_nSize-m_nyOffset,0));
				i<max(szShadow.cy-nKernelSize,min(szParent.cy+m_nSize-m_nyOffset,szParent.cy+2*m_nSize));
				i++)
			{
				UINT32 *pLine=pShadBits+(szShadow.cy-i-1) * szShadow.cx;
				if(i-m_nSize+m_nyOffset<0||i-m_nSize+m_nyOffset>=szParent.cy)	// Line is not covered by parent window
				{
					for(int j=ptAnchors[i][0]; j<ptAnchors[i][1]; j++)
					{
						*(pLine+j)=clCenter;
					}
				}
				else
				{
					for(int j=ptAnchors[i][0];
						j<min(ptAnchorsOri[i-m_nSize+m_nyOffset][0]+m_nSize-m_nxOffset,ptAnchors[i][1]);
						j++)
						*(pLine+j)=clCenter;
					for(int j=max(ptAnchorsOri[i-m_nSize+m_nyOffset][0]+m_nSize-m_nxOffset,0);
						j<min(ptAnchorsOri[i-m_nSize+m_nyOffset][1]+m_nSize-m_nxOffset,szShadow.cx);
						j++)
						*(pLine+j)=0;
					for(int j=max(ptAnchorsOri[i-m_nSize+m_nyOffset][1]+m_nSize-m_nxOffset,ptAnchors[i][0]);
						j<ptAnchors[i][1];
						j++)
						*(pLine+j)=clCenter;
				}
			}

			// Delete used resources
			delete[](ptAnchors-(m_nSize<0?-m_nSize:0)-1);
			delete[] ptAnchorsTmp;
			delete[] ptAnchorsOri;
			delete[] pKernel;
			DeleteObject(hParentRgn);

		}

		// Helper to calculate the alpha-premultiled value for a pixel
		inline DWORD PreMultiply(COLORREF cl,unsigned char nAlpha)
		{
			// It's strange that the byte order of RGB in 32b BMP is reverse to in COLORREF
			return (GetRValue(cl) * (DWORD)nAlpha/255)<<16|
				(GetGValue(cl) * (DWORD)nAlpha/255)<<8|
				(GetBValue(cl) * (DWORD)nAlpha/255);
		}

		// Show or hide the shadow, depending on the enabled status stored in m_Status
		void Show(HWND hParentWnd)
		{
			m_Status&=SS_ENABLED|SS_DISABLEDBYAERO;

			if((m_Status & SS_ENABLED)&&!(m_Status & SS_DISABLEDBYAERO))	// Enabled
			{
				// Determine the show state of shadow according to parent window's state
				LONG lParentStyle=GetWindowLong(hParentWnd,GWL_STYLE);

				if(WS_VISIBLE & lParentStyle)	// Parent visible
				{
					m_Status|=SS_PARENTVISIBLE;

					// Parent is normal, show the shadow
					if(!((WS_MAXIMIZE|WS_MINIMIZE) & lParentStyle))	// Parent visible but does not need shadow
						m_Status|=SS_VISABLE;
				}
			}

			if(m_Status & SS_VISABLE)
			{
				ShowWindow(m_hWnd,SW_SHOWNA);
				Update(hParentWnd);
			}
			else
				ShowWindow(m_hWnd,SW_HIDE);
		}
	public:
		static BOOL Initialize(HINSTANCE hInstance,BOOL bUseAero=FALSE)
		{
			if(NULL!=s_UpdateLayeredWindow)
				return FALSE;
			HMODULE hSysDll=LoadLibrary(_T("USER32.DLL"));
			s_UpdateLayeredWindow=
				(pfnUpdateLayeredWindow)GetProcAddress(hSysDll,
				"UpdateLayeredWindow");
			if(NULL==s_UpdateLayeredWindow)
				return FALSE;
			/*
			   裁剪冰绡(2766556)-- 注释掉就可以了，不然VISTA以上，不会画阴影,因为DUILIB一般都会只保留客户区?
			*/
			//-------
			if(bUseAero)
			{
				hSysDll=LoadLibrary(_T("dwmapi.dll"));
				if(hSysDll)	// Loaded dwmapi.dll succefull, must on Vista or above
				{
					s_bVista=TRUE;
					s_DwmIsCompositionEnabled=
						(pfnDwmIsCompositionEnabled)GetProcAddress(hSysDll,
						"DwmIsCompositionEnabled");
				}
			}
			//------
			s_hInstance=hInstance;

			WNDCLASSEX wcex;
			memset(&wcex,0,sizeof(wcex));
			wcex.cbSize=sizeof(WNDCLASSEX);
			wcex.style=CS_HREDRAW|CS_VREDRAW;
			wcex.lpfnWndProc=DefWindowProc;
			wcex.cbClsExtra=0;
			wcex.cbWndExtra=0;
			wcex.hInstance=hInstance;
			wcex.hIcon=NULL;
			wcex.hCursor=LoadCursor(NULL,IDC_ARROW);
			wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
			wcex.lpszMenuName=NULL;
			wcex.lpszClassName=_T("SilentMoon.ShadowWindow.Class");
			wcex.hIconSm=NULL;
			RegisterClassEx(&wcex);
			return TRUE;
		}

		void Create(HWND hParentWnd)
		{
			if(NULL==s_UpdateLayeredWindow)
				return;
			_ASSERT(s_hInstance!=INVALID_HANDLE_VALUE);

			hParent_=hParentWnd;
			pCShadowWindow_=this;


			m_hWnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TRANSPARENT,_T("SilentMoon.ShadowWindow.Class"),NULL, WS_POPUPWINDOW,CW_USEDEFAULT,0,0,0,hParentWnd,NULL,s_hInstance,NULL);

			m_Status=SS_ENABLED;
			BOOL bAero=FALSE;
			if(s_DwmIsCompositionEnabled)
				s_DwmIsCompositionEnabled(&bAero);
			if(bAero)
				m_Status|=SS_DISABLEDBYAERO;
			Show(hParentWnd);
			m_OriParentProc=GetWindowLong(hParentWnd,GWL_WNDPROC);
#pragma warning(push)
#pragma warning(disable: 4311)
			SetWindowLong(hParentWnd,GWL_WNDPROC,(LONG)ParentProc);
#pragma warning(pop)
		}
		BOOL SetSize(int NewSize=0)
		{
			if(NewSize>20||NewSize<-20)
				return FALSE;

			m_nSize=(signed char)NewSize;
			if(SS_VISABLE & m_Status)
				Update(GetParent(m_hWnd));
			return TRUE;
		}
		int GetSize()
		{
			return m_nSize;
		}
		BOOL SetSharpness(unsigned int NewSharpness=5)
		{
			if(NewSharpness>20)
				return FALSE;

			m_nSharpness=(unsigned char)NewSharpness;
			if(SS_VISABLE & m_Status)
				Update(GetParent(m_hWnd));
			return TRUE;
		}
		unsigned GetSharpness()
		{
			return m_nSharpness;
		}
		BOOL SetDarkness(unsigned int NewDarkness=200)
		{
			if(NewDarkness>255)
				return FALSE;

			m_nDarkness=(unsigned char)NewDarkness;
			if(SS_VISABLE & m_Status)
				Update(GetParent(m_hWnd));
			return TRUE;
		}
		unsigned GetDarkness()
		{
			return m_nDarkness;
		}
		BOOL SetPosition(int NewXOffset=5,int NewYOffset=5)
		{
			if(NewXOffset>20||NewXOffset<-20||
				NewYOffset > 20||NewYOffset<-20)
				return FALSE;

			m_nxOffset=(signed char)NewXOffset;
			m_nyOffset=(signed char)NewYOffset;
			if(SS_VISABLE & m_Status)
				Update(GetParent(m_hWnd));
			return TRUE;
		}
		void GetPosition(int * pnxOffset,int *pnyOffset)
		{
			if(pnxOffset)
				*pnxOffset=m_nxOffset;
			if(pnyOffset)
				*pnyOffset=m_nyOffset;
		}
		BOOL SetColor(COLORREF NewColor=0)
		{
			m_Color=NewColor;
			if(SS_VISABLE & m_Status)
				Update(GetParent(m_hWnd));
			return TRUE;
		}
		COLORREF GetColor()
		{
			return m_Color;
		}
		void SetAdjustShadowBmpCallback(pfnAdjustShadows fNewCallback, void * pCallbackData)
		{
			if(m_fAdjustShadows!=fNewCallback && pCallbackData!=m_pCallbackData)
			{
				m_fAdjustShadows=fNewCallback;
				m_pCallbackData=pCallbackData;
				if(SS_VISABLE & m_Status)
					Update(GetParent(m_hWnd));
			}
		}
		pfnAdjustShadows GetAdjustShadowBmpCallback(void ** ppCallbackData)
		{
			if(ppCallbackData)
				*ppCallbackData=m_pCallbackData;
			return m_fAdjustShadows;
		}
	public:
		CShadowWindow(void)
			: m_hWnd((HWND)INVALID_HANDLE_VALUE)
			,m_OriParentProc(NULL)
			,m_nDarkness(150)
			,m_nSharpness(5)
			,m_nSize(0)
			,m_nxOffset(5)
			,m_nyOffset(5)
			,m_Color(RGB(0,0,0))
			,m_WndSize(0)
			,m_bUpdate(FALSE)
			,m_fAdjustShadows(NULL)
			,m_pCallbackData(NULL)
		{

		}
		virtual ~CShadowWindow()
		{

		}

	};
	_declspec(selectany) pfnUpdateLayeredWindow DuiLib::CShadowWindow::s_UpdateLayeredWindow=NULL;
	_declspec(selectany) pfnDwmIsCompositionEnabled DuiLib::CShadowWindow::s_DwmIsCompositionEnabled=NULL;
	_declspec(selectany) BOOL DuiLib::CShadowWindow::s_bVista=FALSE;
	_declspec(selectany) HINSTANCE DuiLib::CShadowWindow::s_hInstance;

#pragma warning(push)
#pragma warning(disable:4786)

	_declspec(selectany) CShadowWindow*	CShadowWindow::pCShadowWindow_;
	_declspec(selectany) HWND DuiLib::CShadowWindow::hParent_;
#pragma warning(pop) 
}

#endif

