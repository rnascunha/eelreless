#ifndef COMPONENTS_FACILITY_DEBOUNCE_HPP_
#define COMPONENTS_FACILITY_DEBOUNCE_HPP_

#include <cassert>

#include "sys/sys.hpp"
#include "uc/gpio.hpp"

namespace facility {

struct debounce {
  constexpr
  debounce(uc::gpio&& button, int limit) noexcept
   : btn_(button), limit_(limit) {
    assert(limit > 0 && "Limit must be positive");
   }

  constexpr void
  reset() noexcept {
    count_ = 0;
  }

  bool
  check() noexcept {
    if (btn_.read() == 0) {
      if (++count_ == limit_) {
        count_ = 0;
        return true;
      }
    } else
      count_ = 0;
    return false;
  }

  template<typename Duration,
           typename Callable,
           typename ...Args>
  void wait(Duration&& dt, Callable&& call, Args&&... args) noexcept {
    while(true) {
      if (check()) {
        call(std::forward<Args>(args)...);
        break;
      }
      sys::delay(dt);
    }
  }

 private:
  uc::gpio  btn_;
  int limit_;
  int count_ = 0;
};

}  // namespace facility

#endif  // COMPONENTS_FACILITY_DEBOUNCE_HPP_