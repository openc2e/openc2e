#include "RoomCA.h"
#include "CARates.h"
#include <math.h>

void UpdateRoomCA(CCARates const &rates, float inputFromObjectsInRoom, float &newValue, float &tempValue) {
	float lossOrGainRate = newValue>inputFromObjectsInRoom ?
		rates.GetLoss() :
		rates.GetGain();

	tempValue =
		lossOrGainRate*inputFromObjectsInRoom +
		(1.0f-lossOrGainRate)*newValue;
	newValue = 0.0f;
//	newValue = tempValue / 2;
}

void UpdateRoomCA2( float &newValue, float doorage, float tempValue ) {
	newValue += (1 - doorage ) * tempValue;
}

void UpdateDoorCA(float tempValue1, CCARates const &rates1, float relativeDoorSize1,
				  float tempValue2, CCARates const &rates2, float relativeDoorSize2,
				  float opening,
				  float &newValue1, float &newValue2 ) {
	float diffusionRate =
		sqrtf(rates1.GetDiffusion()*rates2.GetDiffusion()) * opening;

	float shareOfTempValue1 = tempValue1*relativeDoorSize1;
	float shareOfTempValue2 = tempValue2*relativeDoorSize2;

	float averageValue = (shareOfTempValue1+shareOfTempValue2)/2.0f;

	newValue1 +=
		shareOfTempValue1*(1.0f-diffusionRate) +
		averageValue*diffusionRate;
	newValue2 +=
		shareOfTempValue2*(1.0f-diffusionRate) +
		averageValue*diffusionRate;
}

