/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QFont>
#include <QPalette>

class QListView;
class QWidget;

/** @namespace ArkenStyle
 *  @brief QWidget bridge for the Arken design system.
 *
 *  ArkenTheme (arkentheme.h) owns the token values; this namespace only
 *  translates them into QWidget concepts (QPalette, QFont, item spacing).
 *  It contains no application behavior and must not grow any: new visual
 *  values belong in ArkenTheme, new component logic belongs in components.
 *
 *  Interaction-state mapping used throughout Arken QWidget surfaces:
 *  normal = token value, hover = ArkenTheme::hoverColor(base),
 *  pressed = ArkenTheme::pressedColor(base), selected = selection token,
 *  checked = accent token, focused = focus token (focus ring / highlight),
 *  disabled = textMuted token, destructive = destructive token.
 */
namespace ArkenStyle {

/** @brief Palette mapping Arken tokens onto standard QPalette roles. */
QPalette panelPalette();
/** @brief Default Arken body font for panels. */
QFont panelFont();
/** @brief Apply the Arken panel palette + body font to @param widget (non-recursive). */
void polishPanel(QWidget *widget);
/** @brief polishPanel() plus list-specific metrics (item spacing, alternating rows). */
void polishListView(QListView *view);

} // namespace ArkenStyle
