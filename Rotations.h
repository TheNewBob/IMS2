#pragma once
class Rotations
{
public:
	Rotations();
	~Rotations();

	static MATRIX3 UnitMatrix();
	static MATRIX3 GetRotationMatrixFromOrientation(VECTOR3 dir, VECTOR3 rot);
	static bool RequiresRotation(VECTOR3 &IN_dir, VECTOR3 &IN_rot);
	static void Rotations::TransformMesh(MESHHANDLE mshTemplate, DEVMESHHANDLE hDmesh, MATRIX3 &rotationmatrix);

	static void BuildRotationMatrix(MATRIX3& result, const VECTOR3& rotationAxis, double angle);
	static void TransformMesh(MESHHANDLE mshTemplate, DEVMESHHANDLE hDmesh, MATRIX3 &firstRot, MATRIX3 &secondRot);
	static double GetAngle(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& signRef);
	static void RotateVector(VECTOR3& v, const VECTOR3& axis, const double angle);
	
	/* Returns a projection of a vector on a plane
	*/
	static VECTOR3 Rotations::GetProjection(const VECTOR3& v, const VECTOR3& planeNormal);

	static MATRIX3 InverseMatrix(MATRIX3 matrix);

};

