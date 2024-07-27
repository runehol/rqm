#include "rqm/rqm.h"

#include <gtest/gtest.h>

TEST(RQM, instatiation)
{
    rqm::num one = 1;
    EXPECT_EQ(one.to_int32_t(), 1);
}
