#include "app.hpp"
#include <fstream>
#include <string>
#include <iostream>

std::string LoadApiKey(const std::string& config_path) {
  std::ifstream file(config_path);
  if (!file.is_open()) {
    throw std::runtime_error("Файл c API не найден: " + config_path);
  }
  nlohmann::json config;
  file >> config;
  if (config.contains(ApiConstants::kApiKey)) {
    return config[ApiConstants::kApiKey];
  }
  throw std::runtime_error("В файле нет поля api_key !");
}

template <typename T>
T unwrap(std::expected<T, std::string>&& exp) {
  if (!exp) {
    throw std::runtime_error(exp.error());
  }
  return std::move(exp.value());
}

std::vector<std::string> PlacesTypeBasedOnWeather(const nlohmann::json& weather_resp) {
  int prec_type = weather_resp["fact"]["prec_type"].get<int>();
  double wind_speed = weather_resp["fact"]["wind_speed"].get<double>();
  double temperature = weather_resp["fact"]["temp"].get<double>();
  bool is_bad_weather = (prec_type != WeatherConditions::kNoPrec) || (wind_speed > WeatherConditions::kStrongWind) || (temperature < WeatherConditions::kCold);
  if (is_bad_weather) {
    return std::vector<std::string>{KindsOfPlaces::kMuseums, KindsOfPlaces::kRestaurants,
            KindsOfPlaces::kTheaters, KindsOfPlaces::kMalls};
  } else {
    return std::vector<std::string>{KindsOfPlaces::kMuseums, KindsOfPlaces::kRestaurants,
            KindsOfPlaces::kTheaters, KindsOfPlaces::kMalls, KindsOfPlaces::kParks};
  }
}

void WeatherInfoOut(const nlohmann::json& weather) {
  std::cout << "\n" << "Погода на данный момент: " << "\n";
  std::cout << "\n" << "| Температура воздуха: " << weather["fact"]["temp"] << "\n";
  std::cout << "\n" << "| Скорость ветра: " << weather["fact"]["wind_speed"] << "\n";
  std::cout << "\n" << "| Осадки: ";
  auto prec = weather["fact"]["prec_type"].get<int>();
  if (prec == WeatherConditions::kNoPrec) {
    std::cout << "отсутствуют" << "\n";
  } else if (prec == WeatherConditions::kRain) {
    std::cout << "дождь" << "\n";
  } else if (prec == WeatherConditions::kRainAndSnow) {
    std::cout << "дождь со снегом" << "\n";
  } else if (prec == WeatherConditions::kSnow) {
    std::cout << "снег" << "\n";
  } else if (prec == WeatherConditions::kHail) {
    std::cout << "град" << "\n";
  }
  std::cout << "------------------------------------" << "\n";
  std::cout << "\n" <<  "========= КУДА СХОДИТЬ СЕГОДНЯ ? =========" << "\n" << "\n";
}



void RunApp() {

  try{

    std::string ip_api_key = LoadApiKey(ApiConstants::kIpApiFilename);
    std::string weather_api_key = LoadApiKey(ApiConstants::kWeatherApiFilename);
    std::string places_api_key = LoadApiKey(ApiConstants::kPlacesApiFilename);
    std::string matrix_api_key = LoadApiKey(ApiConstants::kMatrixApiFilename);
  
    IpApi ip_api(ip_api_key);
    WeatherApi weather_api(weather_api_key);
    PlacesApi places_api(places_api_key);
    MatrixApi matrix_api(matrix_api_key);

    TTaskScheduler task_scheduler;

    auto& get_ip = task_scheduler.add([&ip_api](){return unwrap(ip_api.getIp());});

    auto location_future = get_ip.getFutureResult<const nlohmann::json&>();

    auto& get_weather = task_scheduler.add([&weather_api](const nlohmann::json& location){
      auto lat = std::stod(location["lat"].get<std::string>());
      auto lon = std::stod(location["lon"].get<std::string>());
      return unwrap(weather_api.GetWeather(lat, lon));
    }, location_future);

    auto weather_future = get_weather.getFutureResult<const nlohmann::json&>();

    auto& weather_out = task_scheduler.add([](const nlohmann::json& weather){return WeatherInfoOut(weather);}, weather_future);

    auto& recomended_places = task_scheduler.add([](const nlohmann::json& weather){return PlacesTypeBasedOnWeather(weather);}, weather_future);

    auto recomended_places_future = recomended_places.getFutureResult<const std::vector<std::string>&>();

    auto& get_places = task_scheduler.add([&places_api](const std::vector<std::string>& places_types, const nlohmann::json& location){
      std::vector<Place> all_places;
      auto lat = std::stod(location["lat"].get<std::string>());
      auto lon = std::stod(location["lon"].get<std::string>());
      for (const auto& type : places_types) {
        auto places = unwrap(places_api.GetPlaces(lat, lon, type));
        for (const auto& place : places["result"]["items"]) {
          all_places.push_back(Place(
            place["point"]["lat"].get<double>(),
            place["point"]["lon"].get<double>(),
            place["name"].get<std::string>()
          ));
        }
      }
      return all_places;
    }, recomended_places_future, location_future);

    auto places_future = get_places.getFutureResult<const std::vector<Place>&>();

    auto& get_distances = task_scheduler.add([&matrix_api](
      const std::vector<Place>& places,
      const nlohmann::json& location
    ){
      double lat = std::stod(location["lat"].get<std::string>());
      double lon = std::stod(location["lon"].get<std::string>());
      return unwrap(matrix_api.GetDistance(lat, lon, places));
    }, places_future, location_future);

    auto distances_future = get_distances.getFutureResult<const nlohmann::json&>();

    auto& print_task = task_scheduler.add([](
      const std::vector<Place>& places,
      const nlohmann::json& distances
    ){
      auto& routes = distances["routes"];
      for (size_t i = 0; i < places.size(); i++) {
        int dist = routes[i]["distance"].get<int>();
        std::cout << places[i].name_ << " — " << dist << " метров\n";
      }
    }, places_future, distances_future);

    task_scheduler.executeAll();

  } catch (const std::exception& e) {
    std::cerr << "Ошибка API: " << e.what() << std::endl;
  }
}