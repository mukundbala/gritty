# gritty

Header-only, exception-free C++20 2D grid map library. Main class: `gritty::Grid<T>`. Replaces the Eigen-based `perception::map::GridHandler<CellT>`. Spec 2 (not started) grows it into a Nav2 `costmap_2d` parity library for benchmarking.

Source of truth: Spec 1, `.claude/plans/private/gritty_spec1.md` (gitignored). Appendix "C++20 Features Used in gritty" has the build order and per-feature explanations (§1–§11).

## How we work

Mukund builds gritty by hand to learn modern C++ (concepts, ranges, views, `span`, `mdspan`, `expected`). Learning beats speed.

- No builds, tests or compiler runs (`colcon`, `cmake`, `ctest`, `g++`) unless asked.
- No library code in `include/gritty/` unless explicitly asked. Allowed: scaffolding, CMake/build work, explaining, reviewing, debugging, tests when asked.
- Explain and review as teaching: name the C++ feature, say why it applies, cite the appendix section. Prefer hints and questions over full solutions unless asked.
- Review against Spec 1. Spec deviation: ask if intentional before calling it a bug.
- Stay in scope. "Later" stays later.

## Decisions (do not reopen without asking)

| Area | Decision |
| --- | --- |
| Language | C++20 only, no C++23 library features. Must build on GCC 11 (ROS 2 Humble) and newer. |
| Shape | Header-only. Per header: documented class declaration first, then out-of-class definitions. |
| Errors | No throwing. Must build and pass under `-fno-exceptions`. `tl::expected` as `gritty::expected` / `gritty::unexpected`. Never `.value()`. |
| Access | Pointer: `tryAt` → `T*`. Result: `get`, `valueOr`, `set` → `bool`. Unchecked: `operator[]`, `*Unchecked`, debug `assert`. |
| Types | Strong types: `Coord{int32 u, v}`, `Index{size_t}`, `WorldPt{double x, y}`, `CellPt`, `Rect`, `Pose2`, `MapMeta`. `Coord` stays int32. |
| Conversions | On `Geometry`. Bounded forms short-named (`toCoord`, `toIndex`); `toIndex(WorldPt)` goes world→index directly. Never invert a 3×3 per call. |
| Copying | Copy ctor/assign deleted; `clone()` is the only deep copy. Moves `noexcept`; moved-from grid is a valid 0×0 grid. |
| Views | kokkos/mdspan as `gritty::md`, built with `MDSPAN_USE_PAREN_OPERATOR=1`. `GridView<T, Layout>`, `subview(Rect)`, `tiles()`. `orientedView` read-only at every yaw. |
| Traversals | Lazy ranges, no heap. Exception: concave polygons over 64 vertices allocate once per call (scratch-buffer overload avoids it; convex needs none). Area order row-major, v ascending. Visitors are shortcuts over ranges, constrained by concept `CellVisitor`. |
| Eigen | Not in core. Inputs accept any `PointLike` via `point_traits`. Opt-in `gritty/eigen.hpp`: `toEigen` / `fromEigen`, `asEigen` views. |
| Hashing | Opt-in `gritty/hash.hpp`. Vendored rapidhash default; xxHash adapter only if `<xxhash.h>` exists. |
| Docs | Doxygen (`@brief`, `@tparam`, `@param`, `@return`, `@retval GridError::...`, `@pre`, `@warning`) on declarations only. Definitions get at most a short `//` explaining why. |
| Packaging | Core `gritty/`: plain CMake header-only `INTERFACE` target `gritty::gritty`, `package.xml` (`build_type cmake`). Headers install to `include/gritty/`. tl::expected and mdspan vendored in `include/gritty/third_party/`. |

## Naming

- Namespaces: `gritty`, `gritty::trace` (coordinate traversals), `gritty::geom` (free geometry functions), `gritty::hash`, `gritty::detail`, `gritty::md`.
- Types and concepts `PascalCase`. Functions `camelCase`. Private members `snake_case_`.

## Scope

- Now: core package `gritty/` only.
- Later, do not start: `gritty_ros` (OccupancyGrid conversions), `gritty_nav2` and Spec 2 (costmap parity), `gritty_bench`, buildfarm/vcpkg releases.
