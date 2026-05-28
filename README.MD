# TTaskScheduler

A dependency-aware task scheduler in C++23. Tasks form a DAG — the scheduler resolves dependencies automatically and can execute independent tasks in parallel via a built-in thread pool.

Includes a real-world weather & places application built on top of the library.

## Usage

```cpp
TTaskScheduler scheduler;

auto a = scheduler.add([](int x) { return x * 2; }, 21);
auto b = scheduler.add([](int x) { return x + 1; }, 10);
auto c = scheduler.add(
    [](int x, int y) { return x + y; },
    a.getFutureResult<int>(),
    b.getFutureResult<int>()
);

scheduler.executeAllParallel();
// c result: 42 + 11 = 53
```

Dependencies are expressed through `TFuture<T>` — passing a future as an argument to `add()` creates an edge in the task graph. The scheduler resolves them lazily at execution time; no explicit graph construction needed.

## API

**`TTaskScheduler`**

| Method | Description |
|---|---|
| `add(func, args...)` | Registers a task, returns `TTask&` |
| `executeAll()` | Runs all tasks sequentially |
| `executeAllParallel()` | Runs independent tasks in parallel via thread pool |

**`TTask`**

| Method | Description |
|---|---|
| `getFutureResult<T>()` | Returns a `TFuture<T>` to pass as input to another task |
| `getResultSync<T>()` | Executes the task and returns its result immediately |
| `apply(func)` | Chains a new task that takes this task's output as input |

**`TFuture<T>`**

- Value type `T`: move-only. Calling `get()` twice throws `MyAlreadyMovedException`
- Reference type `const T&`: reference semantics, `get()` can be called multiple times

## Parallel execution

`executeAllParallel()` uses a thread pool sized to `std::thread::hardware_concurrency()`. Tasks are dispatched to worker threads via a shared queue protected by `std::mutex` and `std::condition_variable`. Workers pick up any available task and execute it; if a task has unresolved dependencies, they are resolved recursively on the same thread via `TFuture::get()`.

Task state (`NotProcessed → Processing → Processed`) is stored as `std::atomic<Conditions>`, with `compare_exchange_strong` ensuring that each task executes exactly once even under concurrent access.

## Implementation notes

- **Type erasure**: custom `Any` container instead of `std::any` (excluded by design)
- **Argument storage**: custom `MyTuple` + `myApply` for perfect forwarding into stored argument packs
- **Cycle detection**: three-state atomic enum; cycles throw `MyGraphCycleDetected`
- **Thread safety**: lock-free task state via `std::atomic`, mutex only on the task queue

## Weather & Places app

An application built on top of `TTaskScheduler`:

1. Determines current location via IP geolocation (2ip API)
2. Fetches weather and nearby places in parallel (Yandex Weather + 2GIS)
3. Filters venues by conditions — outdoor places are deprioritized in rain or wind > 8 m/s
4. Prints current conditions and top N recommended spots with distances

API keys are loaded from `.json` config files — see `app/api_key.json.example`.

## Build

```sh
cmake -S . -B build
cmake --build build

# run tests
ctest --test-dir build --verbose

# run the app
./build/bin/weather-app
```

Requirements: C++23, CMake ≥ 3.20, `cpr`, `nlohmann_json`, `GTest`

## Tech

C++23 · thread pool · `std::atomic` · `std::condition_variable` · type erasure · variadic templates · perfect forwarding · move semantics · DAG scheduling · CMake · Google Test
