// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andre Schollmeyer and Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Pixel
// -----------------------------------------------------------------------------

#ifndef BUW_PIXEL_HPP
#define BUW_PIXEL_HPP

// header, system
#include <iosfwd>

// header, project
#include <color.hpp>

class Pixel
{
public : // c'tor

  Pixel(unsigned int = 0, unsigned int = 0);
  void print(std::ostream&) const;

  unsigned int x;
  unsigned int y;
  Color color;
};

std::ostream& operator<<(std::ostream&, const Pixel&);

#endif // BUW_PIXEL_HPP
