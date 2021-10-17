#ifndef Viewport_H
#define Viewport_H

#include "Wnd.h"
#include "Vat.h"
class MainFrame;


class Viewport : public Wnd
{	
	typedef struct 
	{
		int XOrg;
		int YOrg;
		int XExt;
		int YExt;
	} Extent;
	
	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];

protected:

	int myViewportYExt, myViewportXExt;
	Extent myCanvas;
	int myZoom;
	float myScaleX, myScaleY;
	int myWindowXOrg, myWindowYOrg;
	int myBorder;
	UINT myErase;

	void InitViewport(bool reset);
	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	void On_Command(HWND hwnd, int wID, HWND hwndCtl, UINT wNotifyCode);
	void On_Size(HWND  hwnd, UINT sizeState, int cx, int cy);
	void On_VScroll(HWND hwnd, HWND hCtl, UINT code, int pos);
	void On_HScroll(HWND hwnd, HWND hCtl, UINT code, int pos);

	void On_ZoomIn();
	void On_ZoomOut();

	void CalcViewportExt();
	virtual void SetCanvasSpec();
	void CalcScale();
	void SetScrollBoxSize();
	void SetHScrollPos();
	void SetVScrollPos();

	inline float Zoom(float x){ return (x * myZoom); };
	inline float UnZoom(float x){ return (x / myZoom); };
	inline float ScaleX(float x){ return (x * myScaleX); };
	inline float ScaleY(float y){ return (y * myScaleY); };
	inline float UnScaleX(float x){ return (x / myScaleX); };
	inline float UnScaleY(float y){ return (y / myScaleY); };

	inline int Round(float x){ return (int)((float)(x) + 0.5);};
	inline float BitMapXCoord(float x){ return ( Zoom(ScaleX(x)) - Zoom((float)myWindowXOrg) ) + myBorder;};
    inline float BitMapYCoord(float x){ return ( Zoom(ScaleY(x)) - Zoom((float)myWindowYOrg) ) + myBorder;};

	inline void ClipLine(float &x1, float &y1, float &x2, float &y2)
	{
		// clip numbers to big

		if(x1 > 32767) 
		{
			// clip y1 
			float xLength = x2 - x1;
			float x = 32767;

			if(xLength != 0.0f)
			{			
				float slope = (y2-y1)/xLength;
				y1 += ((x-x1)*slope);
			}
			x1 = x;
		}
		else if(y1 > 32767) 
		{
			// clip x1
			float yLength = y2 - y1;
			float y = 32767;
			
			if(yLength != 0.0f)
			{
				float slope = (x2 - x1)/yLength;
				x1 += ((y-y1)*slope);
			}
			y1 = y;
		}
		else if(x1 < -32768) 
		{
			// clip y1
			float xLength = x2 - x1;
			float x = -32768;

			if(xLength != 0.0f)
			{			
				float slope = (y2-y1)/xLength;
				y1 += ((x-x1)*slope);
			}
			x1 = x;
		}
		else if(y1 < -32768) 
		{
			// clip x1
			float yLength = y2 - y1;
			float y = -32768;
			
			if(yLength != 0.0f)
			{
				float slope = (x2 - x1)/yLength;
				x1 += ((y-y1)*slope);
			}
			y1 = y;
		}






		if(x2 > 32767) 
		{
			// clip y2
			float xLength = x2 - x1;
			float x = 32767;
			
			if(xLength != 0.0f)
			{
				float slope = (y2-y1)/xLength;
				y2 += ((x-x2)*slope);
			}
			x1 = x;
		}
		else if(y2 > 32767) 
		{
			// clip x2
			float yLength = y2 - y1;
			float y = 32767;
			
			if(yLength != 0.0f)
			{
				float slope = (x2 - x1)/yLength;
				x2 += ((y-y2)*slope);
			}
			y2 = y;
		}
		else if(x2 < -32768) 
		{
			// clip y2
			float xLength = x2 - x1;
			float x = -32768;
			
			if(xLength != 0.0f)
			{
				float slope = (y2-y1)/xLength;
				y2 += ((x-x2)*slope);
			}
			x1 = x;
		}
		else if(y2 < -32768) 
		{
			// clip x2
			float yLength = y2 - y1;
			float y = -32768;
			
			if(yLength != 0.0f)
			{
				float slope = (x2 - x1)/yLength;
				x2 += ((y-y2)*slope);
			}
			y2 = y;
		}
	};



	inline void ClipRectangle(float &x1, float &y1, float &x2, float &y2)
	{
		// clip numbers to big

		if(x1 > 32766) 
			x1 = 32766;
		else if(x1 < -32767) 
			x1 = -32767;

		if(y1 > 32766) 
			y1 = 32766;
		else if(y1 < -32767) 
			y1 = -32767;

		if(x2 > 32766) 
			x2 = 32766;
		else if(x2 < -32767) 
			x2 = -32767;
		
		if(y2 > 32766) 
			y2 = 32766;
		else if(y2 < -32767) 
			y2 = -32767;
	}


public:

	Viewport(HINSTANCE hInstance, HWND hWndOwner);
	HWND Create();
};



#endif

