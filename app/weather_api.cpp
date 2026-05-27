#include "weather_api.hpp"
#include <string>

std::expected<nlohmann::json, std::string> WeatherApi::GetWeather(const double& latitude, const double& longitude) const {
  auto response = cpr::Get(
    cpr::Url{"https://api.weather.yandex.ru/v2/forecast"},
    cpr::Parameters {
      {"lat", std::to_string(latitude)},
      {"lon", std::to_string(longitude)},
      {"lang", "ru_RU"},
      {"limit", "1"},
      {"hours", "true"},
      {"extra", "true"}
    },
    cpr::Header{
      {"X-Yandex-Weather-Key", api_key_}
    }
  );

  if (response.status_code == 200) {
    try {
      return nlohmann::json::parse(response.text);
    } catch (const std::exception& e) {
      return std::unexpected("JSON Weather API Parse error: " + std::string(e.what()));
    }
  } else {
    return std::unexpected("Weather API error: " + std::to_string(response.status_code));
  }
}