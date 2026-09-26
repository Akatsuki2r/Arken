/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "catch.hpp"
#include "test_utils.hpp"
// test specific headers
#include "arken/arkenshell.h"
#include "arken/arkentheme.h"
#include "arken/arkenstyle.h"

#include <QLayout>
#include <QListView>
#include <QMainWindow>
#include <QMenuBar>
#include <QMetaProperty>
#include <QStatusBar>
#include <QToolBar>

namespace {
QStringList expectedColorProperties()
{
    return {QStringLiteral("appBackground"), QStringLiteral("surface"), QStringLiteral("surfaceElevated"), QStringLiteral("surfaceSecondary"),
            QStringLiteral("border"), QStringLiteral("divider"), QStringLiteral("textPrimary"), QStringLiteral("textSecondary"),
            QStringLiteral("textMuted"), QStringLiteral("accent"), QStringLiteral("accentHover"), QStringLiteral("selection"),
            QStringLiteral("focus"), QStringLiteral("success"), QStringLiteral("warning"), QStringLiteral("destructive")};
}
} // namespace

TEST_CASE("ArkenTheme provides a valid default dark theme", "[ArkenTheme]")
{
    ArkenTheme *theme = ArkenTheme::instance();
    REQUIRE(theme != nullptr);
    REQUIRE(theme->theme() == ArkenTheme::Theme::Dark);

    SECTION("All color tokens are valid colors")
    {
        const QStringList colors = expectedColorProperties();
        for (const QString &name : colors) {
            const int index = theme->metaObject()->indexOfProperty(name.toUtf8().constData());
            REQUIRE(index != -1);
            const QVariant value = theme->metaObject()->property(index).read(theme);
            REQUIRE(value.typeId() == QMetaType::QColor);
            REQUIRE(value.value<QColor>().isValid());
        }
    }

    SECTION("Default theme offers readable text on the app background")
    {
        // Sanity check on hierarchy: primary text must be clearly lighter than
        // the background in the dark-first default theme.
        REQUIRE(theme->textPrimary().lightnessF() > theme->appBackground().lightnessF() + 0.4);
        REQUIRE(theme->textSecondary().lightnessF() > theme->textMuted().lightnessF());
    }

    SECTION("QML binding contract: every token is exposed as a meta property")
    {
        // QML consumes tokens through the meta-object system, so each token
        // name below must resolve to a readable property of the right type.
        const QMetaObject *meta = theme->metaObject();
        for (const QString &name : expectedColorProperties()) {
            const int index = meta->indexOfProperty(name.toUtf8().constData());
            REQUIRE(index != -1);
            REQUIRE(meta->property(index).isReadable());
        }
        const QStringList metrics = {QStringLiteral("spaceSm"), QStringLiteral("radiusSm"), QStringLiteral("controlHeightNormal"),
                                     QStringLiteral("displayFont"), QStringLiteral("bodyFont"), QStringLiteral("captionFont"),
                                     QStringLiteral("monospaceFont"), QStringLiteral("theme")};
        for (const QString &name : metrics) {
            REQUIRE(meta->indexOfProperty(name.toUtf8().constData()) != -1);
        }
    }
}

