class physicsHandler {
private:
	float finalx, finaly;
	
public:
	bool collidePoints(float, float, float, float, float, float, float, float);
	float getCollisionX() { return finalx; }
	float getCollisionY() { return finaly; }
};

