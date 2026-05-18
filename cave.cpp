#include "cave.h"

/**
 * IOI 2013 P4 Cave
 * Fully commented C++ implementation
 *
 * This file contains an implementation of the interactive solution entry point
 * exploreCave(int N) expected by the problem grader. The implementation follows
 * a common constructive approach:
 *  - Discover the correct position (0 or 1) for each switch in order.
 *  - For each discovered switch, binary-search the door index that the switch controls.
 *
 * Notes about the interactive environment
 *  - tryCombination(int S[]) is provided by the judge and returns the index of
 *    the first closed door when switches are set to S, or -1 if all doors are open.
 *  - answer(int a[], int b[]) is provided by the judge and must be called with:
 *      a[d] = correct position for the switch that controls door d
 *      b[d] = index of the switch that controls door d
 *
 * Limits and assumptions
 *  - N <= 5000 (array sizes sized accordingly).
 *  - The number of calls to tryCombination must remain within the judge limit.
 *  - This implementation assumes the judge will call exploreCave(N).
 *
 * Author: adapted and annotated for clarity
 * Date: 2026-05-17
 */

#include <bits/stdc++.h>
#include "cave.h"
#pragma GCC optimize("O3")
#define FOR(i, x, y) for(int i = x; i < y; i++)
typedef long long ll;
using namespace std;

/*
 * exploreCave
 *
 * High level idea
 *  - We iterate i from 0 to N-1. At step i we determine:
 *      1) known[i] the correct position (0 or 1) of the switch that will be
 *         assigned as the i-th discovered switch.
 *      2) D[i] the door index controlled by that switch.
 *
 *  - The algorithm maintains the partial assignment for the first i discovered
 *    switches. When testing the i-th switch we set the already discovered
 *    switches to their known correct positions so they behave as if fixed.
 *
 *  - To determine known[i] we set all switches to 1 except we set the positions
 *    of already discovered switches to their known values. We then call
 *    tryCombination and inspect the returned first-closed-door index. If the
 *    returned index is <= i (or not -1) we infer one value for known[i], otherwise
 *    we infer the other. This inference relies on the fact that the first i
 *    doors are already controlled by the previously discovered switches and
 *    their known positions.
 *
 *  - To find D[i] we perform a binary search on the door index range [0, N-1].
 *    Each binary search query constructs a test configuration T that places the
 *    candidate switches in positions that allow us to decide whether the true
 *    door index is in the left or right half of the current search interval.
 *
 * Important correctness notes
 *  - The algorithm relies on the invariant that after discovering j < i switches
 *    and fixing their positions in queries, the first j doors are effectively
 *    "handled" and will not interfere with the detection of the i-th switch's
 *    door when we interpret tryCombination results.
 *  - The binary search uses the response semantics of tryCombination carefully:
 *    when tryCombination returns an index <= i (and not -1) it indicates that
 *    some door among the first i+1 doors is closed, which is used to guide the
 *    binary search decisions.
 *
 * Complexity
 *  - For each i we perform O(log N) calls for the binary search plus a small
 *    constant number of calls to determine known[i]. Overall calls are O(N log N).
 *
 * Implementation detail
 *  - Arrays sized to 5000 to match constraints.
 *  - We keep two final arrays a and b to pass to answer:
 *      a[d] = known position for the switch that controls door d
 *      b[d] = index of the switch that controls door d
 */
void exploreCave(int N) {
    // Arrays sized to maximum N allowed by problem statement
    int known[5000]; // known[i] stores the correct position (0 or 1) for the i-th discovered switch
    int D[5000];     // D[i] stores the door index controlled by the i-th discovered switch
    int S[5000];     // S used to build switch configurations for tryCombination
    int T[5000];     // T used for binary search test configurations

    // Initialize known[] to -1 meaning "unknown yet"
    fill(known, known + N, -1);

    // Main discovery loop: discover switches one by one
    FOR(i, 0, N) {
        // Step 1 Determine known[i] the correct position for the i-th discovered switch
        //
        // Strategy:
        //  - Set all switches to 1 initially.
        //  - For previously discovered switches j < i, set the switch that controls
        //    door D[j] to its known position known[j]. This simulates fixing those
        //    switches to their correct values so they won't interfere unpredictably.
        //  - Call tryCombination(S). If the returned index is <= i and not -1,
        //    we infer known[i] = 0; otherwise known[i] = 1.
        //
        // Rationale:
        //  - The response from tryCombination tells us whether any of the first
        //    i+1 doors is closed under this configuration. By choosing the base
        //    pattern carefully we can deduce the correct bit for the new switch.
        fill(S, S + N, 1);                // default all switches to 1
        FOR(j, 0, i) S[D[j]] = known[j];  // fix previously discovered switches

        // Query the grader with this configuration
        int res = tryCombination(S);

        // Interpret the response to deduce known[i]
        // If res <= i and res != -1 then some door among the first i+1 doors is closed.
        // The code uses this condition to decide known[i] = 0, otherwise known[i] = 1.
        //
        // Why this works:
        //  - When the i-th switch is set to the wrong value relative to the rest,
        //    it will cause its door to be closed and that door may appear among the
        //    first i+1 doors. The exact reasoning depends on the invariant maintained
        //    by the algorithm and the ordering of discoveries.
        if (res <= i && res != -1) known[i] = 0;
        else known[i] = 1;

        // Step 2 Binary search for D[i] the door index controlled by the i-th discovered switch
        //
        // We search the door index in the range [0, N-1].
        // At each step we build a test configuration T that sets:
        //  - For indices in [l, mid] we set the switch positions to known[i]
        //    (the candidate value for the i-th switch).
        //  - For indices outside that range we set switches to the opposite value.
        //  - Previously discovered switches are fixed to their known positions.
        //
        // After calling tryCombination(T) we inspect the returned index:
        //  - If res <= i and res != -1 then the first closed door is among the
        //    first i+1 doors, so we move the search to the right half (l = mid + 1).
        //  - Otherwise we move to the left half (r = mid).
        //
        // This binary search is tailored to the problem semantics and the way we
        // interpret tryCombination results.
        int l = 0, r = N - 1;
        while (l != r) {
            int mid = (l + r) / 2;

            // Build test configuration T for this mid
            //  - Default to the opposite of known[i] so that only the candidate range
            //    [l, mid] uses known[i]. This creates a contrast that the grader's
            //    response can detect.
            fill(T, T + N, 1 - known[i]);

            // Set the candidate interval [l, mid] to known[i]
            // Note fill uses inclusive end mid + 1
            fill(T + l, T + mid + 1, known[i]);

            // Fix previously discovered switches to their known positions
            FOR(j, 0, i) T[D[j]] = known[j];

            // Query the grader with this test configuration
            int res = tryCombination(T);

            // Interpret the response to update binary search bounds
            // If the first closed door is among the first i+1 doors (res <= i and res != -1)
            // then the i-th switch's door is in the right half of the current interval.
            if (res <= i && res != -1) l = mid + 1;
            else r = mid;
        }

        // After binary search completes l == r and is the discovered door index
        D[i] = l;
    }

    // Build the final arrays expected by answer
    // a[d] = correct position for the switch that controls door d
    // b[d] = index of the switch that controls door d
    int a[5001], b[5001];
    FOR(i, 0, N) {
        // D[i] is the door controlled by the i-th discovered switch
        // known[i] is the correct position for that switch
        a[D[i]] = known[i];
        b[D[i]] = i;
    }

    // Submit the final answer to the grader
    answer(a, b);
}