#include "rqm/rqm.h"

#include <gtest/gtest.h>
#include <iostream>
#include <rapidcheck/gtest.h>
#include <string>

TEST(RQM, instantiation)
{
    rqm::num one = 1;
    EXPECT_EQ(one.to_int64_t(), 1);
}

RC_GTEST_PROP(RQM, comparison, (int64_t ia, int64_t ib))
{
    rqm::num a = ia;
    rqm::num b = ib;
    RC_ASSERT((a == b) == (ia == ib));
    RC_ASSERT((a != b) == (ia != ib));
    RC_ASSERT((a < b) == (ia < ib));
    RC_ASSERT((a <= b) == (ia <= ib));
    RC_ASSERT((a > b) == (ia > ib));
    RC_ASSERT((a >= b) == (ia >= ib));
}

RC_GTEST_PROP(RQM, negate, (int64_t ia))
{
    rqm::num a = ia;
    RC_ASSERT(-a == -ia);
}
