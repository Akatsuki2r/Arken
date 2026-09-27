/*
    SPDX-FileCopyrightText: 2026 ArkenV contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "arkeninspector.h"
#include "definitions.h"

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <memory>

class EffectStackModel;
class AssetParameterModel;
class QSize;

namespace Arken {

class InspectorHeader;
class InspectorCategories;

/** @brief Main ArkenV contextual inspector widget.
 *
 *  Receives selection context via Core signals and presents a unified,
 *  hierarchical inspector with progressive disclosure.
 */
class ArkenInspector : public QWidget
{
    Q_OBJECT
public:
    explicit ArkenInspector(QWidget *parent = nullptr);
    ~ArkenInspector() override;

    /** @brief Set the current inspection context.
     *  Called by Core/MainWindow when selection changes.
     */
    void setContext(InspectorContext context, const ObjectId &owner,
                    std::shared_ptr<EffectStackModel> stackModel = nullptr,
                    std::shared_ptr<AssetParameterModel> paramModel = nullptr,
                    const QSize &frameSize = QSize(),
                    bool showKeyframes = false);

    /** @brief Clear the inspector to empty state. */
    void clear();

    /** @brief Current context type. */
    InspectorContext currentContext() const { return m_context; }

    /** @brief Current owner object ID. */
    ObjectId currentOwner() const { return m_owner; }

public Q_SLOTS:
    /** @brief Handle timeline clip selection. */
    void onTimelineClipSelected(int clipId, const QUuid &timelineUuid);

    /** @brief Handle timeline track selection. */
    void onTimelineTrackSelected(int trackId, const QUuid &timelineUuid);

    /** @brief Handle timeline composition/transition selection. */
    void onTimelineCompositionSelected(int compId, const QUuid &timelineUuid);

    /** @brief Handle timeline master effects selection. */
    void onTimelineMasterSelected(const QUuid &timelineUuid);

    /** @brief Handle bin clip selection. */
    void onBinClipSelected(const QUuid &clipUuid);

Q_SIGNALS:
    void contextChanged(InspectorContext context, const ObjectId &owner);
    void effectActivated(const QString &effectId);
    void addEffectRequested(const QString &effectId);

private:
    void setupUI();
    void showContext(InspectorContext context);
    void updateHeader();
    void updateCategories();
    void applyTheme();

    InspectorContext m_context = InspectorContext::None;
    ObjectId m_owner;
    std::shared_ptr<EffectStackModel> m_stackModel;
    std::shared_ptr<AssetParameterModel> m_paramModel;
    QSize m_frameSize;
    bool m_showKeyframes = false;

    QVBoxLayout *m_mainLayout = nullptr;
    InspectorHeader *m_header = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContent = nullptr;
    QVBoxLayout *m_categoriesLayout = nullptr;
    InspectorEmptyState *m_emptyState = nullptr;

    // Categories
    InspectorTransformCategory *m_transformCategory = nullptr;
    InspectorVideoCategory *m_videoCategory = nullptr;
    InspectorAudioCategory *m_audioCategory = nullptr;
    InspectorEffectsCategory *m_effectsCategory = nullptr;
    InspectorAdvancedCategory *m_advancedCategory = nullptr;
    InspectorTransitionCategory *m_transitionCategory = nullptr;

    // Active categories for current context
    QList<InspectorCategory *> m_activeCategories;
};

} // namespace Arken