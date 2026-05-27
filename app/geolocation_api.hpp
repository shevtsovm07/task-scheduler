#pragma once
#include <string>
#include <expected>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

class IpApi {
  std::string api_key_;

public:
  explicit IpApi(const std::string& key) : api_key_(key) {};
  explicit IpApi(std::string&& key) noexcept : api_key_(std::move(key)) {};
  IpApi(const IpApi&) = delete;
  IpApi& operator=(const IpApi&) = delete;
  IpApi(IpApi&& other) noexcept = default; 
  IpApi& operator=(IpApi&& other) noexcept = default;

  std::expected<nlohmann::json, std::string> getIp();
};