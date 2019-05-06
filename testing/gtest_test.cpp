#include <gtest/gtest.h>
#include "test.h"

class FooEnvironment:public testing::Environment{
public:
    virtual void SetUp()
    {
        std::cout << "Foo FooEnvironment SetUp" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "Foo FooEnvironment TearDown" << std::endl;
    }
};

TEST(Foo, Demo)
{
    EXPECT_DEATH(Foo(), "");        
}

int main(int argc, char** argv)
{
    testing::AddGlobalTestEnvironment(new FooEnvironment);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
