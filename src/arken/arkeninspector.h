/*
    SPDX-FileCopyrightText: 2026 ArkenV contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <memory>

#include "definitions.h"

class AssetParameterModel;
class EffectStackModel;
class QSize;

namespace Arken {

/** @brief Context type for the Inspector.
 *  Determines which presentation and models are used.
 */
enum class InspectorContext {
    None = 0,
    TimelineClip = 1,
    TimelineTrack = 2,
    TimelineComposition = 3,  // transition/mix
    TimelineMaster = 4,
    BinClip = 5,
};

/** @brief Base class for Inspector categories (Transform, Video, Audio, Effects, Advanced).
 *  Each category manages its own collapsible UI and parameter models.
 */
class InspectorCategory : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorCategory(const QString &title, QWidget *parent = nullptr);
    ~InspectorCategory() override = default;

    virtual void setModel(std::shared_ptr<EffectStackModel> stackModel,
                          std::shared_ptr<AssetParameterModel> paramModel,
                          const QSize &frameSize) = 0;
    virtual void clearModel() = 0;
    virtual bool hasContent() const = 0;

    const QString &title() const { return m_title; }
    void setExpanded(bool expanded);
    bool isExpanded() const { return m_expanded; }

Q_SIGNALS:
    void expansionChanged(bool expanded);
    void contentHeightChanged();

protected:
    QString m_title;
    bool m_expanded = true;
    QWidget *m_contentWidget = nullptr;
};

/** @brief Transform category — unified position/scale/rotation/anchor from built-in effects. */
class InspectorTransformCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorTransformCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

private:
    QFormLayout *m_layout = nullptr;
};

/** @brief Video category — opacity, blend mode, crop, color. */
class InspectorVideoCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorVideoCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

private:
    QFormLayout *m_layout = nullptr;
};

/** @brief Audio category — volume, pan, keyframes. */
class InspectorAudioCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorAudioCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

private:
    QFormLayout *m_layout = nullptr;
};

/** @brief Effects category — list of applied effects with add/remove/reorder. */
class InspectorEffectsCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorEffectsCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

Q_SIGNALS:
    void effectActivated(const QString &effectId);
    void effectAdded(const QString &effectId);
    void effectRemoved(const QString &effectId);

private:
    QVBoxLayout *m_layout = nullptr;
};

/** @brief Advanced category — technical settings (field order, proxy, etc.). */
class InspectorAdvancedCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorAdvancedCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

private:
    QFormLayout *m_layout = nullptr;
};

/** @brief Transition/Mix category — parameters for transitions and audio mixes. */
class InspectorTransitionCategory : public InspectorCategory
{
    Q_OBJECT
public:
    explicit InspectorTransitionCategory(QWidget *parent = nullptr);
    void setModel(std::shared_ptr<EffectStackModel>, std::shared_ptr<AssetParameterModel>, const QSize &) override;
    void clearModel() override;
    bool hasContent() const override;

private:
    QFormLayout *m_layout = nullptr;
};

/** @brief Empty state widget shown when no contextual object is selected. */
class InspectorEmptyState : public QWidget
{
    Q_OBJECT
public:
    explicit InspectorEmptyState(QWidget *parent = nullptr);
};

} // namespace Arken