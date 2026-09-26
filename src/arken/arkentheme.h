/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QColor>
#include <QFont>
#include <QObject>
#include <QQmlEngine>
#include <QtQmlIntegration>

/** @class ArkenTheme
 *  @brief Canonical source of Arken design tokens (colors, spacing, type, shape, control sizes).
 *
 *  This is the single place where Arken's visual values are defined. Individual
 *  components must consume these tokens instead of hard-coding colors, spacing
 *  or control dimensions.
 *
 *  Colors are theme-dependent and notify on themeChanged, so QML bindings and
 *  QWidget code re-resolve automatically when setTheme() is called. Spacing,
 *  typography, shape and control metrics are theme-independent constants.
 *
 *  QML consumption (same registration pattern as UiUtils):
 *  @code
 *  import org.kde.kdenlive as K
 *  Rectangle { color: K.ArkenTheme.surface; radius: K.ArkenTheme.radiusSm }
 *  @endcode
 *
 *  QWidget consumption goes through ArkenStyle (arkenstyle.h), which translates
 *  these tokens into QPalette / QFont / layout metrics.
 */
class ArkenTheme : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

    Q_PROPERTY(QColor appBackground READ appBackground NOTIFY themeChanged)
    Q_PROPERTY(QColor surface READ surface NOTIFY themeChanged)
    Q_PROPERTY(QColor surfaceElevated READ surfaceElevated NOTIFY themeChanged)
    Q_PROPERTY(QColor surfaceSecondary READ surfaceSecondary NOTIFY themeChanged)
    Q_PROPERTY(QColor border READ border NOTIFY themeChanged)
    Q_PROPERTY(QColor divider READ divider NOTIFY themeChanged)
    Q_PROPERTY(QColor textPrimary READ textPrimary NOTIFY themeChanged)
    Q_PROPERTY(QColor textSecondary READ textSecondary NOTIFY themeChanged)
    Q_PROPERTY(QColor textMuted READ textMuted NOTIFY themeChanged)
    Q_PROPERTY(QColor accent READ accent NOTIFY themeChanged)
    Q_PROPERTY(QColor accentHover READ accentHover NOTIFY themeChanged)
    Q_PROPERTY(QColor selection READ selection NOTIFY themeChanged)
    Q_PROPERTY(QColor focus READ focus NOTIFY themeChanged)
    Q_PROPERTY(QColor success READ success NOTIFY themeChanged)
    Q_PROPERTY(QColor warning READ warning NOTIFY themeChanged)
    Q_PROPERTY(QColor destructive READ destructive NOTIFY themeChanged)

    Q_PROPERTY(int spaceXxs READ spaceXxs CONSTANT)
    Q_PROPERTY(int spaceXs READ spaceXs CONSTANT)
    Q_PROPERTY(int spaceSm READ spaceSm CONSTANT)
    Q_PROPERTY(int spaceMd READ spaceMd CONSTANT)
    Q_PROPERTY(int spaceLg READ spaceLg CONSTANT)
    Q_PROPERTY(int spaceXl READ spaceXl CONSTANT)
    Q_PROPERTY(int spaceXxl READ spaceXxl CONSTANT)

    Q_PROPERTY(QFont displayFont READ displayFont CONSTANT)
    Q_PROPERTY(QFont headingFont READ headingFont CONSTANT)
    Q_PROPERTY(QFont sectionFont READ sectionFont CONSTANT)
    Q_PROPERTY(QFont bodyFont READ bodyFont CONSTANT)
    Q_PROPERTY(QFont secondaryFont READ secondaryFont CONSTANT)
    Q_PROPERTY(QFont captionFont READ captionFont CONSTANT)
    Q_PROPERTY(QFont monospaceFont READ monospaceFont CONSTANT)

    Q_PROPERTY(int radiusNone READ radiusNone CONSTANT)
    Q_PROPERTY(int radiusXs READ radiusXs CONSTANT)
    Q_PROPERTY(int radiusSm READ radiusSm CONSTANT)
    Q_PROPERTY(int radiusMd READ radiusMd CONSTANT)

    Q_PROPERTY(int controlHeightCompact READ controlHeightCompact CONSTANT)
    Q_PROPERTY(int controlHeightNormal READ controlHeightNormal CONSTANT)
    Q_PROPERTY(int controlHeightLarge READ controlHeightLarge CONSTANT)
    Q_PROPERTY(int iconButtonSize READ iconButtonSize CONSTANT)
    Q_PROPERTY(int toolbarControlHeight READ toolbarControlHeight CONSTANT)
    Q_PROPERTY(int inputHeight READ inputHeight CONSTANT)

