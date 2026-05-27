#include "places_api.hpp"
#include <string>

std::expected<nlohmann::json, std::string> PlacesApi::GetPlaces(const double& latitude, const double& longitude, const std::string& place_type) const {
  auto response = cpr::Get(
    cpr::Url{"https://catalog.api.2gis.com/3.0/items"},
    cpr::Parameters {
      {"q", place_type},
      {"location", std::to_string(longitude) + "," + std::to_string(latitude)},
      {"key", api_key_},
      {"radius", "2000"},
      {"fields", "items.point"},
      {"page_size", "2"}
    }
  );

  if (response.status_code == 200) {
    try {
      return nlohmann::json::parse(response.text);
    } catch (const std::exception& e) {
      return std::unexpected("JSON Places API Parse error: " + std::string(e.what()));
    }
  } else {
    return std::unexpected("Places API error: " + std::to_string(response.status_code));
  }
}