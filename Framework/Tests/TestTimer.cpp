#include <gtest/gtest.h>

#include <o2/Utils/System/Time/Timer.h>

#include <iostream>
#include <thread>
#include <cmath>

TEST(TestTimer, test)
{
    o2::Timer t1;

    t1.Reset();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    auto v = t1.GetTime();

    std::cout << "GetTime: " << v << std::endl;
    ASSERT_EQ(5, (int)std::floor(v));
}