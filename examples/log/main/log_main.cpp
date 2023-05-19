/**
 * @file log_main.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <string_view>

#include "lg/level.hpp"
#include "lg/log.hpp"
#include "lg/format_types.hpp"

#include "facility/ip4.hpp"
#include "facility/mac.hpp"

#include "sys/error.hpp"

template<typename Config>
void print_all() {
  lg::verbose<Config>("TAG", "This is a verbose. {}", 32);
  lg::debug<Config>("TAG", "This is a debug.");
  lg::info<Config>("TAG", "This is a info.");
  lg::warn<Config>("TAG", "This is a warn.");
  lg::error<Config>("TAG", "This is a error.");
}

template<typename Config>
void print_all(const lg::log<Config>& l) {
  l.verbose("This is a verbose. {}", 32);
  l.debug("This is a debug.");
  l.info("This is a info.");
  l.warn("This is a warn.");
  l.error("This is a error.");
}

extern "C" void app_main() {
  auto separator = [](std::string_view title = "") {
    lg::print("{:-^40}\n", title);
  };

  lg::print(fmt::bg(fmt::color::green), "This is bg colored color\n");
  lg::print(fmt::fg(fmt::color::blue), "This is fg colored color\n");
  lg::print("This is a local {} colored\n", fmt::styled(42, fmt::emphasis::bold));

  separator("Free functions");
  print_all<lg::default_config>();
  separator("No breaking lines");
  print_all<lg::config<false>>();
  lg::print("\n");
  separator("no color");
  print_all<lg::config<true, false>>();
  separator("system timestamp");
  print_all<lg::config<true, true, lg::system_timestamp>>();

  separator("Log class");
  print_all(lg::log("MyTag"));
  separator("no color");
  print_all(lg::log<lg::config<true, false>>("MyTag"));


  separator("User defined types");
  separator("sys::error");
  auto err = sys::error{ESP_OK};
  lg::info("Error", "{}", err);
  lg::info("Error", "{:v}", err);
  lg::info("Error", "{:m}", err);
  lg::info("Error", "{:b}", err);

  using namespace facility::literals;
  separator("facility::ip4");
  lg::info("IP", "{}", facility::ip4{});
  lg::info("IP", "{}", facility::ip4{192, 168, 0, 1});
  lg::info("IP", "{}", "192.169.3.1"_ip4);

  separator("facility::mac");
  lg::info("MAC", "{}", facility::mac{});
  lg::info("MAC", "{}", facility::mac{0x32, 0xa, 0x3f, 0xab, 0xe1, 0x00});
  lg::info("MAC", "{}", "00:12:3:a:bc:f"_mac);
  separator("Mac uppercase");
  lg::info("MAC", "{:X}", facility::mac{});
  lg::info("MAC", "{:X}", facility::mac{0x32, 0xa, 0x3f, 0xab, 0xe1, 0x00});
  lg::info("MAC", "{:X}", "00:12:3:a:bc:f"_mac);

  separator("END");
}