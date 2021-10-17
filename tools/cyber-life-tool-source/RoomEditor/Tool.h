#ifndef TOOL_H_
#define TOOL_H_

class CC2ERoomEditorView;

class CTool {
public:
	virtual ~CTool() {}

	void SetView( CC2ERoomEditorView *view ) { m_View = view; }

	bool Dragging();
	virtual void MouseMove( CPoint const &point, bool shift, bool ctrl ); //default draws drag-box while mouse is captured.
	virtual void MouseDown( bool shift, bool ctrl ); //default captures the mouse.
	virtual void MouseUp( bool shift, bool ctrl); //default releases the mouse.
	virtual void MouseMenu( CPoint const &point ) {}

	virtual void KeyPress( UINT key ) {}

	virtual void StartUsing() {} //called when the tool is selected for use
	virtual void StopUsing() {} //called when the tool is deselected for use

	virtual void SetCursor();
protected:
	void DrawSelRect( CRect const &rect );
	CC2ERoomEditorView *m_View;
	CPoint m_PointDown;
	CPoint m_PointLast;
	DWORD m_TimeDown;
};

class CToolAddMetaroom : public CTool {
	virtual void MouseUp( bool shift, bool ctrl );
	virtual void SetCursor();
};

class CToolZoom : public CTool {
	virtual void MouseUp( bool shift, bool ctrl );
	virtual void SetCursor();
	virtual void MouseMenu( CPoint const &point );
};

class CToolSelectRoom : public CTool {
public:
	virtual void MouseMove( CPoint const &point, bool shift, bool ctrl );
	virtual void MouseUp( bool shift, bool ctrl ); //default releases the mouse.
	virtual void MouseMenu( CPoint const &point );
private:
	int m_MoveMode;
	CRect m_MoveRect;
};

class CToolSelectMetaroom : public CTool {
public:
	virtual void MouseMove( CPoint const &point, bool shift, bool ctrl );
	virtual void MouseUp( bool shift, bool ctrl ); //default releases the mouse.
private:
	bool m_OverSelection;
	CRect m_MoveRect;
};

class CToolAddRoom : public CTool {
public:
	CToolAddRoom() : m_PointCount( 0 ) {}
	virtual void MouseMove( CPoint const &point, bool shift, bool ctrl );
	virtual void MouseDown( bool shift, bool ctrl );
	virtual void MouseUp( bool shift, bool ctrl );

	virtual void KeyPress( UINT key );

	virtual void StartUsing();
	virtual void StopUsing();

	void DrawRoom();
	int FormRoomShape( CPoint *in, CPoint *out, int nPoints, int Tol );
private:
	CPoint m_Points[4];
	int m_PointCount;
};

class CToolCheese : public CTool {
public:
	virtual void MouseUp( bool shift, bool ctrl );
};

#endif

