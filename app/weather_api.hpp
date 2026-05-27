#pragma once
#include <string>
#include <expected>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>



class WeatherApi {
  std::string api_key_;


public:
  explicit WeatherApi(const std::string& api_key) : api_key_(api_key) {};
  explicit WeatherApi(std::string&& api_key) noexcept : api_key_(std::move(api_key)) {};
  WeatherApi(const WeatherApi&) = delete;
  WeatherApi& operator=(const WeatherApi&) = delete;
  WeatherApi(WeatherApi&& other) noexcept = default; 
  WeatherApi& operator=(WeatherApi&& other) noexcept = default;


  std::expected<nlohmann::json, std::string> GetWeather(const double& latitude, const double& longitude) const;
};