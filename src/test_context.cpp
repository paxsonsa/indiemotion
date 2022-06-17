#include <gtest/gtest.h>
#include <indiemotion/context.hpp>

using namespace indiemotion;

TEST(ContextUpdateCommitTest, BasicAssertions)
{
    Context ctx;

    ctx.update("keyA", std::string("hello"));
    ctx.update("keyB", 100);
    ctx.update("keyC", false);
    ASSERT_EQ(ctx.saved.get("keyA").has_value(), false);
    ASSERT_EQ(ctx.saved.get("keyB").has_value(), false);
    ASSERT_EQ(ctx.saved.get("keyC").has_value(), false);

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 100);
    ASSERT_EQ(ctx.current["keyC"].as<bool>(), false);

    ctx.save();

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 100);
    ASSERT_EQ(ctx.current["keyC"].as<bool>(), false);

    ctx.update("keyA", std::string("hello, world"));
    ctx.update("keyB", 50);

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello, world");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 50);
    ASSERT_EQ(ctx.current["keyC"].as<bool>(), false);

    ASSERT_EQ(ctx.saved["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.saved["keyB"].as<int>(), 100);
    ASSERT_EQ(ctx.saved["keyC"].as<bool>(), false);

    ASSERT_EQ(ctx.revision(1).get("keyA").has_value(), false);
    ASSERT_EQ(ctx.revision(1).get("keyB").has_value(), false);
    ASSERT_EQ(ctx.revision(1).get("keyC").has_value(), false);

    ctx.update("keyA", std::string("hello"));
    ctx.update("keyB", 100);

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 100);

    ASSERT_EQ(ctx.revision(1).get("keyA").has_value(), false);
    ASSERT_EQ(ctx.revision(1).get("keyB").has_value(), false);

    ctx.update("keyA", std::string("hello, world"));
    ctx.update("keyB", 50);

    ctx.save();

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello, world");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 50);

    ASSERT_EQ(ctx.revision(1)["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.revision(1)["keyB"].as<int>(), 100);

    auto rev = ctx.rollback();
    ASSERT_EQ(rev["keyA"].as<std::string>(), "hello, world");
    ASSERT_EQ(rev["keyB"].as<int>(), 50);

    ASSERT_EQ(ctx.current["keyA"].as<std::string>(), "hello");
    ASSERT_EQ(ctx.current["keyB"].as<int>(), 100);

    ASSERT_EQ(ctx.revision(1).get("keyA").has_value(), false);
    ASSERT_EQ(ctx.revision(1).get("keyB").has_value(), false);

    ctx.update("keyC", true);
    ASSERT_EQ(ctx.pending["keyC"].as<bool>(), true);
    ASSERT_EQ(ctx.current["keyC"].as<bool>(), true);

    ASSERT_EQ(ctx.pending.get("keyA").has_value(), false);
    ASSERT_EQ(ctx.current.get("keyA").has_value(), true);

    ctx.clear_pending();
    ASSERT_EQ(ctx.pending.get("keyC").has_value(), false);
    ASSERT_EQ(ctx.current.get("keyC").has_value(), true);
}