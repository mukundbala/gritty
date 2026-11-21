---
name: gritty-document
description: Write Doxygen documentation blocks on gritty header declarations, following the project's Documentation Conventions. Use when the user highlights a type or method, names a class or method, or says "all" and asks to document it.
---

# gritty-document

Write Doxygen blocks on declarations in `gritty/include/gritty/`. The user invoking this skill is the explicit request to edit those headers. Edit only comments.

## Target

Resolve what to document, in this order:

1. The user's IDE selection, if one is provided.
2. A class, type, concept or method named in the request. Find it with Grep in `gritty/include/gritty/`. Include all its overloads.
3. "all": every public declaration in every header under `gritty/include/gritty/`, excluding `third_party/`.

If a name matches several places and the intent is unclear, ask which one.

## Sources of truth

- `.claude/CLAUDE.md`, row "Docs" in the Decisions table.
- `.claude/plans/private/gritty_spec1.md`, section "Documentation Conventions", for the block style. Read that section before writing.
- The spec as background only, for intent, errors, preconditions, complexity and invalidation rules. The code is the authority: the project does not follow the spec strictly, so document the code as it is.
- Do not compare declarations against the spec, and do not ask whether a difference is intentional. Do not document spec behaviour the code does not have. If the intended behaviour of a declaration is unclear from the code, the spec and its name, ask the user. Do not invent behaviour.
- A declaration the spec does not mention is documented like any other. Do not mark it as an addition to the spec.

## Where blocks go

- Doxygen blocks go on declarations only. Per header, the documented class declaration comes first and the out-of-class definitions below it.
- Never add a doc block to a definition. A definition may carry at most a short `//` comment saying why the code is as it is (an algorithm reference, an invariant being kept, a non-obvious edge case). Do not add one unless it is clearly needed. Leave existing `//` comments as they are.
- Document public declarations. Skip private members.
- Every header keeps its one-line `@file` comment. Do not change it unless the header's contents no longer match it.
- Do not change code: no signatures, attributes, includes, whitespace in code, or definitions. Comments only.

## Block format

```cpp
/**
 * @brief Converts a world point directly to a flat cell index.
 *
 * Runs world -> grid -> index in one call using the cached inverse pose.
 * The point maps to the cell that contains it (floor, not round).
 *
 * @tparam P Any PointLike type (WorldPt, Eigen::Vector2d, geometry_msgs::msg::Point).
 * @param p Point in the world frame, in metres.
 * @return Index of the containing cell.
 * @retval GridError::OutOfBounds The point lies outside the map.
 * @retval GridError::NonFinite   p.x or p.y is NaN or infinite.
 * @retval GridError::Overflow    The point is beyond the int32 cell range.
 * @see toCoord, toIndexUnchecked
 */
```

## Rules

- `@brief` is one line. Detail goes after a blank line.
- `@tparam` for every template parameter, naming the concept it must satisfy.
- `@param` for every parameter, with its unit (metres, cells, radians) and frame (world, map, grid).
- `@return` for every non-void function. For `expected` returns, add one `@retval GridError::...` line per error the call can produce. Take the list from the body if one exists, otherwise from the declaration's intent, and use the spec only as a hint.
- Never write `@throws`. Nothing throws. A function that may allocate says so in a `@note`.
- `@pre` for every precondition of an unchecked function. It must match the debug `assert`.
- `@note` gives the complexity of anything above O(1): traversals, fills, `shiftOrigin`.
- `@warning` for invalidation. Any function returning a view or traversal says which calls invalidate it (Datastructure Setters and Clears invalidate; Cell Modifiers do not).
- `@see` links overload families and the checked/unchecked pairs.
- Group members with `@name` and `@{ ... @}`, using the spec's section names, so the generated docs have the same 13 groups: Class Declarations, Datastructure Setters, Datastructure Clears, Getters, Setters, Conversions, Checks, Neighborhood, Cell Accessors, Cell Modifiers, Views, Iterators, Hashing.
- Put the classes' own `@brief` and `@tparam` on the class declaration, with its invariants if the code or the spec lists any.
- Use the project's names: `gritty::expected`, `GridError`, `Coord`, `WorldPt`, and so on. Spell units out ("metres", "cells", "radians").
- Keep blocks tight. Do not restate what the signature already says.

## Existing blocks

If a declaration already has a block, bring it into line with these rules and keep any correct detail the user wrote. Do not rewrite wording that already follows the rules.

## Do not

- Run builds, tests, Doxygen or the compiler. This repo's rule is no compiler runs unless the user asks.
- Touch `third_party/`.
- Write library code, new declarations, or stubs for things the headers do not yet declare.

## Report

Reply with what was documented (file and symbol names) and any open questions: behaviour you could not infer from the code, and anything in the code that looks wrong (a compile error, a typo). Do not report differences from the spec. Keep it short.
