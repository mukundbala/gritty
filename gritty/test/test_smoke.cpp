#include <gritty/gritty.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstddef>

TEST(Smoke, ExpectedHoldsValueAndError) {
  gritty::expected<int, int> ok = 7;
  ASSERT_TRUE(ok.has_value());
  EXPECT_EQ(*ok, 7);

  gritty::expected<int, int> bad = gritty::unexpected(3);
  ASSERT_FALSE(bad.has_value());
  EXPECT_EQ(bad.error(), 3);
  EXPECT_EQ(bad.value_or(-1), -1);
}

TEST(Smoke, Mdspan2DParenOperator) {
  std::array<int, 6> data{0, 1, 2, 3, 4, 5};
  gritty::md::mdspan<int, gritty::md::dextents<std::size_t, 2>> m(data.data(), 2, 3);
  EXPECT_EQ(m.extent(0), 2U);
  EXPECT_EQ(m.extent(1), 3U);
  EXPECT_EQ(m(1, 2), 5);  // (v, u), row-major
  m(0, 1) = 42;
  EXPECT_EQ(data[1], 42);
}