TEST_CASE("ArkenTheme spacing, shape and control scales are coherent", "[ArkenTheme]")
{
    ArkenTheme *theme = ArkenTheme::instance();

    SECTION("Spacing scale is strictly increasing")
    {
        REQUIRE(theme->spaceXxs() < theme->spaceXs());
        REQUIRE(theme->spaceXs() < theme->spaceSm());
        REQUIRE(theme->spaceSm() < theme->spaceMd());
        REQUIRE(theme->spaceMd() < theme->spaceLg());
        REQUIRE(theme->spaceLg() < theme->spaceXl());
        REQUIRE(theme->spaceXl() < theme->spaceXxl());
    }

    SECTION("Radii stay restrained")
    {
        REQUIRE(theme->radiusNone() == 0);
        REQUIRE(theme->radiusNone() <= theme->radiusXs());
        REQUIRE(theme->radiusXs() <= theme->radiusSm());
        REQUIRE(theme->radiusSm() <= theme->radiusMd());
        REQUIRE(theme->radiusMd() <= 8);
    }

    SECTION("Control heights are ordered and positive")
    {
        REQUIRE(theme->controlHeightCompact() > 0);
        REQUIRE(theme->controlHeightCompact() <= theme->controlHeightNormal());
        REQUIRE(theme->controlHeightNormal() <= theme->controlHeightLarge());
        REQUIRE(theme->iconButtonSize() > 0);
        REQUIRE(theme->toolbarControlHeight() > 0);
        REQUIRE(theme->inputHeight() > 0);
    }

    SECTION("Type hierarchy is ordered")
    {
        REQUIRE(theme->displayFont().pointSizeF() > theme->bodyFont().pointSizeF());
        REQUIRE(theme->bodyFont().pointSizeF() >= theme->captionFont().pointSizeF());
        REQUIRE(theme->displayFont().weight() >= theme->bodyFont().weight());
        REQUIRE(!theme->monospaceFont().family().isEmpty());
        REQUIRE(theme->fontFor(ArkenTheme::TextRole::Heading).pointSizeF() == theme->headingFont().pointSizeF());
    }

    SECTION("State helpers derive from the base color")
    {
        const QColor base = theme->surfaceElevated();
        REQUIRE(theme->hoverColor(base) != base);
        REQUIRE(theme->pressedColor(base) != base);
        REQUIRE(theme->hoverColor(base) != theme->pressedColor(base));
    }
}

TEST_CASE("ArkenTheme supports multiple themes", "[ArkenTheme]")
{
    ArkenTheme *theme = ArkenTheme::instance();

    SECTION("Switching theme swaps token values and notifies")
    {
        const QColor darkBackground = theme->appBackground();
        int notifications = 0;
        QMetaObject::Connection conn =
            QObject::connect(theme, &ArkenTheme::themeChanged, [&notifications]() { ++notifications; });
        theme->setTheme(ArkenTheme::Theme::Light);
        REQUIRE(theme->theme() == ArkenTheme::Theme::Light);
        REQUIRE(notifications == 1);
        REQUIRE(theme->appBackground() != darkBackground);
        // Light theme: dark text on a light background.
        REQUIRE(theme->textPrimary().lightnessF() < theme->appBackground().lightnessF() - 0.4);
        theme->setTheme(ArkenTheme::Theme::HighContrast);
        REQUIRE(theme->appBackground().lightnessF() < 0.1);
        theme->setTheme(ArkenTheme::Theme::Dark);
        REQUIRE(theme->appBackground() == darkBackground);
        QObject::disconnect(conn);
    }

    SECTION("Setting the same theme does not notify")
    {
        theme->setTheme(ArkenTheme::Theme::Dark);
        int notifications = 0;
        QMetaObject::Connection conn =
            QObject::connect(theme, &ArkenTheme::themeChanged, [&notifications]() { ++notifications; });
        theme->setTheme(ArkenTheme::Theme::Dark);
        REQUIRE(notifications == 0);
        QObject::disconnect(conn);
    }
}

TEST_CASE("ArkenStyle bridges tokens to QWidget", "[ArkenStyle]")
{
    ArkenTheme::instance()->setTheme(ArkenTheme::Theme::Dark);

    SECTION("Panel palette maps tokens onto palette roles")
    {
        const ArkenTheme *theme = ArkenTheme::instance();
        const QPalette palette = ArkenStyle::panelPalette();
        REQUIRE(palette.color(QPalette::Window) == theme->appBackground());
        REQUIRE(palette.color(QPalette::Base) == theme->surface());
        REQUIRE(palette.color(QPalette::Text) == theme->textPrimary());
        REQUIRE(palette.color(QPalette::Highlight) == theme->selection());
        REQUIRE(palette.color(QPalette::Disabled, QPalette::Text) == theme->textMuted());
    }

    SECTION("Polishing a list view applies tokens without changing behavior")
    {
        QListView view;
        const QItemSelectionModel *selectionBefore = view.selectionModel();
        ArkenStyle::polishListView(&view);
        REQUIRE(view.palette().color(QPalette::Base) == ArkenTheme::instance()->surface());
        REQUIRE(view.font().pointSizeF() == ArkenTheme::instance()->bodyFont().pointSizeF());
        REQUIRE(view.spacing() == ArkenTheme::instance()->spaceSm());
        REQUIRE(view.alternatingRowColors());
        // No selection model installed by polishing: interaction behavior untouched.
        REQUIRE(view.selectionModel() == selectionBefore);
    }
}

