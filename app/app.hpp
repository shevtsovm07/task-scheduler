#pragma once
#include "geolocation_api.hpp"
#include "weather_api.hpp"
#include "places_api.hpp"
#include "routing_api.hpp"
#include "scheduler.hpp"


namespace ApiConstants{
  const std::string kApiKey = "api_key";
  const std::string kIpApiFilename = "../app/ip_api.json";
  const std::string kWeatherApiFilename = "../app/weather_api.json";
  const std::string kMatrixApiFilename = "../app/matrix_api.json";
  const std::string kPlacesApiFilename = "../app/places_api.json";
}; //namespace ApiConstants

namespace KindsOfPlaces{
  const std::string kMuseums = "музей";
  const std::string kParks = "парк";
  const std::string kRestaurants = "ресторан";
  const std::string kTheaters = "театр";
  const std::string kMalls = "торговый центр";
}; //namespace KindsOfPlaces

namespace WeatherConditions{
  const int kNoPrec = 0;
  const double kStrongWind = 8.0;
  const double kCold = 0.0;

  const int kNoPrecCode = 0;
  const int kRain = 1;
  const int kRainAndSnow = 2;
  const int kSnow = 3;
  const int kHail = 4;
}

std::string LoadApiKey(const std::string& config_path);
template <typename T>
T unwrap(std::expected<T, std::string>&& exp);
void RunApp();