#ifndef _PHYSICS_H_
#define _PHYSICS_H_

class Physics {
	public:
		Physics();
		Physics(double jstr, double maxX, double maxY, double accel);

		double getJumpStrength();
		void setJumpStrength(double jstr);

		double getMaxX();
		double getMaxY();
		void setMaxSpeeds(double x, double y);
		double getAirDrag();

		double getAcceleration();
		void setAcceleration(double accel);

		bool inAir();
		void setAirState(bool state);

		bool isGrappling();
		void setGrappleState(bool state);

		double getDampen();
		double getGrappleStr();

	private:
		double jumpStrength;
		double maxXSPeed;
		double maxYSpeed;
		double airDrag;
		double acceleration;
		double grappleStrength;
		double grappleDampen;
		bool airState;
		bool grappleState;
		
};






#endif
