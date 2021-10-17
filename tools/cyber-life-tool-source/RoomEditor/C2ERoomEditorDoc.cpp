// C2E Room EditorDoc.cpp : implementation of the CC2ERoomEditorDoc class
//

#include "stdafx.h"
#include "C2ERoomEditor.h"

#include "C2ERoomEditorDoc.h"
#include "C2ERoomEditorView.h"
#include "MainFrm.h"
#include "REException.h"
#include "PropertyTypesDlg.h"
#include "WorldSizeDlg.h"
#include "SwitchMetaroomDlg.h"
#include "Game.h"
#include "CAPropertiesDlg.h"
#include "ProgDlg.h"

#include <fstream>
#include <string>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorDoc

IMPLEMENT_DYNCREATE(CC2ERoomEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CC2ERoomEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CC2ERoomEditorDoc)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_FILE_PROPERTY_TYPES, OnFilePropertyTypes)
	ON_COMMAND(ID_FILE_SAVE_TO_GAME, OnFileSaveToGame)
	ON_COMMAND(ID_FILE_SAVE_TO_GAME_ADDON, OnFileSaveToGameAddon)
	ON_COMMAND(ID_FILE_WORLD_SIZE, OnFileWorldSize)
	ON_COMMAND(ID_TOOLS_SET_METAROOM_IN_GAME, OnToolsSetMetaroomInGame)
	ON_COMMAND(ID_FILE_CA_RATES, OnFileCARates)
	ON_COMMAND(ID_TOOLS_STEPCA, OnToolsStepca)
	ON_COMMAND(ID_FILE_SAVETOBOOTSTRAPMAPCOS, OnFileSaveToBootstrapWorld)
	ON_COMMAND(ID_FILE_SAVETOBOOTSTRAPADDONCOS, OnFileSaveToBootstrapAddon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorDoc construction/destruction

CC2ERoomEditorDoc::CC2ERoomEditorDoc() : m_PropertyTypes( c_NumProperties + 6 ),
	m_CARates( NUM_ROOM_TYPES, std::vector<CCARates>( NUM_CA_PROPERTIES ) )
{
	m_DoorPropertyTypes.push_back( CPropertyType( "Opening", 0, 128, false, "" ) );
	m_PropertyTypes[CRoomShape::Ord::Left] = CPropertyType( "Left", 0, 1000000, false, "" );
	m_PropertyTypes[CRoomShape::Ord::Right] = CPropertyType( "Right", 0, 1000000, false, "" );
	m_PropertyTypes[CRoomShape::Ord::LeftTop] = CPropertyType( "Top Left", 0, 1000000, false, "" );
	m_PropertyTypes[CRoomShape::Ord::RightTop] = CPropertyType( "Top Right", 0, 1000000, false, "" );
	m_PropertyTypes[CRoomShape::Ord::LeftBottom] = CPropertyType( "Bottom Left", 0, 1000000, false, "" );
	m_PropertyTypes[CRoomShape::Ord::RightBottom] = CPropertyType( "Bottom Right", 0, 1000000, false, "" );
}

CC2ERoomEditorDoc::~CC2ERoomEditorDoc()
{
}

BOOL CC2ERoomEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorDoc serialization

void CC2ERoomEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorDoc diagnostics

#ifdef _DEBUG
void CC2ERoomEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CC2ERoomEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

CWorld *CC2ERoomEditorDoc::GetWorld()
{
	return &m_World;
}

bool CC2ERoomEditorDoc::Execute( HAction action )
{
	bool success = false;

	CC2ERoomEditorApp *app = (CC2ERoomEditorApp *)AfxGetApp();
	int validation = app->GetValidationOption();

	try
	{
		if( action->Do( this ) )
		{
			try
			{
				if( validation != CC2ERoomEditorApp::Validation::None )
					m_World.Validate();
			}
			catch( ... )
			{
				if( validation == CC2ERoomEditorApp::Validation::AutoUndo )
					action->Undo( this );
				throw;
			}
			m_Undo.push( action );
			while( !m_Redo.empty() ) m_Redo.pop();
			SetModifiedFlag();
			success = true;
		}
	}
	catch( CREException const &exept)
	{
		CString actName;
		actName.LoadString( action->Describe() );
		actName = "Error Executing: " + actName;
		MessageBox( NULL, exept.what(), actName, MB_OK | MB_ICONEXCLAMATION );
	}
	catch( ... )
	{
		CString actName;
		actName.LoadString( action->Describe() );
		actName = "Error Executing: " + actName;
		MessageBox( NULL, "Unknown error executing: " + actName, actName, MB_OK | MB_ICONEXCLAMATION );
	}
	UpdateAllViews( 0 );
	return success;
}

int CC2ERoomEditorDoc::AddMetaroom( CString const &filename, CRect const &rect )
{
	return m_World.AddMetaroom( filename, rect );
}

bool CC2ERoomEditorDoc::AddMetaroom( int index, HMetaroom metaRoom )
{
	return m_World.AddMetaroom( index, metaRoom );
}

bool CC2ERoomEditorDoc::RemoveMetaroom( int handle )
{
	return m_World.RemoveMetaroom( handle );
}

int CC2ERoomEditorDoc::AddRoom( CPoint *points )
{
	return m_World.AddRoom( points );
}

bool CC2ERoomEditorDoc::AddRoom( int index, const HRoom &room )
{
	return m_World.AddRoom( index, room );
}

bool CC2ERoomEditorDoc::RemoveRoom( int handle )
{
	return m_World.RemoveRoom( handle );
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorDoc commands

void CC2ERoomEditorDoc::OnEditRedo() 
{
	m_Redo.top()->Do( this );
	m_Undo.push( m_Redo.top() );
	m_Redo.pop();
	SetModifiedFlag();
	UpdateAllViews( 0 );
}

void CC2ERoomEditorDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !m_Redo.empty() );
	CString Caption;
	Caption.LoadString( IDS_REDO );
	if( !m_Redo.empty() )
	{
		CString actionName;
		actionName.LoadString( m_Redo.top()->Describe() );
		Caption += " " + actionName;
	}
	Caption += "\tCtrl+Y";
	pCmdUI->SetText( Caption );
}

