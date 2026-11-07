// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - installs and updates TaroModpack from Modrinth.
 *
 *  On the first start the modpack configured in TaroModpack/modpack.json (KeoOptimized by default) is
 *  downloaded from Modrinth and installed as an instance called "TaroModpack" with the Taro icon.
 *  On later starts, if Modrinth has a newer version, the player is asked whether to update.
 */

#pragma once

#include <QObject>
#include <QPointer>
#include <QString>

#include "TaroModpack.h"

class InstanceTask;
class MinecraftInstance;
class QNetworkReply;
class QWidget;

class TaroModpackManager : public QObject {
    Q_OBJECT
   public:
    explicit TaroModpackManager(QWidget* window);

    // Checks Modrinth and installs/offers an update. Safe to call more than once; only runs once per session
    // unless force is true.
    void check(bool force = false);

    // The installed TaroModpack instance, or nullptr if it is not installed (yet).
    MinecraftInstance* installedInstance() const;

    const TaroModpack::Config& config() const { return m_config; }

   signals:
    // MainWindow runs the task with its usual progress dialog.
    void runInstanceTask(InstanceTask* task);

   private:
    void onVersionsReceived(QNetworkReply* reply);

    QWidget* m_window;
    TaroModpack::Config m_config;
    QString m_projectId;  // resolved Modrinth project id (config may contain a slug)
    QPointer<QNetworkReply> m_reply;
    bool m_checked = false;
};
