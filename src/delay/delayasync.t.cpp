#include <delay/delayasync.h>

#include "gtest/gtest.h"
using ::testing::Test;

namespace testing {
namespace {
int func_called;
int func2_called;
int func3_called;
}
class DelayAsyncTest : public Test {
protected:
  DelayAsyncTest() {
  }
  ~DelayAsyncTest() {
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

TEST_F(DelayAsyncTest, breathingTest) {
  DelayAsync<int> D1(1);

  int j{D1};

  EXPECT_EQ(1, j);

  DelayAsync<int> D2(func);
  EXPECT_EQ(0, func_called);

  int k = D2;
  EXPECT_EQ(1, func_called);
  EXPECT_EQ(5, k);

  DelayAsync<int> D3 = func;

  DelayAsync<int> D4 = func2(3);
  EXPECT_EQ(1, func2_called);

  DelayAsync<int> D5([]() { return func2(7); });

  DelayAsync<int> D6 = delayAsync(func2, 8);

  DelayAsync<int> D7 = delayAsync(func3, 8, 1);

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

  EXPECT_EQ(5, DelayAsync<int>(func).get());
  EXPECT_EQ(5, force(DelayAsync<int>(func)));
}

TEST_F(DelayAsyncTest, moveTest) {
  std::string str;
  DelayAsync<std::string> d1(str);
  DelayAsync<std::string> d2("test");
  DelayAsync<std::string> d3 = delayAsync(stringTest, "this is a test");
  DelayAsync<std::string> d4([](){return stringTest("another test");});

  EXPECT_TRUE(d1.isForced());
  EXPECT_TRUE(d2.isForced());
  EXPECT_FALSE(d3.isForced());
  EXPECT_FALSE(d4.isForced());

  EXPECT_EQ(std::string("this is a test"), force(d3));
  EXPECT_EQ(std::string("another test"), force(d4));
}
}
