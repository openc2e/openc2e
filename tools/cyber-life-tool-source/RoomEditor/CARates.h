#ifndef  CARATES_H
#define  CARATES_H

#include <ostream>
#include <istream>

class CCARates {
public:
	CCARates()
		:  m_Gain( 0.0 ), m_Loss( 0.0 ), m_Diffusion( 0.0 ) {}
	CCARates( float gain, float loss, float diffusion )
		: m_Gain( gain ), m_Loss( loss ), m_Diffusion( diffusion ) {}

	inline float GetGain() const { return m_Gain; }
	inline float GetLoss() const { return m_Loss; }
	inline float GetDiffusion() const { return m_Diffusion; }

	void SetGain( float gain ) { m_Gain = gain; }
	void SetLoss( float loss ) { m_Loss = loss; }
	void SetDiffusion( float diffusion ) { m_Diffusion = diffusion; }

// Persistance
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream );

private:
	float m_Gain;
	float m_Loss;
	float m_Diffusion;
};


#endif

