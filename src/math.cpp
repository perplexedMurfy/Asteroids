#include "math.hpp"

#include <cstring>

v2::v2(double x, double y) {
	this->x = x;
	this->y = y;
}

v2::v2(const v2 &ref) {
	this->x = ref.x;
	this->y = ref.y;
}

/** to interface with SDL's 2d vector type.
 */
v2::v2(const SDL_Point &ref) {
	this->x = ref.x;
	this->y = ref.y;
}

v2::v2() {
	this->x = 0;
	this->y = 0;
}

/** Gets the angle measured in rads, starting in the +x direction
 */
double v2::getAngle() {
	v2 unit = this->getUnit();

	// special cases
	if (*this == v2(0, 0)) {
		return NAN;
	} else if (unit == v2(1, 0)) {
		return RAD(0);
	} else if (unit == v2(0, 1)) {
		return RAD(90);
	} else if (unit == v2(-1, 0)) {
		return RAD(180);
	} else if (unit == v2(0, -1)) {
		return RAD(270);
	}

	bool xIsPos = unit.x > 0;
	bool yIsPos = unit.y > 0;
	double offset = 0;

	if (xIsPos && yIsPos) {
		offset = RAD(0);
	} else if (xIsPos && !yIsPos) {
		offset = RAD(360);
	} else if (!xIsPos && yIsPos) {
		offset = RAD(180);
	} else if (!xIsPos && !yIsPos) {
		offset = RAD(180);
	}

	double angle = atan(y / x);
	return angle + offset;
}

/**
 * @return the unit vector of this.
 */
v2 v2::getUnit() {
	double mag = getMag();
	if (mag == 0) { return v2(0, 0); }
	return {x / mag, y / mag};
}

/** Gets the hyp of the vector
 */
double v2::getMag() { return sqrt(pow(x, 2) + pow(y, 2)); }

// Below: operator overloading.
// These functions implment operators for use with the vector data type
/** vec + vec */
v2 v2::operator+=(const v2 &rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}
v2 v2::operator+(const v2 &rhs) const {
	v2 lhs(*this);
	return lhs += rhs;
}

/** vec * scaler */
v2 v2::operator*=(const double &rhs) {
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}
// magic so that `vec * scaler` and `scaler * vec` both work.
v2 operator*(const double &lhs, v2 rhs) { return rhs *= lhs; }
v2 operator*(v2 lhs, const double &rhs) { return lhs *= rhs; }

/** vec / scaler */
v2 v2::operator/=(const double &rhs) {
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}
v2 operator/(v2 rhs, const double &lhs) { return rhs /= lhs; }

/** vec negation */
v2 v2::operator-() {
	this->x = -this->x;
	this->y = -this->y;
	return *this;
}

/** vec - vec */
v2 v2::operator-=(const v2 &rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}
v2 v2::operator-(const v2 &rhs) {
	v2 lhs(*this);
	return lhs -= rhs;
}

/** vec == vec */
bool v2::operator==(const v2 &rhs) { return this->x == rhs.x && this->y == rhs.y; }

bool v2::operator!=(const v2 &rhs) { return !(*this == rhs); }

/** vec dot vec */
// dot product
double v2::dot(const v2 &rhs) { return (rhs.x * this->x) + (rhs.y * this->y); }

/** vec cross vec */
// cross product
double v2::cross(const v2 &rhs) { return this->x * rhs.y - this->y * rhs.x; }

// We are ROW MAJOR
#define MAT_A data[0][0]
#define MAT_B data[0][1]
#define MAT_C data[1][0]
#define MAT_D data[1][1]
// These are so...
// [ [ MAT_A, MAT_B ],
//   [ MAT_C, MAT_D ] ]
// It's just nice.

// Some useful matrixes...
#define ROT_MAT2(X) (cos(X), -sin(X), sin(X), cos(X))
#define ID_MAT2 (1, 0, 0, 1)

mat2::mat2() { memset(data, 0, sizeof(data)); }

mat2::mat2(double a, double b, double c, double d) {
	MAT_A = a;
	MAT_B = b;
	MAT_C = c;
	MAT_D = d;
}

// TODO: this function is broken.
// throws garbage data into newly made matrix.
//	mat2 (const mat2& rhs) {
//		memcpy (this->data, rhs.data, 4);
//	}

/**
 * add two matrixes together
 */
mat2 mat2::operator+=(const mat2 &rhs) {
	this->MAT_A += rhs.MAT_A;
	this->MAT_B += rhs.MAT_B;
	this->MAT_C += rhs.MAT_C;
	this->MAT_D += rhs.MAT_D;
	return *this;
}
mat2 mat2::operator+(const mat2 &rhs) const {
	mat2 lhs(*this);
	return lhs += rhs;
}

// TODO: I think there is a better way of defining this pair of functions
/**
 *mul a matrix by another matrix
 */
mat2 mat2::operator*=(const mat2 &rhs) {
	mat2 lhs(*this);
	this->MAT_A = (lhs.MAT_A * rhs.MAT_A + lhs.MAT_B * rhs.MAT_C);
	this->MAT_B = (lhs.MAT_A * rhs.MAT_B + lhs.MAT_B * rhs.MAT_D);
	this->MAT_C = (lhs.MAT_C * rhs.MAT_A + lhs.MAT_D * rhs.MAT_C);
	this->MAT_D = (lhs.MAT_C * rhs.MAT_B + lhs.MAT_D * rhs.MAT_D);
	return *this;
}
mat2 mat2::operator*(const mat2 &rhs) const {
	mat2 lhs(*this);
	return lhs *= rhs;
}

/**
 * mul a vector by a matrix.
 * this transforms the vector according to the matrix
 */
v2 mat2::operator*(const v2 &rhs) {
	return v2(this->MAT_A * rhs.x + this->MAT_B * rhs.y, this->MAT_C * rhs.x + this->MAT_D * rhs.y);
}

/**
 * scale a matrix by a scaler
 */
mat2 mat2::operator*=(const double &rhs) {
	this->MAT_A *= rhs;
	this->MAT_B *= rhs;
	this->MAT_C *= rhs;
	this->MAT_D *= rhs;
	return *this;
}
mat2 mat2::operator*(const double &rhs) {
	mat2 lhs(*this);
	return lhs *= rhs;
}
mat2 operator*(const double rhs, mat2 lhs) { return lhs *= rhs; }

/**
 * the volume scaling factor of the mat
 * @return value of the det of this matrix
 */
double mat2::determinant() { return (MAT_A * MAT_D) - (MAT_B * MAT_C); }

/**
 * the transformation which nulls it's counterpart
 * @return a new matrix which is the inverse of this
 */
mat2 mat2::inverse() {
	double det = this->determinant();
	if (det == 0) { return mat2 ID_MAT2; }
	mat2 ret(MAT_D, -MAT_B, -MAT_C, MAT_A);
	return (1 / det) * ret;
}

/**
 * flips the contents of the matrix across an down left diagonal.
 * I don't really know what this means...
 * @return a new matrix which is the transpose of this
 */
mat2 mat2::transpose() { return mat2(MAT_A, MAT_C, MAT_B, MAT_D); }

#undef MAT_A
#undef MAT_B
#undef MAT_C
#undef MAT_D
