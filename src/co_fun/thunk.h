// thunk.h                                                            -*-C++-*-
#ifndef INCLUDED_THUNK
#define INCLUDED_THUNK

//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:


namespace co_fun {

template <typename R>
class Thunk {
    R r_;
  public:
    Thunk(R r) : r_{r} {}

    operator R() const {return r_;}
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================


}  // close package namespace

#endif
