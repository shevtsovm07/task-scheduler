#include "geolocation_api.hpp"

std::expected<nlohmann::json, std::string> IpApi::getIp() {
  auto response = cpr::Get(cpr::Url{api_key_});
  if (response.status_code == 200) {
    try {
      return nlohmann::json::parse(response.text);
    } catch (const std::exception& e){
      return std::unexpected("JSON IP API Parse error: " + std::string(e.what()));
    }
  }
  return std::unexpected("IP API error: " + std::to_string(response.status_code));
};