public:
    /** @brief Available Arken themes. Dark is the default (dark-first product direction). */
    enum class Theme {
        Dark = 0,
        Light = 1,
        HighContrast = 2,
    };
    Q_ENUM(Theme)

    /** @brief Typographic roles for role-based font lookup via fontFor(). */
    enum class TextRole {
        Display = 0,
        Heading = 1,
        Section = 2,
        Body = 3,
        Secondary = 4,
        Caption = 5,
        Monospace = 6,
    };
    Q_ENUM(TextRole)

    static ArkenTheme *instance();
    static ArkenTheme *create(QQmlEngine *, QJSEngine *);

    Theme theme() const;
    void setTheme(Theme theme);

    // Colors (theme-dependent)
    QColor appBackground() const;
    QColor surface() const;
    QColor surfaceElevated() const;
    QColor surfaceSecondary() const;
    QColor border() const;
    QColor divider() const;
    QColor textPrimary() const;
    QColor textSecondary() const;
    QColor textMuted() const;
    QColor accent() const;
    QColor accentHover() const;
    QColor selection() const;
    QColor focus() const;
    QColor success() const;
    QColor warning() const;
    QColor destructive() const;

    // Spacing scale (px, theme-independent)
    int spaceXxs() const;
    int spaceXs() const;
    int spaceSm() const;
    int spaceMd() const;
    int spaceLg() const;
    int spaceXl() const;
    int spaceXxl() const;

    // Typography (theme-independent)
    QFont displayFont() const;
    QFont headingFont() const;
    QFont sectionFont() const;
    QFont bodyFont() const;
    QFont secondaryFont() const;
    QFont captionFont() const;
    QFont monospaceFont() const;
    Q_INVOKABLE QFont fontFor(TextRole role) const;

    // Shape (px radii, theme-independent, deliberately restrained)
    int radiusNone() const;
    int radiusXs() const;
    int radiusSm() const;
    int radiusMd() const;

    // Control dimensions (px, theme-independent)
    int controlHeightCompact() const;
    int controlHeightNormal() const;
    int controlHeightLarge() const;
    int iconButtonSize() const;
    int toolbarControlHeight() const;
    int inputHeight() const;

    /** @brief Hover variant of a base color (mixed toward textPrimary). */
    Q_INVOKABLE QColor hoverColor(const QColor &base) const;
    /** @brief Pressed variant of a base color (mixed toward appBackground). */
    Q_INVOKABLE QColor pressedColor(const QColor &base) const;

Q_SIGNALS:
    void themeChanged();

private:
    explicit ArkenTheme(QObject *parent = nullptr);

    struct Palette {
        QColor appBackground;
        QColor surface;
        QColor surfaceElevated;
        QColor surfaceSecondary;
        QColor border;
        QColor divider;
        QColor textPrimary;
        QColor textSecondary;
        QColor textMuted;
        QColor accent;
        QColor accentHover;
        QColor selection;
        QColor focus;
        QColor success;
        QColor warning;
        QColor destructive;
    };
    static Palette paletteFor(Theme theme);
    Palette currentPalette() const;

    Theme m_theme = Theme::Dark;
};