void CC2ERoomEditorDoc::OnEditUndo() 
{
	m_Undo.top()->Undo( this );
	m_Redo.push( m_Undo.top() );
	m_Undo.pop();
	SetModifiedFlag();
	UpdateAllViews( 0 );
}

void CC2ERoomEditorDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !m_Undo.empty() );
	CString Caption;
	Caption.LoadString( IDS_UNDO );
	if( !m_Undo.empty() )
	{
		CString actionName;
		actionName.LoadString( m_Undo.top()->Describe() );
		Caption += " " + actionName;
	}
	Caption += "\tCtrl+Z";
	pCmdUI->SetText( Caption );
}

void CC2ERoomEditorDoc::OnFilePropertyTypes() 
{
	CPropertyTypesDlg dlg( m_PropertyTypes );

	if( dlg.DoModal() == IDOK )
		m_PropertyTypes = dlg.GetPropertyTypes();
}

BOOL CC2ERoomEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	std::ifstream stream( lpszPathName );
	Read( stream );
	
	return TRUE;

}

BOOL CC2ERoomEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{

	std::ofstream stream( lpszPathName );
	Write( stream );

	SetModifiedFlag( FALSE );
	return TRUE;
	//	return CDocument::OnSaveDocument(lpszPathName);
}

void CC2ERoomEditorDoc::Write( std::ostream &stream ) const
{
	int i, j;
	stream << "1 4\n"; //File version Major and Minor (NOT necc. the same as
						//Program version no.
	stream << m_PropertyTypes.size() - 6 << '\n';
	for( i = 6; i < m_PropertyTypes.size(); ++i )
	{
		stream << (const char *)m_PropertyTypes[i].m_Name << '\n' <<
			m_PropertyTypes[i].m_Min << ' ' << m_PropertyTypes[i].m_Max <<
			' ' << m_PropertyTypes[i].m_Enumerated << '\n' <<
			(const char *)m_PropertyTypes[i].m_EnumNames << '\n';
	}

	for( i = 0; i < NUM_ROOM_TYPES; ++i )
		for( j = 0; j < NUM_CA_PROPERTIES; ++j )
			m_CARates[i][j].Write( stream );

	m_World.Write( stream );
}

