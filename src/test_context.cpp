#include <gtest/gtest.h>
#include <indiemotion/context.hpp>

using namespace indiemotion;

TEST(ContextUpdateCommitTest, BasicAssertions)
{
    Context ctx;
    EXPECT_TRUE(ctx.get("keyA").empty());
    EXPECT_TRUE(ctx.get("keyB").empty());
    EXPECT_TRUE(ctx.get("keyA/nested").empty());

    ctx.update("keyA", std::string("hello"));
    ctx.update("keyB", 2);
    ctx.update("keyA/nested", true);

    EXPECT_EQ(ctx.get("keyA").as<std::string>(), "hello");
    EXPECT_EQ(ctx.get("keyB").as<int>(), 2);
    EXPECT_EQ(ctx.get("keyA/nested").as<bool>(), true);

    EXPECT_TRUE(ctx.previous("keyA").empty());
    EXPECT_TRUE(ctx.previous("keyB").empty());
    EXPECT_TRUE(ctx.previous("keyA/nested").empty());

    ctx.commit();

    EXPECT_EQ(ctx.get("keyA").as<std::string>(), "hello");
    EXPECT_EQ(ctx.get("keyB").as<int>(), 2);
    EXPECT_EQ(ctx.get("keyA/nested").as<bool>(), true);

    EXPECT_EQ(ctx.current("keyA").as<std::string>(), "hello");
    EXPECT_EQ(ctx.current("keyB").as<int>(), 2);
    EXPECT_EQ(ctx.current("keyA/nested").as<bool>(), true);

    EXPECT_TRUE(ctx.previous("keyA").empty());
    EXPECT_TRUE(ctx.previous("keyB").empty());
    EXPECT_TRUE(ctx.previous("keyA/nested").empty());

    ctx.update("keyA", std::string("world"));
    ctx.update("keyB", 99);
    ctx.update("keyA/nested", false);

    EXPECT_EQ(ctx.get("keyA").as<std::string>(), "world");
    EXPECT_EQ(ctx.get("keyB").as<int>(), 99);
    EXPECT_EQ(ctx.get("keyA/nested").as<bool>(), false);

    EXPECT_EQ(ctx.current("keyA").as<std::string>(), "hello");
    EXPECT_EQ(ctx.current("keyB").as<int>(), 2);
    EXPECT_EQ(ctx.current("keyA/nested").as<bool>(), true);

    EXPECT_TRUE(ctx.previous("keyA").empty());
    EXPECT_TRUE(ctx.previous("keyB").empty());
    EXPECT_TRUE(ctx.previous("keyA/nested").empty());

    ctx.commit();

    EXPECT_EQ(ctx.previous("keyA").as<std::string>(), "hello");
    EXPECT_EQ(ctx.previous("keyB").as<int>(), 2);
    EXPECT_EQ(ctx.previous("keyA/nested").as<bool>(), true);

    EXPECT_EQ(ctx.get("keyA").as<std::string>(), "world");
    EXPECT_EQ(ctx.get("keyB").as<int>(), 99);
    EXPECT_EQ(ctx.get("keyA/nested").as<bool>(), false);

    EXPECT_EQ(ctx.current("keyA").as<std::string>(), "world");
    EXPECT_EQ(ctx.current("keyB").as<int>(), 99);
    EXPECT_EQ(ctx.current("keyA/nested").as<bool>(), false);

}