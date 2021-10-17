#ifndef TextViewport_H
#define TextViewport_H
#include "Viewport.h"
#include "Vat.h"

class TextViewport : public Viewport
{	

public:

	TextViewport(HINSTANCE hInstance, HWND hWndOwner, const char *text);
	~TextViewport();
	HWND Create(int x, int y, int nWidth, int nHeight);

protected:

	LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);

private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];
	HFONT myFont;
	const char *myText;
	SIZE myTextSize;
	unsigned int myNoLines;
	unsigned int myLineLength;

	void On_Paint();
	void SetCanvasSpec();
	void SetScrollBoxSize();

};







#endif

