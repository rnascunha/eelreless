/**
 * @file functions.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MAIN_ADC_HPP_
#define MAIN_ADC_HPP_

#include <optional>
#include <cstdlib>

#include "adc/continuous.hpp"

#define EXAMPLE_ADC_UNIT                    ADC_UNIT_1
#define EXAMPLE_ADC_BUFFER_SIZE             4092
#define EXAMPLE_READ_LEN_BYTES              1400
#define EXAMPLE_READ_LEN                    (EXAMPLE_READ_LEN_BYTES / sizeof(uC::ADC_continuous::data))

[[nodiscard]] std::optional<uC::ADC_continuous>
initiate_adc() noexcept;
[[nodiscard]] bool
validate_data(uC::ADC_continuous::data*,
                    std::size_t) noexcept;
[[nodiscard]] double
process_adc_data(uC::ADC_continuous::data*,
                 std::size_t) noexcept;

#endif  // MAIN_ADC_HPP_
