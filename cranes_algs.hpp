///////////////////////////////////////////////////////////////////////////////
// cranes_algs.hpp
//
// Algorithms that solve the crane unloading problem.
//
// All of the TODO sections for this project reside in this file.
//
// This file builds on crane_types.hpp, so you should familiarize yourself
// with that file before working on this file.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <math.h>

#include "cranes_types.hpp"

namespace cranes {

// Solve the crane unloading problem for the given grid, using an exhaustive
// optimization algorithm.
//
// This algorithm is expected to run in exponential time, so the grid's
// width+height must be small enough to fit in a 64-bit int; this is enforced
// with an assertion.
//
// The grid must be non-empty.

// 1. Generate all possible combinations of 1s and 0s (no recursion, just use two loops) (one function only)
// 2. Use bit shift to see what position you have 1s and 0s
// 3. Use add_step helper functions according to your ones and zeros (convert bit strings to 1s and 0s)
// 4. Pass candidate to is valid function
// 5. best = calculate the shortest lengths and max cranes (vectors of steps)

path crane_unloading_exhaustive(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);

  // Compute maximum path length, and check that it is legal.
  const size_t max_steps = setting.rows() + setting.columns() - 2;
  assert(max_steps < 64);


  path best(setting);
  for (size_t k = 0; k < pow(2, max_steps); k++) {
      path b(setting);

      std::vector<step_direction> candidate;
      for(size_t i = 0; i < max_steps; i++) {
        int bit = (k >> i) & 1;
        if(bit == 0) {
          candidate.push_back(STEP_DIRECTION_EAST);
        } else {
          candidate.push_back(STEP_DIRECTION_SOUTH);
        }
      }
      for(step_direction c: candidate) {
        if(!b.is_step_valid(c))
          break;
        else {
            b.add_step(c);
        }

        if(b.total_cranes() >= best.total_cranes()) {
          best = b;
        }
      }

    }
  return best;
}

// Solve the crane unloading problem for the given grid, using a dynamic
// programming algorithm.
// The grid must be non-empty.

path crane_unloading_dyn_prog(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);
  using cell_type = std::optional<path>;

  std::vector<std::vector<cell_type> > A(setting.rows(),
                                        std::vector<cell_type>(setting.columns()));

  A[0][0] = path(setting);
  assert(A[0][0].has_value());

  for (coordinate r = 0; r < setting.rows(); ++r) {
    for (coordinate c = 0; c < setting.columns(); ++c) {

      if (setting.get(r, c) == CELL_BUILDING){
        A[r][c].reset();
        continue;
        }

    cell_type from_above = std::nullopt;
    cell_type from_left = std::nullopt;

    //Implementation
    if (r > 0 && setting.get(r-1, c) != CELL_BUILDING && A[r-1][c].has_value()) {
      
        from_above.emplace(A[r-1][c].value());
        from_above->add_step(STEP_DIRECTION_SOUTH);
       
    }

    if (c > 0 && setting.get(r, c-1) != CELL_BUILDING && A[r][c-1].has_value()) {
      
        from_left.emplace(A[r][c-1].value());
        from_left->add_step(STEP_DIRECTION_EAST);
      
    }
    
    if (from_above.has_value() && from_left.has_value())
    {
        if (from_left->total_cranes() > from_above->total_cranes())
        {
           A[r][c] = from_left;
        }
        else
        {
           A[r][c] = from_above;
        }
    }
    else if (from_above.has_value())
    {
      A[r][c] = from_above;
    }
    else if (from_left.has_value())
    {
      A[r][c] = from_left;
    }

    }
  }
   
   cell_type* best = &(A[0][0]);
    assert(best->has_value());

    for (coordinate r = 0; r < setting.rows(); ++r)
      {
          for (coordinate c = 0; c < setting.columns(); ++c)
          {
              if (A[r][c]->total_cranes() > (*best)->total_cranes() && A[r][c].has_value())
              {
                  best = &(A[r][c]);
              }
          }
      }
   assert(best->has_value());
    std::cout << "total cranes" << (**best).total_cranes() << std::endl;

   return **best;
	}

}