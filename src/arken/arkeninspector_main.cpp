/*
    SPDX-FileCopyrightText: 2026 ArkenV contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "arkeninspector_main.h"
#include "arkeninspector.h"
#include "arkenstyle.h"
#include "arkentheme.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QIcon>

namespace Arken {

// --- InspectorHeader ---
class InspectorHeader : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorHeader(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        auto *lay = new QHBoxLayout(this);
        lay->setContentsMargins(ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm(),
                                ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm());
        lay->setSpacing(ArkenTheme::instance()->spaceSm());

        m_icon = new QLabel(this);
        m_icon->setFixedSize(32, 32);
        m_icon->setAlignment(Qt::AlignCenter);
        lay->addWidget(m_icon);

        auto *textLay = new QVBoxLayout();
        textLay->setSpacing(2);
        textLay->setContentsMargins(0, 0, 0, 0);

        m_title = new QLabel(tr("No Selection"), this);
        m_title->setFont(ArkenTheme::instance()->headingFont());
        ArkenStyle::polishPanel(m_title);
        textLay->addWidget(m_title);

        m_subtitle = new QLabel(tr("Select an item to inspect"), this);
        m_subtitle->setFont(ArkenTheme::instance()->captionFont());
        auto palette = m_subtitle->palette();
        palette.setColor(QPalette::WindowText, ArkenTheme::instance()->textMuted());
        m_subtitle->setPalette(palette);
        textLay->addWidget(m_subtitle);

        lay->addLayout(textLay, 1);

        m_typeBadge = new QLabel(this);
        m_typeBadge->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_typeBadge->setFont(ArkenTheme::instance()->captionFont());
        m_typeBadge->setStyleSheet(QStringLiteral("QLabel { background: %1; color: %2; padding: 2px 6px; border-radius: %3px; }")
                                       .arg(ArkenTheme::instance()->accent().name())
                                       .arg(ArkenTheme::instance()->textPrimary().name())
                                       .arg(ArkenTheme::instance()->radiusSm()));
        m_typeBadge->setVisible(false);
        lay->addWidget(m_typeBadge);

        ArkenStyle::polishPanel(this);
    }

    void setContent(const QString &title, const QString &subtitle, const QString &type, const QIcon &icon)
    {
        m_title->setText(title);
        m_subtitle->setText(subtitle);
        if (!type.isEmpty()) {
            m_typeBadge->setText(type);
            m_typeBadge->setVisible(true);
        } else {
            m_typeBadge->setVisible(false);
        }
        if (!icon.isNull()) {
            m_icon->setPixmap(icon.pixmap(32, 32));
        }
    }

    void clear()
    {
        m_title->setText(tr("No Selection"));
        m_subtitle->setText(tr("Select an item to inspect"));
        m_typeBadge->setVisible(false);
        m_icon->clear();
    }

private:
    QLabel *m_icon = nullptr;
    QLabel *m_title = nullptr;
    QLabel *m_subtitle = nullptr;
    QLabel *m_typeBadge = nullptr;
};

// --- ArkenInspector implementation ---
ArkenInspector::ArkenInspector(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyTheme();
    clear(); // Start with empty state
}

ArkenInspector::~ArkenInspector() = default;

void ArkenInspector::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Header
    m_header = new InspectorHeader(this);
    m_mainLayout->addWidget(m_header);

    // Scroll area for categories
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ArkenStyle::polishPanel(m_scrollArea);

    m_scrollContent = new QWidget();
    m_categoriesLayout = new QVBoxLayout(m_scrollContent);
    m_categoriesLayout->setContentsMargins(ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm(),
                                           ArkenTheme::instance()->spaceMd(), ArkenTheme::instance()->spaceSm());
    m_categoriesLayout->setSpacing(ArkenTheme::instance()->spaceMd());
    m_categoriesLayout->addStretch(1);

    m_scrollArea->setWidget(m_scrollContent);
    m_mainLayout->addWidget(m_scrollArea, 1);

    // Create categories
    m_transformCategory = new InspectorTransformCategory(m_scrollContent);
    m_videoCategory = new InspectorVideoCategory(m_scrollContent);
    m_audioCategory = new InspectorAudioCategory(m_scrollContent);
    m_effectsCategory = new InspectorEffectsCategory(m_scrollContent);
    m_advancedCategory = new InspectorAdvancedCategory(m_scrollContent);
    m_transitionCategory = new InspectorTransitionCategory(m_scrollContent);

    // Empty state
    m_emptyState = new InspectorEmptyState(m_scrollContent);
    m_emptyState->setVisible(true);
    m_categoriesLayout->addWidget(m_emptyState);

    // Connect category signals
    connect(m_effectsCategory, &InspectorEffectsCategory::effectActivated, this, &ArkenInspector::effectActivated);
    connect(m_effectsCategory, &InspectorEffectsCategory::effectAdded, this, &ArkenInspector::addEffectRequested);
}

void ArkenInspector::applyTheme()
{
    QPalette palette = ArkenStyle::panelPalette();
    setPalette(palette);
    setFont(ArkenTheme::instance()->bodyFont());
    setAutoFillBackground(true);
}

void ArkenInspector::setContext(InspectorContext context, const ObjectId &owner,
                                std::shared_ptr<EffectStackModel> stackModel,
                                std::shared_ptr<AssetParameterModel> paramModel,
                                const QSize &frameSize,
                                bool showKeyframes)
{
    if (m_context == context && m_owner == owner && m_stackModel == stackModel) {
        return; // No change
    }

    m_context = context;
    m_owner = owner;
    m_stackModel = std::move(stackModel);
    m_paramModel = std::move(paramModel);
    m_frameSize = frameSize;
    m_showKeyframes = showKeyframes;

    showContext(context);
    updateHeader();
    updateCategories();

    Q_EMIT contextChanged(m_context, m_owner);
}

void ArkenInspector::clear()
{
    m_context = InspectorContext::None;
    m_owner = ObjectId();
    m_stackModel.reset();
    m_paramModel.reset();
    m_frameSize = QSize();
    m_showKeyframes = false;

    m_header->clear();
    m_emptyState->setVisible(true);

    // Hide all categories
    QList<InspectorCategory *> allCategories = {m_transformCategory, m_videoCategory, m_audioCategory,
                                                m_effectsCategory, m_advancedCategory, m_transitionCategory};
    for (auto *cat : allCategories) {
        if (cat) {
            cat->setVisible(false);
            cat->clearModel();
            m_categoriesLayout->removeWidget(cat);
        }
    }
    m_activeCategories.clear();
}

void ArkenInspector::showContext(InspectorContext context)
{
    m_emptyState->setVisible(context == InspectorContext::None);

    // Show/hide categories based on context
    bool showTransform = false, showVideo = false, showAudio = false, showEffects = false,
         showAdvanced = false, showTransition = false;

    switch (context) {
    case InspectorContext::TimelineClip:
    case InspectorContext::BinClip:
        showTransform = true;
        showVideo = true;
        showAudio = true;
        showEffects = true;
        showAdvanced = true;
        break;
    case InspectorContext::TimelineTrack:
        showEffects = true;
        showAdvanced = true;
        break;
    case InspectorContext::TimelineComposition:
        showTransition = true;
        break;
    case InspectorContext::TimelineMaster:
        showEffects = true;
        showAdvanced = true;
        break;
    default:
        break;
    }

    auto setCatVisible = [this](InspectorCategory *cat, bool visible) {
        if (!cat) return;
        if (visible) {
            if (!cat->isVisible()) {
                m_categoriesLayout->insertWidget(m_categoriesLayout->count() - 1, cat); // Before stretch
            }
            cat->setVisible(true);
            if (!m_activeCategories.contains(cat)) m_activeCategories.append(cat);
        } else {
            cat->setVisible(false);
            cat->clearModel();
            m_categoriesLayout->removeWidget(cat);
            m_activeCategories.removeAll(cat);
        }
    };

    setCatVisible(m_transformCategory, showTransform);
    setCatVisible(m_videoCategory, showVideo);
    setCatVisible(m_audioCategory, showAudio);
    setCatVisible(m_effectsCategory, showEffects);
    setCatVisible(m_advancedCategory, showAdvanced);
    setCatVisible(m_transitionCategory, showTransition);
}

void ArkenInspector::updateHeader()
{
    QString title, subtitle, type;
    QIcon icon;

    switch (m_context) {
    case InspectorContext::TimelineClip:
        title = tr("Timeline Clip");
        subtitle = tr("Editing clip on timeline");
        type = tr("CLIP");
        icon = QIcon::fromTheme("video-x-generic");
        break;
    case InspectorContext::BinClip:
        title = tr("Project Bin Clip");
        subtitle = tr("Editing clip in project bin");
        type = tr("BIN");
        icon = QIcon::fromTheme("video-x-generic");
        break;
    case InspectorContext::TimelineTrack:
        title = tr("Track Effects");
        subtitle = tr("Effects applied to track");
        type = tr("TRACK");
        icon = QIcon::fromTheme("audio-x-generic");
        break;
    case InspectorContext::TimelineComposition:
        title = tr("Transition / Mix");
        subtitle = tr("Editing transition parameters");
        type = tr("TRANSITION");
        icon = QIcon::fromTheme("kdenlive-transition");
        break;
    case InspectorContext::TimelineMaster:
        title = tr("Master Effects");
        subtitle = tr("Effects applied to entire sequence");
        type = tr("MASTER");
        icon = QIcon::fromTheme("applications-multimedia");
        break;
    default:
        break;
    }

    m_header->setContent(title, subtitle, type, icon);
}

void ArkenInspector::updateCategories()
{
    if (!m_stackModel && !m_paramModel) {
        return;
    }

    // Update active categories with models
    for (auto *cat : m_activeCategories) {
        cat->setModel(m_stackModel, m_paramModel, m_frameSize);
    }
}

// --- Slots for Core signals ---
void ArkenInspector::onTimelineClipSelected(int clipId, const QUuid &timelineUuid)
{
    Q_UNUSED(timelineUuid)
    // Model will be fetched via Core::showEffectStackFromId
    // This slot just ensures inspector is ready
}

void ArkenInspector::onTimelineTrackSelected(int trackId, const QUuid &timelineUuid)
{
    Q_UNUSED(trackId)
    Q_UNUSED(timelineUuid)
}

void ArkenInspector::onTimelineCompositionSelected(int compId, const QUuid &timelineUuid)
{
    Q_UNUSED(compId)
    Q_UNUSED(timelineUuid)
}

void ArkenInspector::onTimelineMasterSelected(const QUuid &timelineUuid)
{
    Q_UNUSED(timelineUuid)
}

void ArkenInspector::onBinClipSelected(const QUuid &clipUuid)
{
    Q_UNUSED(clipUuid)
}

} // namespace Arken

#include "arkeninspector_main.moc"