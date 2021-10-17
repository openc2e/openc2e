#include "Wnd.h"
#include <string>

class Edit : public Wnd
{
public:
	Edit(HINSTANCE hInstance, HWND hWndOwner, const char *text) :
		myText(text),
		myFont(NULL)
		{
			myWCS.hInstance = hInstance;
			myWCS.hWndParent = hWndOwner;
		}
	~Edit();
	virtual HWND Create(int x, int y, int nWidth, int nHeight);

private:
	std::string myText;
	HFONT myFont;
};

