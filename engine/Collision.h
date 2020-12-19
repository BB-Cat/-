#pragma once
#include "Vector3D.h"
#include "Colliders.h"

class Collision
{
public:
	static bool DetectCollision(Collider* col1, Vec3 pos1, Collider* col2, Vec3 pos2);

	// NOTE : Currently collision resolution is discrete ONLY for the first AABB pass, 
	//meaning objects can slip through eachother if their colliders entirely pass eachother in one frame.
	//The mass value determines how much both colliders are "pushed" to solve collision.
	//a value of 1 means only the first collider is moved, a value of 0 means only the second collider is moved.
	static bool ResolveCollision(Collider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass = 1.0f);

	// DETECTION FUNCTIONS //
	static bool DetectAABBDiscrete(Vec3 min1, Vec3 max1, Vec3 min2, Vec3 max2);
	static bool DetectAABBContinuous(Collider* col1, Vec3 pos1, Vec3 prevpos, Collider* col2, Vec3 pos2);
	static bool DetectSphereCube(Vec3 s_pos, float radius, Vec3 c_min, Vec3 c_max, Vec3* closest = nullptr);
	static bool DetectSphereSphere(Vec3 pos1, float r, Vec3 pos2, float r2);
	static bool DetectCapsuleCapsule(CapsuleCollider* col1, Vec3 pos1, CapsuleCollider* col2, Vec3 pos2, 
		Vec3* closest1 = nullptr, Vec3* closest2 = nullptr);

	// RESOLUTION FUNCTIONS //
	static bool ResolveAABBContinuous(CubeCollider* col1, Vec3& pos1, Vec3 prevpos, CubeCollider* col2, Vec3& pos2, float mass);
	/* The Sphere Cube Continuous Resolution does not currently work (I'm still determining a better way) */
	static bool ResolveSphereCubeContinuous(Vec3& s_pos, float radius, Vec3 prevpos, CubeCollider* col2, Vec3& c_pos, float mass, Vec3* closest = nullptr);
	static bool ResolveSphereSphereContinuous(Vec3& pos1, float r, Vec3 prevpos, Vec3& pos2, float r2, float mass);
	//This function finds the face of least intersection and adjusts the shapes from that direction
	static bool ResolveSphereCubeDiscrete(Vec3& s_pos, float radius, CubeCollider* col2, Vec3& c_pos, float mass, Vec3* closest = nullptr);
	static bool ResolveCapsuleCapsuleDiscrete(CapsuleCollider* col1, Vec3& pos1, CapsuleCollider* col2, Vec3& pos2, float mass,
		Vec3* closest1 = nullptr, Vec3* closest2 = nullptr);


	// UTILITY FUNCTIONS //
	static float pointPointDistance(Vec3 p1, Vec3 p2);
	static float lineLineDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, float& dist_from_l1, float& dist_from_l2);
	static float segmentSegmentDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, Vec3* closest1 = nullptr, Vec3* closest2 = nullptr);

	static float pointLineDistance(Vec3 point, Vec3 line_start, Vec3 line_end, float& dist_from_l1);
	static float pointSegmentDistance(Vec3 point, Vec3 line_start, Vec3 line_end, float& dist_from_l1);

	//calculate whether the angle between two vectors is acute
	static bool isAcute(const Vec3& v1, const Vec3& v2);


private:
	// DETECTION FUNCTIONS //
	/* These functions simply check if there is a collision and return a bool value */
	static bool interpretCubeDetection(Vec3 min, Vec3 max, Collider* col2, Vec3 pos2);
	static bool interpretSphereDetection(SphereCollider* col1, Vec3 pos1, Collider* col2, Vec3 pos2);
	static bool interpretCapsuleDetection(CapsuleCollider* col1, Vec3 pos1, Collider* col2, Vec3 pos2);

	// RESOLUTION FUNCTIONS //
	/* These functions detect if there is a collision and then adjust the position values of each collider
	   so that they do not overlap. */
	static bool interpretCubeResolution(CubeCollider* col1, Vec3 min1, Vec3 max1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass);
	static bool interpretSphereResolution(SphereCollider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass);
	static bool interpretCapsuleResolution(CapsuleCollider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass);


};