// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default theme, matching the home screen (dark background, grass green accent, square edges).
 */

#pragma once

#include "FusionTheme.h"

class TaroTheme : public FusionTheme {
   public:
    ~TaroTheme() override = default;

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
