add_executable(weather-app
  main.cpp
)

target_link_libraries(weather-app PRIVATE
  scheduler-lib
  weather-app-lib
  cpr::cpr
  nlohmann_json::nlohmann_json
)