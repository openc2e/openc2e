#include "fileformats/mngparser.h"

#include <gtest/gtest.h>

TEST(fileformats, MngParser) {
	std::string script = R"(
        // This is the manager
        Variable(managerVar,0.0)


        Effect(Dulcimer) 
         {
            // Produces randomly panned echoes,fading in then fading out
            Stage
                {
                Pan( Random ( -1.0, 1.0) ) Volume(0.40)  Delay(0.2)
                }
            Stage
                {
                Pan( Random ( -1.0, 1.0) ) Volume(0.50)  Delay(0.2)
                }
        }

        Track(MetallicChords)
        {
        FadeIn(4)
        FadeOut(4)
        AleotoricLayer(Chords)
            {
            Variable(temp,0)
            Update
                {
                Interval = Multiply(Threat,2)
                Interval = Subtract(9,Interval)
                temp = Random(0,2)
                Interval = Add ( Interval, temp)
                Volume = Multiply(Threat,0.1)
                Volume = Add(Volume,0.9)
                }
            Volume(1.0)
            Interval(4.0)
            Effect(SlowPad)
            Voice
                {
                Wave(Metal0)
                }
            }
        }

        Track(Machinery)
        {
        FadeIn(5)
        FadeOut(8)

        LoopLayer(Ep)
            {
            Variable(counter,0.0)
            Variable(temp,0.0)
            Update
                {
                counter = Add(counter,0.5)
                temp = SineWave(counter, 30)
                Pan = temp
                temp = Multiply ( temp, 0.4 )
                temp = Add (temp, 0.4)
                Volume = Multiply( Mood, 0.2)
                Volume = Add( Volume, temp ) 
                }
            UpdateRate(0.5) 
            Wave(EpThrob)
            }

        AleotoricLayer(Dream)
            {
            Volume(0.8)
            Effect(RandomPad)
            Voice 
                {
                Wave(Drm0) Interval( Random (2.0, 5.0) ) 
                }
            Voice 
                {
                Wave(Drm2) Interval( Random (2.0, 5.0) ) 
                }
            Voice 
                {
                Wave(Drm3) Interval( Random (2.0, 5.0) ) 
                }
            Voice 
                {
                Wave(Drm5) Interval( Random (2.0, 5.0) ) 
                }
            }
        }


	)";

	// Shouldn't throw
	mngparse(script);
}