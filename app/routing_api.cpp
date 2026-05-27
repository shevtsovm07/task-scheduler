#include "routing_api.hpp"
#include <iostream>

std::expected<nlohmann::json, std::string> MatrixApi::GetDistance(
  const double& latitude_departure,
  const double& longitude_departure,
  const std::vector<Place>& found_places
  ) const {

    nlohmann::json points = nlohmann::json::array();
    points.push_back(nlohmann::json::object({ {"lat", latitude_departure}, {"lon", longitude_departure} }));
    int places_amount = 0;
    for (const auto& place : found_places) {
      points.push_back(nlohmann::json::object({ {"lat", place.lat_}, {"lon", place.lon_} }));
      places_amount++;
    }

    nlohmann::json body;
    body["points"] = points;
    body["sources"] = nlohmann::json::array({0});
    nlohmann::json targets_ids = nlohmann::json::array();
    for (int place = 1; place <= places_amount; place++) {
      targets_ids.push_back(place);
    }
    body["targets"] = targets_ids;
    body["transport"] = "walking";

    auto response = cpr::Post(
      cpr::Url{"https://routing.api.2gis.com/get_dist_matrix"},
      cpr::Parameters {
        {"key", api_key_},
        {"version", "2.0"}
      },
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{body.dump()}
    );

  if (response.status_code == 200) {
    try {
      return nlohmann::json::parse(response.text);
    } catch (const std::exception& e) {
      return std::unexpected("JSON Distance API Parse error: " + std::string(e.what()));
    }
  } else {
    return std::unexpected("Distance API error: " + std::to_string(response.status_code));
  }
  }