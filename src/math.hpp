#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <math.h>
// text-replacement macro, converts Degree angle to Radians
#define RAD(x) (x * (M_PI / 180.0))
#define DEG(x) ((180.0 / M_PI) * x)

struct v2 {
	double x;
	double y;

	v2(double, double);

	v2(const v2 &);

	/** to interface with SDL's 2d vector type.
	 */
	v2(const SDL_Point &);

	v2();

	/** Gets the angle measured in rads, starting in the +x direction
	 */
	double getAngle();

	/**
	 * @return the unit vector of this.
	 */
	v2 getUnit();

	/** Gets the hyp of the vector
	 */
	double getMag();

	// Below: operator overloading.
	// These functions implment operators for use with the vector data type
	/** vec + vec */
	v2 operator+=(const v2 &);
	v2 operator+(const v2 &) const;

	/** vec * scaler */
	v2 operator*=(const double &);
	// magic so that `vec * scaler` and `scaler * vec` both work.
	friend v2 operator*(const double &, v2);
	friend v2 operator*(v2, const double &);

	/** vec / scaler */
	v2 operator/=(const double &);
	friend v2 operator/(v2, const double &);

	/** vec negation */
	v2 operator-();

	/** vec - vec */
	v2 operator-=(const v2 &);
	v2 operator-(const v2 &);

	/** vec == vec */
	bool operator==(const v2 &);

	bool operator!=(const v2 &);

	/** vec dot vec */
	// dot product
	double dot(const v2 &);

	/** vec cross vec */
	// cross product
	double cross(const v2 &);
};

// We are ROW MAJOR
struct mat2 {
	double data[2][2];
	// Some useful matrixes...
#define ROT_MAT2(X) (cos(X), -sin(X), sin(X), cos(X))
#define ID_MAT2 (1, 0, 0, 1)

	mat2();

	mat2(double a, double b, double c, double d);

	/**
	 * add two matrixes together
	 */
	mat2 operator+=(const mat2 &);
	mat2 operator+(const mat2 &) const;

	/**
	 *mul a matrix by another matrix
	 */
	mat2 operator*=(const mat2 &);
	mat2 operator*(const mat2 &) const;

	/**
	 * mul a vector by a matrix.
	 * this transforms the vector according to the matrix
	 */
	v2 operator*(const v2 &);

	/**
	 * scale a matrix by a scaler
	 */
	mat2 operator*=(const double &);
	mat2 operator*(const double &);
	friend mat2 operator*(const double, mat2);

	/**
	 * the volume scaling factor of the mat
	 * @return value of the det of this matrix
	 */
	double determinant();

	/**
	 * the transformation which nulls it's counterpart
	 * @return a new matrix which is the inverse of this
	 */
	mat2 inverse();

	/**
	 * flips the contents of the matrix across an down left diagonal.
	 * I don't really know what this means...
	 * @return a new matrix which is the transpose of this
	 */
	mat2 transpose();
};
