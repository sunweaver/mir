/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Daniel van Vugt <daniel.van.vugt@canonical.com>
 */

#include "mir/geometry/rectangle.h"
#include "src/server/compositor/occlusion.h"
#include "mir_test_doubles/fake_renderable.h"

#include <gtest/gtest.h>
#include <memory>

using namespace testing;
using namespace mir::geometry;
using namespace mir::compositor;
namespace mg=mir::graphics;
namespace mtd=mir::test::doubles;

struct OcclusionFilterTest : public Test
{
    OcclusionFilterTest()
    {
        monitor_rect.top_left = {0, 0};
        monitor_rect.size = {1920, 1200};
    }

    Rectangle monitor_rect;
};

TEST_F(OcclusionFilterTest, single_window_not_occluded)
{
    auto window = std::make_shared<mtd::FakeRenderable>(12, 34, 56, 78);
    mg::RenderableList list{window};
 
    filter_occlusions_from(list, monitor_rect);
    ASSERT_EQ(1u, list.size());
    EXPECT_EQ(window, list.front());
}

TEST_F(OcclusionFilterTest, smaller_window_occluded)
{
    auto front = std::make_shared<mtd::FakeRenderable>(10, 10, 10, 10);
    auto back = std::make_shared<mtd::FakeRenderable>(12, 12, 5, 5);
    mg::RenderableList list{front, back};

    filter_occlusions_from(list, monitor_rect);

    ASSERT_EQ(1u, list.size());
    EXPECT_EQ(front, list.front());
}

TEST_F(OcclusionFilterTest, translucent_window_occludes_nothing)
{
    auto front = std::make_shared<mtd::FakeRenderable>(10, 10, 10, 10, 0.5f);
    auto back = std::make_shared<mtd::FakeRenderable>(12, 12, 5, 5, 1.0f);
    mg::RenderableList list{front, back};

    filter_occlusions_from(list, monitor_rect);

    ASSERT_EQ(2u, list.size());
    EXPECT_EQ(front, list.front());
    EXPECT_EQ(back, list.back());
}

TEST_F(OcclusionFilterTest, hidden_window_is_self_occluded)
{
    auto window = std::make_shared<mtd::FakeRenderable>(10, 10, 10, 10, 1.0f, true, false);
    mg::RenderableList list{window};

    filter_occlusions_from(list, monitor_rect);

    EXPECT_EQ(0u, list.size());
}

#if 0
TEST_F(OcclusionFilterTest, hidden_window_occludes_nothing)
{
    OcclusionFilter filter(monitor_rect);

    FakeRenderable front(10, 10, 10, 10, 1.0f, true, false);
    EXPECT_TRUE(filter(front));

    FakeRenderable back(12, 12, 5, 5);
    EXPECT_FALSE(filter(back));
}

TEST_F(OcclusionFilterTest, shaped_window_occludes_nothing)
{
    OcclusionFilter filter(monitor_rect);

    FakeRenderable front(10, 10, 10, 10, 1.0f, false, true);
    EXPECT_FALSE(filter(front));

    FakeRenderable back(12, 12, 5, 5);
    EXPECT_FALSE(filter(back));
}

TEST_F(OcclusionFilterTest, identical_window_occluded)
{
    OcclusionFilter filter(monitor_rect);

    FakeRenderable front(10, 10, 10, 10);
    EXPECT_FALSE(filter(front));

    FakeRenderable back(10, 10, 10, 10);
    EXPECT_TRUE(filter(back));
}

TEST_F(OcclusionFilterTest, larger_window_never_occluded)
{
    OcclusionFilter filter(monitor_rect);

    FakeRenderable front(10, 10, 10, 10);
    EXPECT_FALSE(filter(front));

    FakeRenderable back(9, 9, 12, 12);
    EXPECT_FALSE(filter(back));
}

TEST_F(OcclusionFilterTest, cascaded_windows_never_occluded)
{
    OcclusionFilter filter(monitor_rect);

    for (int x = 0; x < 10; x++)
    {
        FakeRenderable win(x, x, 200, 100);
        ASSERT_FALSE(filter(win));
    }
}

TEST_F(OcclusionFilterTest, some_occluded_and_some_not)
{
    OcclusionFilter filter(monitor_rect);

    FakeRenderable front(10, 20, 400, 300);
    EXPECT_FALSE(filter(front));

    EXPECT_TRUE(filter(FakeRenderable(10, 20, 5, 5)));
    EXPECT_TRUE(filter(FakeRenderable(100, 100, 20, 20)));
    EXPECT_TRUE(filter(FakeRenderable(200, 200, 50, 50)));

    EXPECT_FALSE(filter(FakeRenderable(500, 600, 34, 56)));
    EXPECT_FALSE(filter(FakeRenderable(200, 200, 1000, 1000)));
}

TEST(OcclusionMatchTest, remembers_matches)
{
    OcclusionMatch match;
    FakeRenderable win(1, 2, 3, 4);

    EXPECT_FALSE(match.occluded(win));
    match(win);
    EXPECT_TRUE(match.occluded(win));
}
#endif
