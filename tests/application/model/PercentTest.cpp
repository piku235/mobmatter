#include "application/model/Percent.h"

#include <gtest/gtest.h>

using mmbridge::application::model::Percent;

TEST(PercentTest, Max)
{
    auto percent = Percent::max();

    ASSERT_EQ(10000, percent.value100ths());
    ASSERT_EQ(100, percent.value());
}

TEST(PercentTest, Min)
{
    auto percent = Percent::min();

    ASSERT_EQ(0, percent.value100ths());
    ASSERT_EQ(0, percent.value());
}

TEST(PercentTest, ConstructsFromValue)
{
    auto percent = Percent::from(23);

    ASSERT_TRUE(percent.has_value());
    ASSERT_EQ(2300, percent->value100ths());
    ASSERT_EQ(23, percent->value());
}

TEST(PercentTest, ConstructFailsFromValue)
{
    auto percent = Percent::from(101);

    ASSERT_FALSE(percent.has_value());
}

TEST(PercentTest, ConstructsFromValue100ths)
{
    auto percent = Percent::from100ths(2345);

    ASSERT_TRUE(percent.has_value());
    ASSERT_EQ(2345, percent->value100ths());
    ASSERT_EQ(23, percent->value());
}

TEST(PercentTest, ConstructFailsFromValue100ths)
{
    auto percent = Percent::from100ths(10100);

    ASSERT_FALSE(percent.has_value());
}

TEST(PercentTest, AddsAnother)
{
    auto percent = Percent::from(10);
    auto other = Percent::from100ths(1234);

    auto actual = *percent + *other;

    ASSERT_EQ(2234, actual.value100ths());
    ASSERT_EQ(22, actual.value());
}

TEST(PercentTest, AddOverflow)
{
    auto percent = Percent::from(90);
    auto other = Percent::from100ths(1234);

    auto actual = *percent + *other;

    ASSERT_EQ(10000, actual.value100ths());
    ASSERT_EQ(100, actual.value());
}

TEST(PercentTest, SubsAnother)
{
    auto percent = Percent::from100ths(1234);
    auto other = Percent::from(10);

    auto actual = *percent - *other;

    ASSERT_EQ(234, actual.value100ths());
    ASSERT_EQ(2, actual.value());
}

TEST(PercentTest, SubsOverflow)
{
    auto percent = Percent::from(10);
    auto other = Percent::from100ths(1234);

    auto actual = *percent - *other;

    ASSERT_EQ(0, actual.value100ths());
    ASSERT_EQ(0, actual.value());
}

TEST(PercentTest, Compare)
{
    ASSERT_EQ(Percent::from(10), Percent::from(10));
    ASSERT_EQ(Percent::from(10), Percent::from100ths(1000));
    ASSERT_NE(Percent::from(10), Percent::from100ths(1001));
    ASSERT_LT(Percent::from(10), Percent::from100ths(1001));
    ASSERT_LE(Percent::from(10), Percent::from100ths(1000));
    ASSERT_GT(Percent::from100ths(1001), Percent::from(10));
    ASSERT_GE(Percent::from100ths(1000), Percent::from(10));
}
