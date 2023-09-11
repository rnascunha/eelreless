# TODO

- [ ] Use uc/include/uc.hpp to select correct ADC format mode;
- [x] ~~remove "esp_log.h" dependencie (make sys::log work correctly)~~. New log library dependes of fmt. Still needs to investigate memory usage.
- [ ] Make strong types ('enum class' macro constants, for example)
- [ ] Create top namespace?
- [x] ~~Simplify WiFi station/ap config~~
- [ ] Document it;
- [x] ~~Make FMT library as a dependency.~~
- [ ] Wrap freeRTOS functions
- [ ] Wrap http:serve::uri to work better with Websocket
- [ ] Standarize the use of words (like handler, native... to return the underlying handler);

## Fix

- [ ] Websocket closed is been called to any HTTP connection, when used in conjuction to other HTTPs resources
- [ ] User defined types at lg (log) component not working correctly.
- [ ] `uc/serial` not finding `QueueHandle_t` type. Ignoring.

## Test

- [x] ~~Make script to compile all tests~~
- [ ] Make scripts to run tests on devices
- [ ] Make tests (Google Tests) on components that don't need to upload to device (facilities ip4 and mac)