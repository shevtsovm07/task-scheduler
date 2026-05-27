#pragma once
#include <string>

struct Place {
  double lat_;
  double lon_;
  std::string name_;

  Place(double lat, double lon, const std::string& name) : lat_(lat), lon_(lon), name_(name) {};
};
