/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "arkenstyle.h"
#include "arkentheme.h"

#include <QListView>
#include <QWidget>

namespace ArkenStyle {

QPalette panelPalette()
{
    const ArkenTheme *theme = ArkenTheme::instance();
    QPalette palette;
    palette.setColor(QPalette::Window, theme->appBackground());
    palette.setColor(QPalette::WindowText, theme->textPrimary());
    palette.setColor(QPalette::Base, theme->surface());
    palette.setColor(QPalette::AlternateBase, theme->surfaceSecondary());
    palette.setColor(QPalette::Text, theme->textPrimary());
    palette.setColor(QPalette::Button, theme->surfaceElevated());
    palette.setColor(QPalette::ButtonText, theme->textPrimary());
    palette.setColor(QPalette::Highlight, theme->selection());
    palette.setColor(QPalette::HighlightedText, theme->textPrimary());
    palette.setColor(QPalette::Link, theme->accent());
    palette.setColor(QPalette::LinkVisited, theme->accent());
    palette.setColor(QPalette::ToolTipBase, theme->surfaceElevated());
    palette.setColor(QPalette::ToolTipText, theme->textPrimary());
    palette.setColor(QPalette::PlaceholderText, theme->textMuted());
    palette.setColor(QPalette::BrightText, theme->textPrimary());
    palette.setColor(QPalette::Disabled, QPalette::WindowText, theme->textMuted());
    palette.setColor(QPalette::Disabled, QPalette::Text, theme->textMuted());
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, theme->textMuted());
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, theme->textMuted());
    return palette;
}

QFont panelFont()
{
    return ArkenTheme::instance()->bodyFont();
}

void polishPanel(QWidget *widget)
{
    if (widget == nullptr) {
        return;
    }
    widget->setPalette(panelPalette());
    widget->setFont(panelFont());
}

void polishListView(QListView *view)
{
    if (view == nullptr) {
        return;
    }
    polishPanel(view);
    view->setSpacing(ArkenTheme::instance()->spaceSm());
    view->setAlternatingRowColors(true);
}

} // namespace ArkenStyle
