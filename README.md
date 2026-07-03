## How to run this problem
Download from Online Judge

Include only these files in your build:
grader.cpp
cave.cpp
cave.h

Place your test input sample.in to cave.in by copying in the correct directory.

Running CMake will compile the judge and your solution together.

## Solution Summary 

The solution to IOI 2013 Cave uses a constructive strategy to identify both the correct position of each switch and the specific door it controls. It processes switches one by one: for each switch, it first determines whether its correct state is 0 or 1 by fixing all previously discovered switches and observing how the judge responds to a test configuration. Once the correct state is known, the algorithm performs a binary search over all door indices to find which door the switch controls, again using carefully crafted switch configurations to interpret the judge’s returned “first closed door.” By maintaining the invariant that earlier doors are already resolved, each new switch can be isolated and identified efficiently. The final mapping of switch positions and door assignments is then returned to the grader.
