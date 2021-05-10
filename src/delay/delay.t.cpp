#include <delay/delay.h>

#include "gtest/gtest.h"
using ::testing::Test;

namespace testing {
namespace {
int func_called;
int func2_called;
int func3_called;
}
class DelayTest : public Test {
protected:
  DelayTest() {
  }
  ~DelayTest() {
  }

  virtual void SetUp() {
    func_called = 0;
    func2_called = 0;
    func3_called = 0;
  }

  virtual void TearDown() {
  }

public:
};

namespace {
int func() {
  func_called++;
  return 5;
}

int func2(int i) {
  func2_called++;
  return i;
}

int func3(int i, int j) {
  func3_called++;
  return i + j;
}

std::string stringTest(const char* str) {
  return str;
}
}


TEST_F(DelayTest, breathingTest) {
  Delay<int> D1(1);

  int j{D1};

  EXPECT_EQ(1, j);

  Delay<int> D2(func);
  EXPECT_EQ(0, func_called);

  int k = D2;
  EXPECT_EQ(1, func_called);
  EXPECT_EQ(5, k);

  Delay<int> D3 = func;

  Delay<int> D4 = func2(3);
  EXPECT_EQ(1, func2_called);

  Delay<int> D5([]() { return func2(7); });

  Delay<int> D6 = delay(func2, 8);

  Delay<int> D7 = delay(func3, 8, 1);

  EXPECT_EQ(1, func_called);
  EXPECT_EQ(1, func2_called);
  EXPECT_EQ(0, func3_called);

  EXPECT_EQ(5, D3.get());
  EXPECT_EQ(2, func_called);

  EXPECT_EQ(3, D4.get());
  EXPECT_EQ(1, func2_called);

  EXPECT_EQ(7, D5.get());
  EXPECT_EQ(2, func2_called);

  EXPECT_EQ(8, D6.get());
  EXPECT_EQ(3, func2_called);

  EXPECT_EQ(9, D7.get());
  EXPECT_EQ(1, func3_called);

  EXPECT_EQ(2, func_called);
  EXPECT_EQ(3, func2_called);
  EXPECT_EQ(1, func3_called);

  EXPECT_EQ(1, static_cast<int>(D1));
  EXPECT_EQ(5, static_cast<int>(D2));
  EXPECT_EQ(5, static_cast<int>(D3));
  EXPECT_EQ(3, static_cast<int>(D4));
  EXPECT_EQ(7, static_cast<int>(D5));
  EXPECT_EQ(8, static_cast<int>(D6));
  EXPECT_EQ(9, force(D7));

  EXPECT_EQ(2, func_called);
  EXPECT_EQ(3, func2_called);
  EXPECT_EQ(1, func3_called);

  EXPECT_EQ(5, Delay<int>(func).get());
  EXPECT_EQ(5, force(Delay<int>(func)));
}

TEST_F(DelayTest, moveTest) {
  std::string str;
  Delay<std::string> d1(str);
  Delay<std::string> d2("test");
  Delay<std::string> d3 = delay(stringTest, "this is a test");
  Delay<std::string> d4([](){return stringTest("another test");});

  EXPECT_TRUE(d1.isForced());
  EXPECT_TRUE(d2.isForced());
  EXPECT_FALSE(d3.isForced());
  EXPECT_FALSE(d4.isForced());

  EXPECT_EQ(std::string("this is a test"), force(d3));
  EXPECT_EQ(std::string("another test"), force(d4));
}
}
