#include "IfcPLMath.h"

#include <cassert>
#include <iostream>

std::shared_ptr<Vector2d> operator*(const Matrix<double, 2, 2> m, std::shared_ptr<Vector2d> b) {
	return std::make_shared<Vector2d>(m * *b);
}

std::shared_ptr<Vector2d> operator+(std::shared_ptr<Vector2d> a, std::shared_ptr<Vector2d> b) {
	return std::make_shared<Vector2d>(a->x() + b->x(), a->y() + b->y());
}

std::shared_ptr<Vector4d> operator+(std::shared_ptr<Vector4d> a, std::shared_ptr<Vector4d> b) {
	return std::make_shared<Vector4d>(*a + *b);
}

std::shared_ptr<Vector2d> operator-(std::shared_ptr<Vector2d> a, std::shared_ptr<Vector2d> b) {
	return std::make_shared<Vector2d>(a->x() - b->x(), a->y() - b->y());
}

std::shared_ptr<Vector2d> operator*(double s, std::shared_ptr<Vector2d> v) {
	return std::make_shared<Vector2d>(s * v->x(), s * v->y());
}

double factorial(const int n) {
	const int maxValue = 20;

	//assert(n >= 0, "Invalid value.");
	//assert(n <= maxValue, "Invalid value.");

	static double lookupTable[maxValue + 1] = {
	  1.0,                                    // 0!
	  1.0,                                    // 1!
	  2.0,                                    // 2!
	  6.0,                                    // 3!
	  24.0,                                   // 4!
	  24.0 * 5,                               // 5!
	  1 * 2 * 3 * 4 * 5 * 6,                  // 6!
	  1 * 2 * 3 * 4 * 5 * 6 * 7,              // 7!
	  1 * 2 * 3 * 4 * 5 * 6 * 7 * 8,          // 8!
	  1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9,      // 9!
	  1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10, // 10!
	  39916800,                               // 11!
	  479001600,                              // 12!
	  6227020800,                             // 13!
	  87178291200,                            // 14!
	  1307674368000,                          // 15!
	  20922789888000,                         // 16!
	  355687428096000,                        // 17!
	  6402373705728000,                       // 18!
	  121645100408832000,                     // 19!
	  2432902008176640000                     // 20!
	};

	return lookupTable[n];
}


void print(std::shared_ptr<Vector2d> v) {
	std::cout << *v << std::endl;
}

void print(std::shared_ptr<Vector3d> v) {
	std::cout << *v << std::endl;
}
void print(std::shared_ptr<Vector4d> v) {
	std::cout << *v << std::endl;
}