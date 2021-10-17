class CCARates;

void UpdateRoomCA(CCARates const &rates, float inputFromObjectsInRoom, float &newValue, float &tempValue);
void UpdateRoomCA2( float &newValue, float doorage, float tempValue );

void UpdateDoorCA(float tempValue1, CCARates const &rates1, float relativeDoorSize1,
				  float tempValue2, CCARates const &rates2, float relativeDoorSize2,
				  float opening,
				  float &newValue1, float &newValue2 );

