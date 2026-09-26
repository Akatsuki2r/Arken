# Arken Design System — Implementation Guide

Technical companion to `Arken.md`. This documents the token foundation built in
Phase 1: where values live, how to consume them, and how to extend them.

## Where tokens live

- `src/arken/arkentheme.h` / `arkentheme.cpp` — `ArkenTheme`, the canonical
  token owner. Singleton `QObject`, also exposed to QML as a singleton in the
  existing `org.kde.kdenlive` module (same registration pattern as `UiUtils`).
- `src/arken/arkenstyle.h` / `arkenstyle.cpp` — `ArkenStyle` namespace, the
  QWidget bridge. Translates tokens into `QPalette` / `QFont` / item metrics.
  Contains no application behavior by design.
- `src/arken/CMakeLists.txt` — appends the two `.cpp` files to `kdenlive_SRCS`
  (same pattern as `src/utils/CMakeLists.txt`), wired via
  `add_subdirectory(arken)` in `src/CMakeLists.txt`.
- `tests/arkenthemetest.cpp` — Catch2 coverage for token validity, scale
  ordering, theme switching/notification, and the QWidget bridge.

Responsibility split: `ArkenTheme` describes visual values, `ArkenStyle`
exposes them to QWidget, components consume. New visual values belong in
`ArkenTheme`; component logic belongs in components.

## How QML consumes the system

```qml
import org.kde.kdenlive as K

Rectangle {
    color: K.ArkenTheme.surface
    border.color: K.ArkenTheme.border
    radius: K.ArkenTheme.radiusSm
}
Text {
    color: K.ArkenTheme.textSecondary
    font: K.ArkenTheme.secondaryFont
}
Item {
    property int pad: K.ArkenTheme.spaceMd
    property int fieldHeight: K.ArkenTheme.controlHeightNormal
}
```

Notes:

- Color properties notify on `themeChanged`, so bindings re-resolve
  automatically when `ArkenTheme.setTheme()` is called. Spacing, type, shape
  and control metrics are theme-independent constants.
- Role-based font lookup: `K.ArkenTheme.fontFor(K.ArkenTheme.TextRole.Body)`.
- State styling: `K.ArkenTheme.hoverColor(base)` /
  `K.ArkenTheme.pressedColor(base)` derive hover/pressed variants. Full mapping:
  normal = token, hover = `hoverColor`, pressed = `pressedColor`,
  selected = `selection`, checked = `accent`, focused = `focus`,
  disabled = `textMuted`, destructive = `destructive`.
- Do not add new QML theme singletons or per-file color literals. If a value
  is missing, add a token to `ArkenTheme` so every surface shares it.

## How QWidget/C++ consumes the system

```cpp
#include "arken/arkenstyle.h"
#include "arkentheme.h" // only needed for direct token reads

ArkenStyle::polishPanel(widget);        // palette + body font
ArkenStyle::polishListView(listView);   // + item spacing + alternating rows

// Direct reads for custom painting / delegates:
QColor border = ArkenTheme::instance()->border();
int pad = ArkenTheme::instance()->spaceMd();
QFont heading = ArkenTheme::instance()->headingFont();
```

`ArkenStyle::panelPalette()` maps tokens onto standard `QPalette` roles
(`Window`→app background, `Base`→surface, `Highlight`→selection,
disabled roles→muted text, …). Prefer it over stylesheets; stylesheets remain
acceptable only for things palettes cannot express (e.g. focus rings, radii).

## How future themes are added

1. Add an enumerator to `ArkenTheme::Theme` (e.g. `Graphite`).
2. Add a `case` in `ArkenTheme::paletteFor()` returning the 16 colors.
3. Extend `tests/arkenthemetest.cpp` with a switching assertion.

No component changes are needed: colors are the only theme-dependent tokens
and all propagate through the existing `themeChanged` notification. There is
intentionally no theme-selection settings UI yet (out of scope for Phase 1);
call `ArkenTheme::instance()->setTheme(...)` from future wiring.

## What of the old styling system remains

Untouched and still authoritative outside Arken-adopted surfaces:

- `SystemPalette` + `K.UiUtils` (fixed/smallest-readable fonts, base-size
  scalar) in timeline/monitor QML.
- `KColorScheme` / `QPalette` derivation at existing call sites
  (`timelinecontroller.cpp`, param widgets, dialogs).
- `KdenliveSettings` color entries (timeline clip colors, monitor background).
  These stay as user overrides; they are not the token source.
- `QIcon::fromTheme` everywhere; only brand icons bundled in `data/icons/`.
- Breeze integration via `KColorSchemeManager` (`dialogs/splash.cpp`).

Migration rule: surface by surface. When a surface is redesigned, rewire its
style reads to `ArkenTheme`/`ArkenStyle` and remove the local hard-coded
values. Never a flag-day rewrite.

## Proof-of-concept surface (Phase 1)

Undo History dock (`QUndoView` in `src/mainwindow.cpp`): a single
`ArkenStyle::polishListView(m_undoView)` call applies the shared palette,
body font, item spacing and alternating rows. No model, undo, action-ID or
behavior change — purely token consumption in real application code.

## Shell pilot (Phase 1B)

`src/arken/arkenshell.h` / `arkenshell.cpp` (`ArkenShell` namespace) holds the
reusable shell primitives:

- `shellStyleSheet()` — application-scoped QSS generated entirely from tokens.
  Narrow selectors only: `QMenuBar`, `QMenu`, `QToolBar` (+ toolbar buttons,
  separators), `QStatusBar`, `QToolTip`. No `QWidget`/view selectors, so inner
  panels keep existing styling until their own phases.
- `styleMainWindow(QMainWindow *)` — Arken palette via `ArkenStyle`,
  body font on the menu bar, caption font on the status bar.
- `styleToolBar(QToolBar *)` — body font + token spacing; visuals come from
  the sheet. Button style, icon size and behavior untouched.

Wiring: `MainWindow::finishUiSetup()` (`src/mainwindow.cpp`) applies all of
the above once the full workspace exists. KDDockWidgets chrome is styled at
construction in `src/kddocksetup.cpp` (title-bar palette/section font, tab-bar
palette/body font, separator divider/accent tokens) because those widgets
self-paint and answer better to palette than to sheets.

User-facing identity: `app.setApplicationDisplayName("ArkenV")` in
`src/main.cpp` (window titles, About dialog) and the splash title in
`src/dialogs/Splash.qml`. Internal identifiers (`org.kde.kdenlive`, component
name, config paths, action IDs) deliberately unchanged.

Known limitation: Qt style sheets do not support `outline`, so keyboard-focus
rings on toolbar buttons currently rely on hover/checked states only. A
delegate-painted focus indicator is deferred to a later phase.
