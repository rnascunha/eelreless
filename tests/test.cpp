#include <cmath>
#include <cassert>
#include <cstdint>

#include <iterator>
#include <type_traits>
#include <algorithm>

#include <iostream>

namespace signal {

template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

struct access {
  template<typename T>
  static constexpr auto&
  get(const T& obj) noexcept {
    if constexpr (std::is_class_v<T>)
      return obj.value();
    else
      return obj;
  }

  template<typename T>
  static constexpr auto&
  get(T& obj) noexcept {
    if constexpr (std::is_class_v<T>)
      return obj.value();
    else
      return obj;
  }
};

template<typename T>
T normalize(T value, T mini, T maxi) noexcept {
  static_assert(std::is_arithmetic_v<T>, 
                "Must be a arithmetic value");
  return value * mini / maxi;
}

template<typename Iter, typename Accessor = access>
auto mean(Iter begin, Iter end) noexcept {
  std::size_t size = 0;
  using value_type = remove_cvref_t<decltype(*begin)>;
  value_type sum = 0;
  while (begin != end) {
    sum += Accessor::get(*begin);
    ++size;
    ++begin;
  }
  return sum / size;
}

template<typename Iter, typename T, typename Accessor = access>
void remove_constant(Iter begin, Iter end, T dc) noexcept {
  std::transform(begin, end, begin, [&dc](auto v) {
    return Accessor::get(v) - dc;
  });
}

template<typename Iter, typename Accessor = access>
void filter_first_order(Iter begin, Iter end, double weight) noexcept {
  assert(weight >= 0 && weight <= 1 && "Weight must be 0 <= weight <= 1");

  Iter next = begin++;
  while (next != end) {
    Accessor::get(*next) = weight * Accessor::get(*begin) + 
                           (1 - weight) * Accessor::get(*next);
    next = begin++;
  }
}

template<typename Iter, typename Accessor = access>
auto rms_sine_square(Iter begin, Iter end) noexcept {
  std::size_t size = 0;
  using value_type = remove_cvref_t<decltype(*begin)>;
  value_type sum = 0.0;
  while (begin != end) {
    sum += Accessor::get(*begin) * Accessor::get(*begin);
    ++size;
    ++begin;
  }
  return sum / size;
}

template<typename Iter, typename Accessor = access>
auto rms_sine(Iter begin, Iter end) noexcept {
  return std::sqrt(rms_sine_square<Iter, Accessor>(begin, end));
}

template<typename IterIn, typename IterOut, typename Accessor = access>
void convert(IterIn begin, IterIn end, IterOut out) {
    // Circuit polarization parameters for currente sensor
    double Vmax = 2.450;							// Maximum voltage ADC can read [V];
    double Vmin = 0.120;							// Minimum voltage ADC can read [V];                     
    int d_max = 2895;                              // Max decimal value correlated with Vmax on 12 bit range;
    // double GND  = 0.0;								// gnd for ADC converter [V];
    double Vdc = 3.3;								// Voltage divisor supply [V];
    double R1 = 180.0*1000;							// Voltage divisor top resistor [Ohms];
    double R2 = 120.0*1000;							// Voltage divisor bottom resistor [Ohms];
    double V_R2 = R2/(R1+R2)*Vdc;					// Voltage over R2 [V] or Vref for ADC converter [V];

    // double Rb1 = 0.0;								// Burden resistor (bias) [Ohms];
    double Rb2 = 120.0;								// Burden resistor (bias) [Ohms];
    double N1 = 1;									// Current transformer sensor ration parameters
    double N2 = 2000;								// Current transformer sensor ration parameters

    std::transform(begin, end, out, [&](auto v) {
        return (v * (Vmax - Vmin) / (d_max) + Vmin - V_R2) * (1 / Rb2) * (N2 / N1);
    });
}

}  // namespace signal

