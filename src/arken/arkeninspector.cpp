/*
    SPDX-FileCopyrightText: 2026 ArkenV contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "arkeninspector.h"
#include "arkenstyle.h"
#include "arkentheme.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QToolButton>
#include <QIcon>
#include <QApplication>
#include <QFormLayout>

namespace Arken {

InspectorCategory::InspectorCategory(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Header button
    auto *header = new QToolButton(this);
    header->setText(title);
    header->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    header->setArrowType(Qt::DownArrow);
    header->setCheckable(true);
    header->setChecked(true);
    header->setAutoRaise(true);
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ArkenStyle::polishPanel(header);
    header->setFont(ArkenTheme::instance()->sectionFont());
    connect(header, &QToolButton::toggled, this, [this, header](bool checked) {
        m_expanded = checked;
        header->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
        if (m_contentWidget) m_contentWidget->setVisible(checked);
        Q_EMIT expansionChanged(checked);
        Q_EMIT contentHeightChanged();
    });
    lay->addWidget(header);

    // Content widget
    m_contentWidget = new QWidget(this);
    m_contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *contentLay = new QVBoxLayout(m_contentWidget);
    contentLay->setContentsMargins(ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm(),
                                   ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm());
    contentLay->setSpacing(ArkenTheme::instance()->spaceSm());
    lay->addWidget(m_contentWidget);
    lay->addStretch(1);
}

void InspectorCategory::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;
        if (m_contentWidget) m_contentWidget->setVisible(expanded);
        Q_EMIT expansionChanged(expanded);
        Q_EMIT contentHeightChanged();
    }
}

// --- InspectorTransformCategory ---
InspectorTransformCategory::InspectorTransformCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Transform"), parent)
    , m_layout(new QFormLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setVerticalSpacing(ArkenTheme::instance()->spaceSm());
    m_layout->setHorizontalSpacing(ArkenTheme::instance()->spaceMd());
    m_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void InspectorTransformCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Transform controls (position, scale, rotation, anchor)"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addRow(label);
}

void InspectorTransformCategory::clearModel()
{
    while (m_layout->rowCount() > 0) {
        m_layout->removeRow(0);
    }
}

bool InspectorTransformCategory::hasContent() const
{
    return m_layout->rowCount() > 0;
}

// --- InspectorVideoCategory ---
InspectorVideoCategory::InspectorVideoCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Video"), parent)
    , m_layout(new QFormLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setVerticalSpacing(ArkenTheme::instance()->spaceSm());
    m_layout->setHorizontalSpacing(ArkenTheme::instance()->spaceMd());
    m_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void InspectorVideoCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Video controls (opacity, blend mode, crop, color)"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addRow(label);
}

void InspectorVideoCategory::clearModel()
{
    while (m_layout->rowCount() > 0) {
        m_layout->removeRow(0);
    }
}

bool InspectorVideoCategory::hasContent() const
{
    return m_layout->rowCount() > 0;
}

// --- InspectorAudioCategory ---
InspectorAudioCategory::InspectorAudioCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Audio"), parent)
    , m_layout(new QFormLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setVerticalSpacing(ArkenTheme::instance()->spaceSm());
    m_layout->setHorizontalSpacing(ArkenTheme::instance()->spaceMd());
    m_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void InspectorAudioCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Audio controls (volume, pan, keyframes)"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addRow(label);
}

void InspectorAudioCategory::clearModel()
{
    while (m_layout->rowCount() > 0) {
        m_layout->removeRow(0);
    }
}

bool InspectorAudioCategory::hasContent() const
{
    return m_layout->rowCount() > 0;
}

// --- InspectorEffectsCategory ---
InspectorEffectsCategory::InspectorEffectsCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Effects"), parent)
    , m_layout(new QVBoxLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(ArkenTheme::instance()->spaceSm());
}

void InspectorEffectsCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Effects stack (add, remove, reorder, parameters)"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addWidget(label);

    auto *addBtn = new QPushButton(tr("Add Effect…"));
    addBtn->setFixedHeight(ArkenTheme::instance()->controlHeightNormal());
    ArkenStyle::polishPanel(addBtn);
    m_layout->addWidget(addBtn);
}

void InspectorEffectsCategory::clearModel()
{
    while (m_layout->count() > 0) {
        auto item = m_layout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }
}

bool InspectorEffectsCategory::hasContent() const
{
    return m_layout->count() > 0;
}

// --- InspectorAdvancedCategory ---
InspectorAdvancedCategory::InspectorAdvancedCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Advanced"), parent)
    , m_layout(new QFormLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setVerticalSpacing(ArkenTheme::instance()->spaceSm());
    m_layout->setHorizontalSpacing(ArkenTheme::instance()->spaceMd());
    m_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    setExpanded(false); // Collapsed by default
}

void InspectorAdvancedCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Advanced settings (field order, proxy, technical)"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addRow(label);
}

void InspectorAdvancedCategory::clearModel()
{
    while (m_layout->rowCount() > 0) {
        m_layout->removeRow(0);
    }
}

bool InspectorAdvancedCategory::hasContent() const
{
    return m_layout->rowCount() > 0;
}

// --- InspectorTransitionCategory ---
InspectorTransitionCategory::InspectorTransitionCategory(QWidget *parent)
    : InspectorCategory(QObject::tr("Transition"), parent)
    , m_layout(new QFormLayout(m_contentWidget))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setVerticalSpacing(ArkenTheme::instance()->spaceSm());
    m_layout->setHorizontalSpacing(ArkenTheme::instance()->spaceMd());
    m_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void InspectorTransitionCategory::setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &)
{
    auto *label = new QLabel(tr("Transition / Mix parameters"));
    label->setWordWrap(true);
    ArkenStyle::polishPanel(label);
    m_layout->addRow(label);
}

void InspectorTransitionCategory::clearModel()
{
    while (m_layout->rowCount() > 0) {
        m_layout->removeRow(0);
    }
}

bool InspectorTransitionCategory::hasContent() const
{
    return m_layout->rowCount() > 0;
}

// --- InspectorEmptyState ---
InspectorEmptyState::InspectorEmptyState(QWidget *parent)
    : QWidget(parent)
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(ArkenTheme::instance()->spaceXl(), ArkenTheme::instance()->spaceXl(),
                            ArkenTheme::instance()->spaceXl(), ArkenTheme::instance()->spaceXl());
    lay->setAlignment(Qt::AlignCenter);
    lay->setSpacing(ArkenTheme::instance()->spaceMd());

    auto *icon = new QLabel(this);
    icon->setPixmap(QIcon::fromTheme("kdenlive").pixmap(64, 64));
    icon->setAlignment(Qt::AlignCenter);
    lay->addWidget(icon);

    auto *title = new QLabel(tr("Inspector"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setFont(ArkenTheme::instance()->headingFont());
    ArkenStyle::polishPanel(title);
    lay->addWidget(title);

    auto *subtitle = new QLabel(tr("Select a clip, track, transition, or effect to edit its properties."), this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setFont(ArkenTheme::instance()->secondaryFont());
    auto palette = subtitle->palette();
    palette.setColor(QPalette::WindowText, ArkenTheme::instance()->textMuted());
    subtitle->setPalette(palette);
    lay->addWidget(subtitle);
}

} // namespace Arken