#pragma once
#include <string>
#include <expected>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "place.hpp"


class MatrixApi {
  std::string api_key_;

public:
  explicit MatrixApi(const std::string& api_key) : api_key_(api_key) {};
  explicit MatrixApi(std::string&& api_key) noexcept : api_key_(std::move(api_key)) {};
  MatrixApi(const MatrixApi&) = delete;
  MatrixApi& operator=(const MatrixApi&) = delete;
  MatrixApi(MatrixApi&& other) noexcept = default; 
  MatrixApi& operator=(MatrixApi&& other) noexcept = default;


  std::expected<nlohmann::json, std::string> GetDistance(
    const double& latitude_departure,
    const double& longitude_departure,
    const std::vector<Place>& found_places
    ) const;
};