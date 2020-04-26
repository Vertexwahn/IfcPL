#pragma once

#include <memory>
#include <vector>

template<typename T>
class IfcPLArray
{
public:
	IfcPLArray(size_t count) {
		resize(count);
	}

	void resize(size_t count) {
		elements.resize(count);
	}

	int count() const {
		return static_cast<int>(elements.size());
	}

	std::shared_ptr<T>& operator[](size_t index) {
		return elements[index];
	}

	const std::shared_ptr<T>& operator[](size_t index) const {
		return elements[index];
	}

private:
	std::vector<std::shared_ptr<T>> elements;
};

// explicit (full) template specialization for int
template<>
class IfcPLArray<int>
{
public:
	IfcPLArray(size_t count) {
		resize(count);
	}

	void resize(size_t count) {
		elements.resize(count);
	}

	int count() const {
		return static_cast<int>(elements.size());
	}

	int& operator[](size_t index) {
		return elements[index];
	}

	const int& operator[](size_t index) const {
		return elements[index];
	}

private:
	std::vector<int> elements;
};