void CC2ERoomEditorDoc::Read( std::istream &stream )
{
	int i, j;
	int versionMajor = 0, versionMinor = 0, nTypes;

	stream >> nTypes;
	if( nTypes != 32 ) 
	{
		versionMajor = nTypes;
		stream >> versionMinor;
		stream >> nTypes;
	}
	int version = versionMajor * 1000 + versionMinor;

	stream.ignore( 10000, '\n' );
	m_PropertyTypes.resize( nTypes + 6 );
	for( i = 6; i < nTypes + 6; ++i )
	{
		std::string temp;
		std::getline( stream, temp );
		m_PropertyTypes[i].m_Name = temp.c_str();
		stream >> m_PropertyTypes[i].m_Min >> m_PropertyTypes[i].m_Max
			>> m_PropertyTypes[i].m_Enumerated;
		stream.ignore( 10000, '\n' );
		std::getline( stream, temp );
		m_PropertyTypes[i].m_EnumNames = temp.c_str();
	}

	if( versionMajor )
	{
		int numCAProperties = 16;
		if( versionMinor >= 3 ) numCAProperties = NUM_CA_PROPERTIES;
		for( i = 0; i < NUM_ROOM_TYPES; ++i )
			for( j = 0; j < numCAProperties; ++j )
				m_CARates[i][j].Read( stream );
	}

	m_World.Read( stream, version );
}

void CC2ERoomEditorDoc::OnFileSaveToGame() 
{
	try
	{
		CGame game;
		SaveToGame( game, false );
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		MessageBox( NULL, "Unknown error.", "Error Injecting to Game", MB_OK | MB_ICONEXCLAMATION );
	}
}

void CC2ERoomEditorDoc::OnFileSaveToGameAddon() 
{
	try
	{
		CGame game;
		SaveToGame( game, true );
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		MessageBox( NULL, "Unknown error.", "Error Injecting to Game", MB_OK | MB_ICONEXCLAMATION );
	}
}

