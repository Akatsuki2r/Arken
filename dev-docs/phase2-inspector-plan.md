# Phase 2: ArkenV Contextual Inspector — Implementation Plan

## Architecture Summary (verified)

### Selection → Inspector Flow
```
Timeline clip select  → TimelineController::showAsset() → showItemEffectStack → MainWindow → AssetPanel::showEffectStack
Timeline track select → TimelineController::showTrackAsset() → showItemEffectStack → AssetPanel::showEffectStack
Bin clip select       → Bin::openClipInMonitor() → Core::requestShowBinEffectStack → AssetPanel::showEffectStack
Core API              → Core::showEffectStackFromId() → routes to appropriate controller
```

### Current AssetPanel Structure
```
AssetPanel (QWidget)
├── Toolbar (title, composition switch, mask, split, enable, keyframes, save)
├── ScrollArea
│   └── Container (QVBoxLayout)
│       ├── TransitionStackView (hidden by default)
│       ├── MixStackView (hidden by default)
│       ├── EffectStackView (hidden by default)
│       └── MaskManager (hidden by default)
└── KMessageWidget (warnings)
```

### EffectStackView Structure
```
EffectStackView (QWidget)
├── Built-in widget (Flip H/V, Remove Background) - shown for video clips
└── QTreeView (with EffectStackFilter)
    └── CollapsibleEffectView per effect (index widget)
        └── AssetParameterView (QFormLayout)
            └── AbstractParamWidget subclasses per parameter type
```

### Models (stable, do not rewrite)
- `EffectStackModel` - stack of effects, undo/redo, active effect, enable/disable
- `AssetParameterModel` - parameters for one effect/transition/mix
- `KeyframeModelList` - keyframe data per animated parameter

---

## Phase 2 Implementation Strategy

### 1. New Inspector Widget (`ArkenInspector`)
Replace `AssetPanel` as the primary contextual inspector. Single widget that:
- Receives selection context via `Core` signals (existing + one new additive signal)
- Maintains internal state: current context type, current model
- Composes sub-views: `InspectorHeader`, `InspectorCategories`, `InspectorParameters`
- Uses `ArkenShell`/`ArkenStyle` tokens throughout

### 2. Context Types (enum)
```cpp
enum class InspectorContext {
    None,              // empty state
    TimelineClip,      // clip on timeline
    TimelineTrack,     // track header
    TimelineComposition, // transition/mix
    TimelineMaster,    // master effects
    BinClip,           // clip in project bin
};
```

### 3. Visual Hierarchy (Arken tokens)
```
ArkenInspector
├── InspectorHeader (object identity + type badge + primary actions)
├── InspectorCategories (collapsible sections)
│   ├── Transform (position, scale, rotation, anchor)
│   ├── Video (opacity, blend mode, crop)
│   ├── Audio (volume, pan, keyframes)
│   ├── Effects (list of applied effects, add button)
│   └── Advanced (technical: field order, proxy, etc.)
└── InspectorFooter (status, keyframe navigation)
```

### 4. Progressive Disclosure
- Categories collapsed by default except Transform/Video for clips
- "Show Advanced" toggle per category
- Effect parameters stay in `CollapsibleEffectView` but embedded in Effects category

### 5. Transform Coherence
- Extract built-in transform params (position, scale, rotation, anchor) from `EffectStackModel` built-in effects
- Present as unified "Transform" category at top
- Monitor geometry overlay still works (no MLT changes)

### 6. Keyframe Coherence
- `AssetParameterView` already has `KeyframeContainer` for animated params
- Add keyframe navigation (prev/next, add/remove) to category header when keyframes exist
- Timeline keyframe lane stays separate (Phase 3)

### 7. Integration Points (minimal plumbing)
- Add `Core::inspectorContextRequested(InspectorContext, ObjectId)` signal (new, additive)
- `MainWindow` connects to new `ArkenInspector` instead of `AssetPanel`
- `AssetPanel` kept for compatibility during transition, can be removed later
- Dock widget title/context updates via `Core` signal

---

## Files to Create/Modify

### New Files
1. `src/arken/arkeninspector.h/.cpp` - Main inspector widget
2. `src/arken/inspectorheader.h/.cpp` - Header with object identity
3. `src/arken/inspectorcategory.h/.cpp` - Collapsible category container
4. `src/arken/inspectortransform.h/.cpp` - Transform category (extracts from built-in effects)
5. `src/arken/inspectoreffects.h/.cpp` - Effects category (wraps EffectStackView)
6. `src/arken/inspectoraudio.h/.cpp` - Audio category
7. `src/arken/inspectorempty.h/.cpp` - Empty state widget
8. `src/arken/arkeninspectordock.h/.cpp` - Dock integration (optional, can reuse KDDockWidgets)

### Modified Files
1. `src/arken/CMakeLists.txt` - Add new sources
2. `src/core.h/.cpp` - Add `inspectorContextRequested` signal + handler
3. `src/mainwindow.cpp` - Wire new inspector, keep AssetPanel for transition
4. `src/timeline2/view/timelinecontroller.h/.cpp` - Emit new context signal (additive)
5. `src/bin/bin.cpp` - Emit new context signal for bin clips (additive)
6. `dev-docs/arken-design-system.md` - Document Inspector primitives

---

## Acceptance Criteria
- [ ] Select timeline clip → Inspector shows clip name, type, Transform/Video/Audio/Effects categories
- [ ] Change clip selection → Inspector updates immediately
- [ ] Select transition → Inspector shows transition parameters in context
- [ ] Select track → Inspector shows track effects
- [ ] Select bin clip → Inspector shows bin clip effects
- [ ] Modify effect parameter → Underlying edit works, undo/redo works
- [ ] Empty selection → Intentional empty state (not blank panel)
- [ ] Advanced controls accessible (progressive disclosure)
- [ ] All Arken tokens used, no hard-coded values
- [ ] Build passes, tests pass, runtime verified

---

## Risk Mitigation
| Risk | Mitigation |
|------|------------|
| EffectStackView tightly coupled to AssetPanel | Keep AssetPanel functional; new Inspector wraps/reuses EffectStackView |
| Built-in transform effects scattered | Extract at model level (EffectStackModel already has `hasBuiltInEffect`) |
| KeyframeContainer tied to AssetParameterView | Reuse existing KeyframeContainer, expose via category header |
| Multi-selection ambiguous | Preserve current single-selection behavior; document limitation |
| Dock integration | Reuse existing KDDockWidgets dock; just swap widget |