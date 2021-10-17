#ifndef DlgWnd_H
#define DlgWnd_H

#include "Wnd.h"


class DlgWnd : public Wnd
{
public:

	virtual int ModalMessageLoop()
	{
		MSG msg;

		while(GetMessage(&msg, NULL, 0, 0))
		{
			
			if((msg.hwnd == myHWnd) || IsChild(myHWnd, msg.hwnd) || IsOwnedByMe( msg.hwnd ) )
			{
				if(!IsDialogMessage(myHWnd, &msg))
				{
					TranslateMessage(&msg);	// translate keyboard input (key up/down) to wm_char
					DispatchMessage(&msg);
				}
			}
		}

		ShowWindow(myHWnd, SW_HIDE);	// may have exited but not been destroyed
		return msg.wParam;
	};

protected:

	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
	{
		return FALSE;
//		return Wnd::WndProc( hwnd, msg, wParam, lParam);
	};

};
#endif //DlgWnd_H