void CC2ERoomEditorDoc::SaveToGame(CGame &game, bool addon) 
{
	CProgressDlg dlg( IDS_SAVING_TO_GAME );

	class GamePauser
	{
	public:
		GamePauser(CGame& game, bool ldo) : myGame(game), myDo(ldo)
		{ 
			if (myDo)
				myGame.Execute("dbg: paws\n"); 
		};

		~GamePauser()
		{ 
			if (myDo)
				myGame.Execute("dbg: play\n");
		};

	private:
		CGame& myGame;
		bool myDo;
	} gamePauser(game, game.ExecutingLive());

	game.Execute("* Autogenerated by the Creatures Map Editor");
	game.Execute("* For more info see http://cdn.creatures.net");
	game.Execute("");
	if (addon)
		game.Execute("* Addon file: deletes existing metarooms at the same place, same background name");
	else
		game.Execute("* World file: existing map deleted, CA rates written");

	game.Execute("* %d metarooms, %d rooms, %d doors",
			m_World.GetMetaroomCount(),
			m_World.GetRoomCount(),
			m_World.GetDoorSet().size());
	game.Execute("");

	if (!addon)
	{
		// for live injection, remove all old metarooms before call to mapd
		if (game.ExecutingLive())
		{
			CString metarooms = game.Execute("outs emid\n");

			std::istringstream instr((const char*)metarooms);
			std::string meta;
			instr >> meta;
			for (; !meta.empty(); instr >> meta)
			{
				CString result = game.Execute(
							 "setv va00 %s\n"
							 "%s\n",
					meta.c_str(), (const char*)CMetaroom::CAOSCheckDeleteMetaRoomScript());
				if (result == "agent")
					throw CREException( "The existing metaroom can't be deleted because there is an agent in it.  The camera has moved to the agent and it has been marked.  Please remove the agent and try again." );
				if (result == "background")
					throw CREException( "There is a metaroom in the same place with a different background, so it is probably from a different addon.  Please move your metaroom, or start a new world and try again." );
			}
		}

		game.Execute( "mapk" );

		game.Execute( "brmi %d %d", m_World.GetMetaroomIndexBase(),
			m_World.GetRoomIndexBase() );
	}

	CDoorSet doorErrors;
	m_World.SaveToGame( game, doorErrors, dlg, addon );

	if (!addon)
	{
		dlg.SetStatus( "Writing CA Rates" );
		dlg.SetRange( 0, 15 );
		for( int i = 0; i < NUM_ROOM_TYPES; ++i )
		{
			CString execute;
			dlg.SetPos( i );
			for( int j = 0; j < NUM_CA_PROPERTIES; ++j )
			{
				CString rate;
				rate.Format( "rate %d %d %f %f %f\n", i, j,
					m_CARates[i][j].GetGain(), 
					m_CARates[i][j].GetLoss(), 
					m_CARates[i][j].GetDiffusion() );
				execute += rate;
			}
			game.Execute( execute );
		}
	}

	if( !doorErrors.empty() )
	{
		CDoorSet::const_iterator itorDoors;
		CString allDoors( "Following doors not recognized in the game:" );
		for( itorDoors = doorErrors.begin(); itorDoors != doorErrors.end(); ++itorDoors )
		{
			CString thisDoor;
			thisDoor.Format( "\nroom%d room%d", (*itorDoors)->GetRoom1(),
				(*itorDoors)->GetRoom2() );
			allDoors += thisDoor;
		}
		::AfxMessageBox( allDoors, MB_OK | MB_ICONSTOP );
		POSITION pos = GetFirstViewPosition();
		while( pos )
		{
			CC2ERoomEditorView *view = 
				(CC2ERoomEditorView *)GetNextView(pos);
			view->ClearSelection();
			for( itorDoors = doorErrors.begin(); itorDoors != doorErrors.end(); ++itorDoors )
				view->ToggleSelectedDoor( *itorDoors );
		}
	}

	// Don't think this makes sense to be in map cos file:
	// game.Execute( "meta 0 -1 -1 0" );
}

void CC2ERoomEditorDoc::OnFileWorldSize() 
{
	CWorldSizeDlg dlg;
	dlg.m_Width = m_World.GetSize().cx;
	dlg.m_Height = m_World.GetSize().cy;
	dlg.m_MetaroomBase = m_World.GetMetaroomIndexBase();
	dlg.m_RoomBase = m_World.GetRoomIndexBase();
	if( dlg.DoModal() == IDOK )
	{
		Execute( HAction( new CActionSizeWorld( CSize( dlg.m_Width, dlg.m_Height ), dlg.m_MetaroomBase, dlg.m_RoomBase ) ) );
	}
	UpdateAllViews( 0 );
}

void CC2ERoomEditorDoc::OnToolsSetMetaroomInGame() 
{
	CSwitchMetaroomDlg dlg( &m_World );
	dlg.DoModal();
}

void CC2ERoomEditorDoc::OnFileCARates() 
{
	CCAPropertiesDlg dlg;
	std::vector< CString > roomTypes;
	std::vector< CString > CANames;
	int i;

	for( i = 0; i < NUM_ROOM_TYPES; ++i )
		 roomTypes.push_back( m_PropertyTypes[6].GetEnumName( i ) );
	dlg.SetRoomTypes( roomTypes );

	for( i = 0; i < NUM_CA_PROPERTIES; ++i )
	{
		CString name = m_PropertyTypes[i + 7].m_Name;
		if( name == "" ) name.Format( "%d", i );
		CANames.push_back( name );
	}
	dlg.SetCANames( CANames );

	dlg.SetCARates( m_CARates );
	if( dlg.DoModal() == IDOK )
	{
		m_CARates = dlg.GetCARates();
	}
}

void CC2ERoomEditorDoc::OnToolsStepca() 
{
	m_World.StepCA( m_CARates, m_Cheeses );
	UpdateAllViews( NULL );
}

