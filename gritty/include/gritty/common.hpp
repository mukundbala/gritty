#pragma once

/**
 * @file common.hpp
 * @brief Version macros, the expected / unexpected / md aliases, and detail math helpers.
 *
 * Shared by every gritty header, so keep it light. The math helpers live in gritty::detail.
 */

#define GRITTY_VERSION_MAJOR 0  ///< Major version of gritty.
#define GRITTY_VERSION_MINOR 1  ///< Minor version of gritty.
#define GRITTY_VERSION_PATCH 0  ///< Patch version of gritty.

// gritty code indexes mdspans as m(row, col). C++20 has no multi-argument operator[],
// so make sure the call operator is enabled. CMake also sets this on the gritty target;
// this guard covers builds that include the headers without CMake.
#ifndef MDSPAN_USE_PAREN_OPERATOR
#define MDSPAN_USE_PAREN_OPERATOR 1
#endif

#ifdef GRITTY_USE_SYSTEM_DEPS
#include <tl/expected.hpp>
#include <mdspan/mdspan.hpp>
#else
#include <gritty/third_party/tl/expected.hpp>
#include <gritty/third_party/mdspan/mdspan.hpp>
#endif

#include <cmath>
#include <cstdint>
#include <numbers>
#include <utility>

namespace gritty {

/**
 * @brief Result type holding a value of type T or an error of type E.
 *
 * Alias of tl::expected. Use `*r` only after checking `r`; never call `.value()`.
 *
 * @tparam T Value type.
 * @tparam E Error type, normally GridError.
 */
template <class T, class E>
using expected = tl::expected<T, E>;

/**
 * @brief Wrapper that builds the error side of an expected.
 *
 * Alias of tl::unexpected.
 *
 * @tparam E Error type, normally GridError.
 */
template <class E>
using unexpected = tl::unexpected<E>;

/** @brief Alias of the pinned mdspan namespace (Kokkos). Code uses `gritty::md::` only. */
namespace md = ::Kokkos;

}  // namespace gritty

// Internal helpers. Not part of the public API; excluded from Doxygen.
namespace gritty::detail {

// int32_t limits as doubles. Both are exactly representable, so comparisons are exact.
inline constexpr double kInt32MinD = -2147483648.0;
inline constexpr double kInt32MaxD =  2147483647.0;

/**
 * @brief Tells whether an already-floored double can be cast to int32_t without undefined behaviour.
 *
 * Call this before every double -> int32_t cast. NaN returns false, because every comparison fails.
 *
 * @param floored A value that has already been floored.
 * @return true if floored lies in [INT32_MIN, INT32_MAX].
 */
constexpr bool fitsInt32(double floored) noexcept {
  return floored >= kInt32MinD && floored <= kInt32MaxD;
}

/**
 * @brief Finds which quarter turn yaw sits on, if it sits on one.
 *
 * Shared by cosSnapped and sinSnapped.
 *
 * @param yaw Heading, in radians.
 * @pre yaw is finite (Geometry::create checks this first).
 * @return 0, 1, 2 or 3 if yaw is within tolerance of 0, pi/2, pi or 3pi/2 (modulo 2pi); -1 otherwise.
 */
// inline: an ordinary function defined in a header must be inline (one-definition rule).
inline int quarterTurnOf(double yaw) noexcept {
  constexpr double kQuarterTurn = std::numbers::pi / 2.0;
  constexpr double kTolerance   = 1e-12;  // in quarter turns, about 1.6e-12 rad

  const double quarters = yaw / kQuarterTurn;
  const double nearest  = std::round(quarters);

  if (std::abs(quarters - nearest) >= kTolerance) {
    return -1;
  }
  // nearest is a whole number; reduce it to 0..3. fmod keeps the sign, so fold negatives.
  int turn = static_cast<int>(std::fmod(nearest, 4.0));
  if (turn < 0) {
    turn += 4;
  }
  return turn;
}

/**
 * @brief Returns cos of yaw, snapped to an exact value at multiples of pi/2.
 *
 * std::cos(pi / 2) is about 6e-17, not 0. On a 90-degree map that tiny error can push a
 * point lying on a grid line into the neighbouring cell. Snapping removes it.
 *
 * @param yaw Heading, in radians.
 * @pre yaw is finite (Geometry::create checks this first).
 * @return cos(yaw).
 * @see sinSnapped
 */
inline double cosSnapped(double yaw) noexcept {
  constexpr double kCosAtQuarter[4] = {1.0, 0.0, -1.0, 0.0};
  const int turn = quarterTurnOf(yaw);
  return turn >= 0 ? kCosAtQuarter[turn] : std::cos(yaw);
}

/**
 * @brief Returns sin of yaw, snapped to an exact value at multiples of pi/2.
 *
 * See cosSnapped for why snapping is needed.
 *
 * @param yaw Heading, in radians.
 * @pre yaw is finite (Geometry::create checks this first).
 * @return sin(yaw).
 * @see cosSnapped
 */
inline double sinSnapped(double yaw) noexcept {
  constexpr double kSinAtQuarter[4] = {0.0, 1.0, 0.0, -1.0};
  const int turn = quarterTurnOf(yaw);
  return turn >= 0 ? kSinAtQuarter[turn] : std::sin(yaw);
}

}  // namespace gritty::detail