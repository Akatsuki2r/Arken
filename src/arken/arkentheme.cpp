/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "arkentheme.h"

#include <QFontDatabase>
#include <QGuiApplication>

namespace {
// Mix @param mixAmount (0.0 - 1.0) of @param towards with @param base.
QColor mixTowards(const QColor &base, const QColor &towards, double mixAmount)
{
    const double inv = 1.0 - mixAmount;
    return QColor::fromRgbF(base.redF() * inv + towards.redF() * mixAmount, base.greenF() * inv + towards.greenF() * mixAmount,
                            base.blueF() * inv + towards.blueF() * mixAmount, base.alphaF());
}
} // namespace

ArkenTheme *ArkenTheme::instance()
{
    static ArkenTheme *instance;
    if (!instance) {
        instance = new ArkenTheme;
    }
    return instance;
}

ArkenTheme *ArkenTheme::create(QQmlEngine *, QJSEngine *)
{
    return instance();
}

ArkenTheme::ArkenTheme(QObject *parent)
    : QObject(parent)
{
}

ArkenTheme::Theme ArkenTheme::theme() const
{
    return m_theme;
}

void ArkenTheme::setTheme(Theme theme)
{
    if (m_theme == theme) {
        return;
    }
    m_theme = theme;
    Q_EMIT themeChanged();
}

ArkenTheme::Palette ArkenTheme::paletteFor(Theme theme)
{
    switch (theme) {
    case Theme::Light:
        return {
            QColor(0xED, 0xEE, 0xF0), // appBackground
            QColor(0xF6, 0xF7, 0xF8), // surface
            QColor(0xFF, 0xFF, 0xFF), // surfaceElevated
            QColor(0xE4, 0xE6, 0xE9), // surfaceSecondary
            QColor(0xC9, 0xCD, 0xD2), // border
            QColor(0xDD, 0xE0, 0xE4), // divider
            QColor(0x23, 0x26, 0x2B), // textPrimary
            QColor(0x4D, 0x54, 0x5C), // textSecondary
            QColor(0x8A, 0x91, 0x99), // textMuted
            QColor(0x2F, 0x6F, 0xBD), // accent
            QColor(0x3A, 0x7F, 0xCE), // accentHover
            QColor(0xC7, 0xDD, 0xF5), // selection
            QColor(0x2F, 0x6F, 0xBD), // focus
            QColor(0x2E, 0x7D, 0x4F), // success
            QColor(0x9A, 0x6B, 0x1A), // warning
            QColor(0xB8, 0x3A, 0x3A), // destructive
        };
    case Theme::HighContrast:
        return {
            QColor(0x00, 0x00, 0x00), // appBackground
            QColor(0x0A, 0x0A, 0x0A), // surface
            QColor(0x14, 0x14, 0x14), // surfaceElevated
            QColor(0x10, 0x10, 0x10), // surfaceSecondary
            QColor(0x9A, 0x9A, 0x9A), // border
            QColor(0x5A, 0x5A, 0x5A), // divider
            QColor(0xFF, 0xFF, 0xFF), // textPrimary
            QColor(0xE8, 0xE8, 0xE8), // textSecondary
            QColor(0xC0, 0xC0, 0xC0), // textMuted
            QColor(0x66, 0xA3, 0xFF), // accent
            QColor(0x88, 0xBB, 0xFF), // accentHover
            QColor(0x00, 0x5A, 0x9C), // selection
            QColor(0x66, 0xA3, 0xFF), // focus
            QColor(0x3D, 0xDC, 0x84), // success
            QColor(0xFF, 0xC5, 0x3D), // warning
            QColor(0xFF, 0x6B, 0x6B), // destructive
        };
    case Theme::Dark:
    default:
        // Default Arken theme (dark-first).
        return {
            QColor(0x1B, 0x1D, 0x20), // appBackground
            QColor(0x23, 0x25, 0x29), // surface
            QColor(0x2B, 0x2E, 0x33), // surfaceElevated
            QColor(0x20, 0x22, 0x26), // surfaceSecondary
            QColor(0x35, 0x38, 0x3E), // border
            QColor(0x2C, 0x2F, 0x34), // divider
            QColor(0xE8, 0xE9, 0xEB), // textPrimary
            QColor(0xB4, 0xB8, 0xBE), // textSecondary
            QColor(0x7D, 0x83, 0x8C), // textMuted
            QColor(0x3E, 0x85, 0xD6), // accent
            QColor(0x54, 0x95, 0xDE), // accentHover
            QColor(0x35, 0x61, 0x8E), // selection
            QColor(0x3E, 0x85, 0xD6), // focus
            QColor(0x5C, 0xB8, 0x5C), // success
            QColor(0xD9, 0xA4, 0x41), // warning
            QColor(0xD0, 0x53, 0x53), // destructive
        };
    }
}

