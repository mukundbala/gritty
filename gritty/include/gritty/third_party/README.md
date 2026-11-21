# Vendored third-party code

Reached only through `gritty/config.hpp` (`gritty::expected`, `gritty::unexpected`, `gritty::md`).
Build with `-DGRITTY_USE_SYSTEM_DEPS=ON` to use `find_package` copies instead.

| Dependency | Version | Upstream | Licence | Path |
| --- | --- | --- | --- | --- |
| tl::expected | v1.3.1 (`include/tl/expected.hpp`, unmodified) | https://github.com/TartanLlama/expected | CC0-1.0 (`tl/LICENSE`) | `tl/expected.hpp` |
| kokkos/mdspan | `stable` branch, commit `fd7a1b843faffe510ed41ee5c79365c64e8ad581` (2026-10-26); `include/mdspan/` copied unmodified | https://github.com/kokkos/mdspan | Apache-2.0 with LLVM exception (`mdspan/LICENSE`, `mdspan/COPYRIGHT.md`) | `mdspan/` |

Notes

- mdspan namespace at this commit: `Kokkos` (`std::experimental` in 0.6.0 and older).
- Upstream ships no pre-built single header (only `make_single_header.py`), so the header tree is copied as is. `submdspan` is included via `mdspan/mdspan.hpp`.
- gritty requires `MDSPAN_USE_PAREN_OPERATOR=1`; the CMake target sets it for every consumer.
