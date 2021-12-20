#include <co_fun/co_fun.h>
#include <co_fun/thunk.h>

#include <iostream>

/*

function Z(f) {
   function innerZfunction1(x) {
      const xx = v => {
         return x(x)(v);
      };
      return f(xx);
   };
   function innerZfunction2(x) {
      const xx = v => {
         return x(x)(v);
      };
      return f(xx);
   };
   return innerZfunction1(innerZfunction2);
*/
int main(int /*argc*/, char** /*argv*/) {
    using namespace co_fun;

    constexpr auto selfcirc = [](auto x) { return x(x); };

    auto y = ([selfcirc](auto f) {
        return selfcirc([f](auto g) -> std::function<int(int)> {
            return f([g](auto args) { return (g(g))(args); });
        });
    });

    auto fact = [](auto f) {
        return [f](int x) {
            if (x == 0) {
                return 1;
            } else {
                return x * f(x - 1);
            }
        };
    };

    auto factorial = y(fact);
    std::cout << factorial(5);
}