ArkenTheme::Palette ArkenTheme::currentPalette() const
{
    return paletteFor(m_theme);
}

QColor ArkenTheme::appBackground() const
{
    return currentPalette().appBackground;
}
QColor ArkenTheme::surface() const
{
    return currentPalette().surface;
}
QColor ArkenTheme::surfaceElevated() const
{
    return currentPalette().surfaceElevated;
}
QColor ArkenTheme::surfaceSecondary() const
{
    return currentPalette().surfaceSecondary;
}
QColor ArkenTheme::border() const
{
    return currentPalette().border;
}
QColor ArkenTheme::divider() const
{
    return currentPalette().divider;
}
QColor ArkenTheme::textPrimary() const
{
    return currentPalette().textPrimary;
}
QColor ArkenTheme::textSecondary() const
{
    return currentPalette().textSecondary;
}
QColor ArkenTheme::textMuted() const
{
    return currentPalette().textMuted;
}
QColor ArkenTheme::accent() const
{
    return currentPalette().accent;
}
QColor ArkenTheme::accentHover() const
{
    return currentPalette().accentHover;
}
QColor ArkenTheme::selection() const
{
    return currentPalette().selection;
}
QColor ArkenTheme::focus() const
{
    return currentPalette().focus;
}
QColor ArkenTheme::success() const
{
    return currentPalette().success;
}
QColor ArkenTheme::warning() const
{
    return currentPalette().warning;
}
QColor ArkenTheme::destructive() const
{
    return currentPalette().destructive;
}

int ArkenTheme::spaceXxs() const
{
    return 2;
}
int ArkenTheme::spaceXs() const
{
    return 4;
}
int ArkenTheme::spaceSm() const
{
    return 8;
}
int ArkenTheme::spaceMd() const
{
    return 12;
}
int ArkenTheme::spaceLg() const
{
    return 16;
}
int ArkenTheme::spaceXl() const
{
    return 24;
}
int ArkenTheme::spaceXxl() const
{
    return 32;
}

namespace {
QFont arkenBaseFont()
{
    if (QGuiApplication::instance() != nullptr) {
        return QGuiApplication::font();
    }
    return QFont();
}

QFont arkenScaledFont(double pointSizeDelta, QFont::Weight weight)
{
    QFont font = arkenBaseFont();
    const double baseSize = font.pointSizeF() > 0.0 ? font.pointSizeF() : 9.0;
    font.setPointSizeF(qMax(7.0, baseSize + pointSizeDelta));
    font.setWeight(weight);
    return font;
}
} // namespace

QFont ArkenTheme::displayFont() const
{
    return arkenScaledFont(6.0, QFont::Bold);
}
QFont ArkenTheme::headingFont() const
{
    return arkenScaledFont(3.0, QFont::Bold);
}
QFont ArkenTheme::sectionFont() const
{
    return arkenScaledFont(1.0, QFont::DemiBold);
}
QFont ArkenTheme::bodyFont() const
{
    return arkenScaledFont(0.0, QFont::Normal);
}
QFont ArkenTheme::secondaryFont() const
{
    return arkenScaledFont(0.0, QFont::Normal);
}
QFont ArkenTheme::captionFont() const
{
    return arkenScaledFont(-1.0, QFont::Normal);
}
QFont ArkenTheme::monospaceFont() const
{
    return QFontDatabase::systemFont(QFontDatabase::FixedFont);
}

QFont ArkenTheme::fontFor(TextRole role) const
{
    switch (role) {
    case TextRole::Display:
        return displayFont();
    case TextRole::Heading:
        return headingFont();
    case TextRole::Section:
        return sectionFont();
    case TextRole::Secondary:
        return secondaryFont();
    case TextRole::Caption:
        return captionFont();
    case TextRole::Monospace:
        return monospaceFont();
    case TextRole::Body:
    default:
        return bodyFont();
    }
}

int ArkenTheme::radiusNone() const
{
    return 0;
}
int ArkenTheme::radiusXs() const
{
    return 2;
}
int ArkenTheme::radiusSm() const
{
    return 4;
}
int ArkenTheme::radiusMd() const
{
    return 6;
}

int ArkenTheme::controlHeightCompact() const
{
    return 22;
}
int ArkenTheme::controlHeightNormal() const
{
    return 28;
}
int ArkenTheme::controlHeightLarge() const
{
    return 34;
}
int ArkenTheme::iconButtonSize() const
{
    return 28;
}
int ArkenTheme::toolbarControlHeight() const
{
    return 32;
}
int ArkenTheme::inputHeight() const
{
    return 28;
}

QColor ArkenTheme::hoverColor(const QColor &base) const
{
    return mixTowards(base, textPrimary(), 0.12);
}

QColor ArkenTheme::pressedColor(const QColor &base) const
{
    return mixTowards(base, appBackground(), 0.18);
}
