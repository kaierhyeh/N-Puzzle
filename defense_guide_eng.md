# N-Puzzle Defense Guide

This document covers the core concepts, workflow logic, algorithm choices, heuristic functions, and data structure design decisions of the program.

---

## 0. Language Choice

This project is implemented in **C++**. Below is a comparison of performance and development speed across different languages:

| Language | Performance | Development Speed | Built-in Data Structures (STL) | Overall Evaluation |
|---|---|---|---|---|
| **C** | ⭐⭐⭐ | ⭐ | ❌ Requires manual implementation of Queue/Set | High development cost and prone to memory leaks |
| **C++** | ⭐⭐⭐ | ⭐⭐ | ✅ `priority_queue`, `unordered_set` | **Best balance between performance and development efficiency** |
| **Python** | ⭐ | ⭐⭐⭐ | ✅ Built-in `heapq`, `set` | Too slow; often times out on 4x4 boards |

For an A* algorithm that needs to expand tens of thousands of nodes per second, C++ is the best choice for this project.

---

## 1. Core Concept & Workflow

**Goal**: Transform a random N×N puzzle into a **Snail (spiral) goal configuration** by sliding the empty tile (0).

**Program workflow**:
1. **Input & Parsing**: Read the board from a file or generate a random solvable board.
2. **Solvability Check**: Before running expensive search, determine if the puzzle is solvable using inversion parity.
   - **Why is this critical?**
     Mathematically, for a random 3x3 puzzle, there is a **50% chance it is unsolvable**. Without this check, A* would attempt to explore millions of states, eventually exhausting memory and crashing. This check prevents catastrophic failure by instantly rejecting impossible cases.
3. **A\* Search Initialization**: Insert the starting state into the Open List.
4. **State Expansion**:
   Repeatedly extract the state with the lowest $f(n)$:
   - If it is the goal → stop.
   - Otherwise, generate all valid neighbor states (move 0 up/down/left/right) and insert them into the queue.
5. **Path Reconstruction**:
   Once the goal is found, reconstruct the solution by following parent pointers back to the initial state.

---

## 2. Search Algorithm & Variants

**Implemented Algorithm**: Standard **A\* (A-Star) Search**

**Why A\*?**
A* combines:
- **Dijkstra’s Algorithm** → guarantees optimality via real cost $g(n)$
- **Greedy Best-First Search** → improves efficiency using heuristic $h(n)$

Final scoring function:
**$f(n) = g(n) + h(n)$**

### Variants implemented:

- **Uniform-Cost Search (UCS)**
  $f(n) = g(n)$ (force $h = 0$)
  - Explores uniformly like a wave
  - Very slow but guarantees optimal solution

- **Greedy Best-First Search**
  $f(n) = h(n)$ (force $g = 0$)
  - Extremely fast and memory efficient
  - Does **not guarantee shortest path**

---

### 📌 Capability & Limits

| Algorithm | 3x3 | 4x4 | 5x5+ (e.g., 10x10) | Optimal? | Bottleneck |
|---|---|---|---|---|---|
| **A\*** ($g+h$) | ⚡ Instant | ✅ Solvable | ❌ OOM / extremely slow | ✅ Yes | Memory explosion ($O(b^d)$) |
| **Uniform-Cost** | ✅ Slow | ❌ Very slow / OOM | ❌ Impossible | ✅ Yes | Blind search |
| **Greedy** | ⚡ Instant | ⚡ Instant | ✅ **Only viable** | ❌ No | Local optima |

> 💡 **Defense Tip**
> For large puzzles like 10x10, switch to Greedy (`-a greedy`).
> Finding the optimal solution is mathematically infeasible due to the state space size ($100! / 2$).
> Greedy sacrifices optimality but remains practical.

---

## 3. Heuristics & Admissibility

> **Admissible heuristic**: never overestimates the true cost.

### A. Manhattan Distance (Required)
- Sum of horizontal + vertical distances of each tile to its goal.
- **Proof**: Each move shifts a tile by 1 → cannot overestimate.

### B. Hamming Distance
- Number of misplaced tiles.
- **Proof**: Each misplaced tile requires at least 1 move.

### C. Linear Conflict (Best Performance)
- Adds +2 moves when two tiles are in the correct row/column but reversed.
- **Proof**: One tile must move out and back → at least 2 extra moves.

**Result**: Dramatically reduces node expansions (millions → hundreds of thousands on 4x4).

---

## 4. Data Structures

### 1. Board Representation (`std::vector<int>`)
- 1D array instead of 2D
- **Reason**: Better cache locality and faster copying

### 2. Open List (`std::priority_queue`)
- Retrieves lowest $f(n)$ in $O(\log N)$
- Efficient Min-Heap structure

### 3. Closed Set (`std::unordered_set<Puzzle>`)
- Prevents revisiting states
- Average lookup: $O(1)$ via hashing

### 4. Path Map (`std::unordered_map<Puzzle, shared_ptr<Node>>`)
- Stores parent pointers and node metadata
- `shared_ptr` avoids memory leaks

---

## 5. Code Reading Guide

1. **Core Layer**: `Puzzle.hpp / .cpp`
   - Board structure and solvability check

2. **Parsing Layer**: `Parser.hpp / .cpp`
   - File input and random generation

3. **Heuristics Layer**: `Heuristics.hpp / .cpp`
   - Manhattan & Linear Conflict explanations

4. **Solver Layer**: `Solver.hpp / .cpp`
   - A* loop, priority queue, closed set

5. **Main Entry**: `main.cpp`
   - Integration and performance timing

---

## 6. Visual Demo: Web Visualizer

During defense, open `visualizer.html`:

1. **Parity Demonstration**
   - Show that standard row-major goals and snail goals have different parity
   - Their state spaces are **disconnected**

2. **Solution Replay (Autoplay)**
   - Visualize the solving process step-by-step