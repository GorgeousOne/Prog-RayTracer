// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Color
// -----------------------------------------------------------------------------

#ifndef BUW_COLOR_HPP
#define BUW_COLOR_HPP

#include <iostream>
#include <glm/vec3.hpp>

struct Color {

	friend std::ostream &operator<<(std::ostream &os, Color const& c) {
		os << "(" << c.r << "," << c.g << "," << c.b << ")\n";
		return os;
	}

	Color &operator+=(Color const& other) {
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}

	Color &operator-=(Color const& other) {
		r -= other.r;
		g -= other.g;
		b -= other.b;
		return *this;
	}

	Color &operator*=(float f) {
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	Color &operator*=(Color const& coefficient) {
		r *= coefficient.r;
		g *= coefficient.g;
		b *= coefficient.b;
		return *this;
	}

	friend Color operator+(Color const& a, Color const& b) {
		auto tmp(a);
		tmp += b;
		return tmp;
	}

	friend Color operator-(Color const& a, Color const& b) {
		auto tmp(a);
		tmp -= b;
		return tmp;
	}

	friend Color operator*(Color const& a, float f) {
		auto tmp(a);
		tmp *= f;
		return tmp;
	}

	friend Color operator*(Color const& a, Color const& coefficient) {
		auto tmp(a);
		tmp *= coefficient;
		return tmp;
	}
	float r = 0;
	float g = 0;
	float b = 0;
};

#endif //#define BUW_COLOR_HPP