int main() {
    std::uint16_t data[] = {
        1417, 1420, 1420, 1417, 1413, 1417, 1422, 1423, 1421, 1422, 1437, 1426, 1434, 1434,
        1429, 1439, 1437, 1437, 1443, 1434, 1445, 1447, 1449, 1450, 1453, 1447, 1450, 1449,
        1456, 1456, 1461, 1456, 1462, 1456, 1462, 1460, 1470, 1459, 1467, 1467, 1475, 1471,
        1474, 1482, 1475, 1475, 1474, 1483, 1488, 1483, 1490, 1488, 1494, 1490, 1498, 1494,
        1501, 1500, 1510, 1504, 1513, 1502, 1510, 1511, 1520, 1519, 1522, 1521, 1522, 1520,
        1529, 1527, 1535, 1533, 1545, 1535, 1547, 1546, 1552, 1552, 1552, 1553, 1555, 1555,
        1563, 1559, 1568, 1573, 1571, 1569, 1576, 1571, 1580, 1580, 1582, 1582, 1584, 1582,
        1584, 1583, 1591, 1591, 1591, 1593, 1597, 1599, 1601, 1600, 1609, 1605, 1613, 1605,
        1607, 1611, 1612, 1612, 1611, 1611, 1616, 1601, 1618, 1616, 1627, 1619, 1619, 1626,
        1626, 1623, 1627, 1628, 1631, 1633, 1631, 1629, 1632, 1629, 1631, 1630, 1633, 1635,
        1634, 1631, 1632, 1633, 1636, 1639, 1636, 1638, 1636, 1633, 1637, 1646, 1634, 1632,
        1649, 1638, 1639, 1637, 1639, 1637, 1638, 1641, 1635, 1639, 1634, 1639, 1637, 1639,
        1633, 1635, 1628, 1633, 1631, 1629, 1626, 1630, 1627, 1629, 1623, 1628, 1617, 1625,
        1616, 1616, 1611, 1612, 1610, 1614, 1613, 1610, 1601, 1601, 1603, 1602, 1600, 1603,
        1600, 1598, 1594, 1598, 1591, 1590, 1587, 1587, 1584, 1586, 1585, 1584, 1584, 1579,
        1579, 1583, 1571, 1577, 1571, 1558, 1566, 1570, 1577, 1566, 1568, 1562, 1563, 1559,
        1555, 1553, 1549, 1552, 1535, 1549, 1535, 1546, 1535, 1535, 1529, 1531, 1530, 1527,
        1526, 1522, 1545, 1519, 1517, 1520, 1514, 1511, 1505, 1513, 1507, 1509, 1498, 1503,
        1491, 1497, 1491, 1488, 1489, 1486, 1486, 1485, 1478, 1485, 1475, 1481, 1469, 1472,
        1467, 1471, 1467, 1467, 1457, 1460, 1456, 1457, 1456, 1459, 1451, 1454, 1455, 1452,
        1441, 1446, 1445, 1443, 1437, 1440, 1436, 1439, 1433, 1435, 1426, 1424, 1424, 1424,
        1422, 1423, 1424, 1419, 1417, 1417, 1411, 1414, 1408, 1409, 1409, 1410, 1409, 1408,
        1407, 1405, 1408, 1405, 1408, 1407, 1402, 1405, 1406, 1406, 1399, 1407, 1399, 1405,
        1403, 1399, 1395, 1399, 1401, 1398, 1401, 1402, 1402, 1405, 1402, 1397, 1394, 1402,
        1403, 1403, 1402, 1403, 1403, 1399, 1405, 1405, 1406, 1407, 1408, 1407, 1409, 1411
    };
    auto bd = std::begin(data);
    auto ed = std::end(data); 

    double output[sizeof(data) / sizeof(data[0])]{};
    auto bout = std::begin(output);
    auto eout = std::end(output);

    signal::filter_first_order(bd, ed, 0.8);
    signal::convert(bd, ed, bout);
    signal::remove_constant(bout, eout,
                            signal::mean(bout, eout));

    std::cout << signal::rms_sine(bout, eout) << "\n";

    return 0;
}
