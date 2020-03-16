/*
 * Copyright (C) 2020 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mock_logger.h"

#include <multipass/logging/log.h>
#include <multipass/top_catch_all.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>

namespace mp = multipass;
namespace mpl = multipass::logging;
namespace mpt = multipass::test;
using namespace testing;

namespace
{
struct TopCatchAll : public Test
{
    void SetUp() override
    {
        mock_logger = std::make_shared<StrictMock<mpt::MockLogger>>();
        mpl::set_logger(mock_logger);
    }

    void TearDown() override
    {
        mock_logger = nullptr;
        mpl::set_logger(mock_logger);
    }

    std::shared_ptr<StrictMock<mpt::MockLogger>> mock_logger = nullptr;
};
} // namespace

TEST_F(TopCatchAll, calls_function_with_no_args)
{
    int ret = 123, got = 0;
    EXPECT_NO_THROW(got = mp::top_catch_all("", [ret] { return ret; }););
    EXPECT_EQ(got, ret);
}

TEST_F(TopCatchAll, calls_function_with_args)
{
    int a = 5, b = 7, got = 0;
    EXPECT_NO_THROW(got = mp::top_catch_all("", std::plus<int>{}, a, b););
    EXPECT_EQ(got, a + b);
}

TEST_F(TopCatchAll, handles_unknown_error)
{
    const std::string category = "testing";
    int got = 0;

    EXPECT_CALL(*mock_logger, log(Eq(mpl::Level::error), Property(&mpl::CString::c_str, StrEq(category.c_str())),
                                  Property(&mpl::CString::c_str, HasSubstr("unknown"))));
    EXPECT_NO_THROW(got = mp::top_catch_all(category, [] {
                        throw 123;
                        return 0;
                    }););
    EXPECT_EQ(got, EXIT_FAILURE);
}