TEST_CASE("ArkenShell stylesheet is generated from tokens", "[ArkenShell]")
{
    ArkenTheme::instance()->setTheme(ArkenTheme::Theme::Dark);
    const ArkenTheme *theme = ArkenTheme::instance();
    const QString sheet = ArkenShell::shellStyleSheet();

    SECTION("Sheet is non-trivial and token-driven")
    {
        REQUIRE(!sheet.isEmpty());
        // Every structural color must come from the active theme, not literals.
        REQUIRE(sheet.contains(theme->appBackground().name()));
        REQUIRE(sheet.contains(theme->surface().name()));
        REQUIRE(sheet.contains(theme->surfaceElevated().name()));
        REQUIRE(sheet.contains(theme->border().name()));
        REQUIRE(sheet.contains(theme->divider().name()));
        REQUIRE(sheet.contains(theme->textPrimary().name()));
        REQUIRE(sheet.contains(theme->textMuted().name()));
        REQUIRE(sheet.contains(theme->accent().name()));
        REQUIRE(sheet.contains(theme->selection().name()));
    }

    SECTION("Sheet only targets shell chrome selectors")
    {
        REQUIRE(sheet.contains(QStringLiteral("QMenuBar")));
        REQUIRE(sheet.contains(QStringLiteral("QMenu")));
        REQUIRE(sheet.contains(QStringLiteral("QToolBar")));
        REQUIRE(sheet.contains(QStringLiteral("QStatusBar")));
        REQUIRE(sheet.contains(QStringLiteral("QToolTip")));
        // Inner editing surfaces must not be restyled by the shell sheet.
        REQUIRE(!sheet.contains(QStringLiteral("QTreeView")));
        REQUIRE(!sheet.contains(QStringLiteral("QListView")));
        REQUIRE(!sheet.contains(QStringLiteral("QWidget")));
    }

    SECTION("Switching theme regenerates the sheet from the new tokens")
    {
        ArkenTheme::instance()->setTheme(ArkenTheme::Theme::Light);
        const QString lightSheet = ArkenShell::shellStyleSheet();
        REQUIRE(lightSheet.contains(ArkenTheme::instance()->appBackground().name()));
        REQUIRE(lightSheet != sheet);
        ArkenTheme::instance()->setTheme(ArkenTheme::Theme::Dark);
    }
}

TEST_CASE("ArkenShell styles shell widgets from tokens", "[ArkenShell]")
{
    ArkenTheme::instance()->setTheme(ArkenTheme::Theme::Dark);
    const ArkenTheme *theme = ArkenTheme::instance();

    SECTION("Main window gets the Arken palette and chrome fonts")
    {
        QMainWindow window;
        ArkenShell::styleMainWindow(&window);
        REQUIRE(window.palette().color(QPalette::Window) == theme->appBackground());
        REQUIRE(window.font().pointSizeF() == theme->bodyFont().pointSizeF());
        REQUIRE(window.menuBar()->font().pointSizeF() == theme->bodyFont().pointSizeF());
        REQUIRE(window.statusBar()->font().pointSizeF() == theme->captionFont().pointSizeF());
    }

    SECTION("Toolbar gets Arken spacing without behavior change")
    {
        QToolBar bar;
        const Qt::ToolButtonStyle styleBefore = bar.toolButtonStyle();
        ArkenShell::styleToolBar(&bar);
        REQUIRE(bar.font().pointSizeF() == theme->bodyFont().pointSizeF());
        REQUIRE(bar.layout()->spacing() == theme->spaceXs());
        REQUIRE(bar.toolButtonStyle() == styleBefore);
    }

    SECTION("Null widgets are ignored")
    {
        ArkenShell::styleMainWindow(nullptr);
        ArkenShell::styleToolBar(nullptr);
    }
}
