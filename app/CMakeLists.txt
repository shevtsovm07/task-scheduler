add_library(weather-app-lib
  app.cpp
  geolocation_api.cpp
  routing_api.cpp
  places_api.cpp
  weather_api.cpp
)

target_link_libraries(weather-app-lib PRIVATE
  scheduler-lib
  cpr::cpr
  nlohmann_json::nlohmann_json
)

target_include_directories(weather-app-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})