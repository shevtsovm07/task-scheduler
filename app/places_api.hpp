#pragma once
#include <string>
#include <expected>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

class PlacesApi {
  std::string api_key_;

public:
  explicit PlacesApi(const std::string& api_key) : api_key_(api_key) {};
  explicit PlacesApi(std::string&& api_key) noexcept : api_key_(std::move(api_key)) {};
  PlacesApi(const PlacesApi&) = delete;
  PlacesApi& operator=(const PlacesApi&) = delete;
  PlacesApi(PlacesApi&& other) noexcept = default; 
  PlacesApi& operator=(PlacesApi&& other) noexcept = default;


  std::expected<nlohmann::json, std::string> GetPlaces(const double& latitude, const double& longitude, const std::string& place_type) const;
};