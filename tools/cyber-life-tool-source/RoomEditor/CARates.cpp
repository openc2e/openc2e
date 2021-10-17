#include "CARates.h"

void CCARates::Write( std::ostream &stream ) const
{
	stream << m_Gain << ' ' << m_Loss << ' ' << m_Diffusion << '\n';
}

void CCARates::Read( std::istream &stream )
{
	stream >> m_Gain >> m_Loss >> m_Diffusion;
}

