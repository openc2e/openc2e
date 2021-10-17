#ifndef HANDLE_H
#define HANDLE_H

template<class Handled> class handle
{
public:
	handle() : m_Handled( 0 ) { static int nullCount = 1; ++nullCount; m_Count = &nullCount; }
	handle( Handled *pHandled ) : m_Handled( pHandled ), m_Count( new int(1) ) {}
	handle( handle const &other ) : m_Handled( other.m_Handled ), m_Count( other.m_Count ) {(*m_Count)++;}
	~handle()
	{
		Release();
	}

	handle &operator=( handle const &other )
	{
		if( m_Handled == other.m_Handled ) return *this;
		Release();
		m_Handled = other.m_Handled;
		m_Count = other.m_Count;
		(*m_Count)++;
		return *this;
	}

	Handled *operator->() { return m_Handled; }
	Handled *GetObj() { return m_Handled; }
	Handled *obj() { return m_Handled; }
	Handled &operator*() { return *m_Handled; }

	Handled const *operator->() const { return m_Handled; }
	Handled const *GetObj() const { return m_Handled; }
	Handled const *obj() const { return m_Handled; }
	Handled const &operator*() const { return *m_Handled; }

	bool operator<( handle const &other ) const { return *m_Handled < *other.m_Handled; }
	bool operator==( handle const &other ) const { return m_Handled == other.m_Handled; }


private:
	void Release()
	{
		if( --(*m_Count) == 0 )
		{
			delete m_Handled;
			delete m_Count;
		}
	}
	Handled *m_Handled;
	int *m_Count;
};



#endif

