# TODO

- [ ] Use uc/include/uc.hpp to select correct ADC format mode;
- [x] ~~remove "esp_log.h" dependencie (make sys::log work correctly)~~. New log library dependes of fmt. Still needs to investigate memory usage.
- [ ] Make strong types ('enum class' macro constants, for example)
- [ ] Create top namespace?
- [x] ~~Simplify WiFi station/ap config~~
- [ ] Document it;
- [x] Make FMT library as a dependency.
- [ ] Wrap freeRTOS functions

## Test

- [x] ~~Make script to compile all tests~~
- [ ] Make scripts to run tests on devices
- [ ] Make tests (Google Tests) on components that don't need to upload to device (facilities ip4 and mac)