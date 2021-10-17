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

bool operator==( const CAppearance &a1, const CAppearance &a2 )
{
	return 
		a1.m_Sex == a2.m_Sex &&
		a1.m_Age == a2.m_Age &&
		a1.m_Head == a2.m_Head &&
		a1.m_HeadVar == a2.m_HeadVar &&
		a1.m_Body == a2.m_Body &&
		a1.m_BodyVar == a2.m_BodyVar &&
		a1.m_Legs == a2.m_Legs &&
		a1.m_LegsVar == a2.m_LegsVar &&
		a1.m_Arms == a2.m_Arms &&
		a1.m_ArmsVar == a2.m_ArmsVar &&
		a1.m_Tail == a2.m_Tail &&
		a1.m_TailVar == a2.m_TailVar;
		a1.m_Ears == a2.m_Ears &&
		a1.m_EarsVar == a2.m_EarsVar;
		a1.m_Hair == a2.m_Hair &&
		a1.m_HairVar == a2.m_HairVar;
}

