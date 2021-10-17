#ifndef APPEARANCE_H_
#define APPEARANCE_H_

class CAppearance
{
public:
	CAppearance() : 
		m_Sex(0), m_Age(0), m_Head(0), m_HeadVar(0), m_Body(0),
		m_BodyVar(0), m_Legs(0), m_LegsVar(0), m_Arms(0),
		m_ArmsVar(0), m_Tail(0), m_TailVar(0),
		m_Ears(0), m_EarsVar(0), m_Hair(0), m_HairVar(0)
		{}
	CAppearance( int Sex, int Age, int Head, int HeadVar, int Body, int BodyVar,
		int Legs, int LegsVar, int Arms, int ArmsVar, int Tail, int TailVar,
		int Ears, int EarsVar, int Hair, int HairVar ) : 
		m_Sex(Sex), m_Age(Age), m_Head(Head), m_HeadVar(HeadVar), m_Body(Body),
		m_BodyVar(BodyVar), m_Legs(Legs), m_LegsVar(LegsVar), m_Arms(Arms),
		m_ArmsVar(ArmsVar), m_Tail(Tail), m_TailVar(TailVar),
		m_Ears(Ears), m_EarsVar(EarsVar), m_Hair(Hair), m_HairVar(HairVar)
		{}
	int	m_Sex;
	int	m_Age;
	int	m_Head;
	int	m_HeadVar;
	int	m_Body;
	int	m_BodyVar;
	int	m_Legs;
	int	m_LegsVar;
	int	m_Arms;
	int	m_ArmsVar;
	int	m_Tail;
	int	m_TailVar;
	int m_Ears;
	int m_EarsVar;
	int m_Hair;
	int m_HairVar;
};

#endif

