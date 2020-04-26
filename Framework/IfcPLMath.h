#pragma once

#include <memory>
#include "BlueFramework/Core/Math/Vector.h"
#include "BlueFramework/Core/Math/Matrix.h"
#include "BlueFramework/Core/Math/util.h"

using namespace buw;

std::shared_ptr<Vector4d> operator+(std::shared_ptr<Vector4d> a, std::shared_ptr<Vector4d> b);
std::shared_ptr<Vector2d> operator*(const Matrix<double, 2, 2> m, std::shared_ptr<Vector2d> b);
std::shared_ptr<Vector2d> operator-(std::shared_ptr<Vector2d> a, std::shared_ptr<Vector2d> b);
std::shared_ptr<Vector2d> operator+(std::shared_ptr<Vector2d> a, std::shared_ptr<Vector2d> b);


std::shared_ptr<Vector2d> operator*(double s, std::shared_ptr<Vector2d> v);


void print(std::shared_ptr<Vector2d> v);
void print(std::shared_ptr<Vector3d> v);
void print(std::shared_ptr<Vector4d> v);