/*
void CC2ERoomEditorDoc::OnToolsUpdateCaFromGame() 
{
	try
	{
		CGame game;
		int nRooms = m_World.GetRoomCount();
		CString res = game.Execute( "SETV VA00 0\n"
			"REPS %d\n"
			"OUTV PROP VA00 %d\n"
			"OUTS \"\\n\"\n"
			"ADDV VA00 1\n"
			"REPE", nRooms, 0 );
		std::istrstream stream( res );
		m_World.ReadCAValues( stream, 0 );
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		AfxMessageBox( "Error updating CA", MB_OK | MB_ICONEXCLAMATION );
	}
	
}
*/

void CC2ERoomEditorDoc::DoLoadFromGame() 
{
	CProgressDlg dlg( IDS_LOADING_FROM_GAME );
	try
	{
		CGame game;

		CDoorSet doorErrors;
		m_World.LoadFromGame( game, doorErrors, dlg );

		dlg.SetStatus( "Reading CA Rates" );
		dlg.SetRange( 0, 15 );
		int i, j;
		for( i = 0; i < NUM_ROOM_TYPES; ++i )
		{
			CString execute;
			dlg.SetPos( i );
			for( j = 0; j < NUM_CA_PROPERTIES; ++j )
			{
				CString rate;
				rate.Format( "outs rate %d %d\n", i, j );
				execute += rate;
			}
			std::stringstream stream( (const char *)game.Execute( execute ) );
			for( j = 0; j < NUM_CA_PROPERTIES; ++j )
			{
				float gain, loss, diff;
				stream >> gain >> loss >> diff;
				m_CARates[i][j] = CCARates( gain, loss, diff );
			}
		}

		if( !doorErrors.empty() )
		{
			CDoorSet::const_iterator itorDoors;
			CString allDoors( "Following doors not recognized in the game:" );
			for( itorDoors = doorErrors.begin(); itorDoors != doorErrors.end(); ++itorDoors )
			{
				CString thisDoor;
				thisDoor.Format( "\nroom%d room%d", (*itorDoors)->GetRoom1(),
					(*itorDoors)->GetRoom2() );
				allDoors += thisDoor;
			}
			::AfxMessageBox( allDoors, MB_OK | MB_ICONSTOP );
			POSITION pos = GetFirstViewPosition();
			while( pos )
			{
				CC2ERoomEditorView *view = 
					(CC2ERoomEditorView *)GetNextView(pos);
				view->ClearSelection();
				for( itorDoors = doorErrors.begin(); itorDoors != doorErrors.end(); ++itorDoors )
					view->ToggleSelectedDoor( *itorDoors );
			}
		}
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		MessageBox( NULL, "Unknown error.", "Error Loading from Game", MB_OK | MB_ICONEXCLAMATION );
	}
	UpdateAllViews( 0 );
	POSITION p;
	p = GetFirstViewPosition();
	((CC2ERoomEditorView*)GetNextView(p))->OnViewZoomWorld();
}

void CC2ERoomEditorDoc::OnFileSaveToBootstrapAddon() 
{
	try
	{
		CString mapName = GetBootstrapDirectory() + "!addon.cos";
		DeleteFile( mapName );
		//Special game doesn't actually talk to game.
		CGame game( mapName, false );
		SaveToGame( game, true );
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		MessageBox( NULL, "Unknown error.", "Error Injecting into Game", MB_OK | MB_ICONEXCLAMATION );
	}
}

void CC2ERoomEditorDoc::OnFileSaveToBootstrapWorld() 
{
	try
	{
		CString mapName = GetBootstrapDirectory() + "!world.cos";
		DeleteFile( mapName );
		//Special game doesn't actually talk to game.
		CGame game( mapName, false );
		SaveToGame( game, false );
	}
	catch( CREException &except )
	{
		AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
	}
	catch( ... )
	{
		MessageBox( NULL, "Unknown error.", "Error Injecting into Game", MB_OK | MB_ICONEXCLAMATION );
	}
}

void CC2ERoomEditorDoc::SelectRoomWronguns(int perm, int height, std::set< int > &selection)
{
	m_World.SelectRoomWronguns( perm, height, selection );
}

