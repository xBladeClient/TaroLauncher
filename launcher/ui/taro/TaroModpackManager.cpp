// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - installs and updates TaroModpack from Modrinth.
 */

#include "TaroModpackManager.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QWidget>

#include "Application.h"
#include "BuildConfig.h"
#include "InstanceImportTask.h"
#include "InstanceList.h"
#include "minecraft/MinecraftInstance.h"

TaroModpackManager::TaroModpackManager(QWidget* window) : QObject(window), m_window(window)
{
    m_config = TaroModpack::readConfig(APPLICATION->taroModpackDir());
    m_projectId = m_config.modrinthProject;
}

MinecraftInstance* TaroModpackManager::installedInstance() const
{
    if (!m_config.isValid()) {
        return nullptr;
    }
    auto* instances = APPLICATION->instances();
    for (int i = 0; i < instances->count(); i++) {
        auto* inst = instances->at(i);
        if (inst->isManagedPack() && inst->getManagedPackType() == "modrinth" &&
            (inst->getManagedPackID() == m_projectId || inst->getManagedPackID() == m_config.modrinthProject)) {
            return inst;
        }
    }
    return nullptr;
}

void TaroModpackManager::check(bool force)
{
    if (!m_config.isValid()) {
        qWarning() << "TaroModpack: no modpack configured (TaroModpack/modpack.json)";
        return;
    }
    if ((m_checked && !force) || m_reply) {
        return;
    }
    m_checked = true;

    QUrl url(QString("%1/project/%2/version").arg(BuildConfig.MODRINTH_PROD_URL, m_config.modrinthProject));
    QUrlQuery query;
    if (!m_config.loader.isEmpty()) {
        query.addQueryItem("loaders", QString("[\"%1\"]").arg(m_config.loader));
    }
    if (!m_config.gameVersion.isEmpty()) {
        query.addQueryItem("game_versions", QString("[\"%1\"]").arg(m_config.gameVersion));
    }
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, BuildConfig.USER_AGENT);
    qInfo() << "TaroModpack: checking" << url.toString();
    m_reply = APPLICATION->network()->get(request);
    auto* reply = m_reply.data();
    connect(reply, &QNetworkReply::finished, this, [this, reply] { onVersionsReceived(reply); });
}

void TaroModpackManager::onVersionsReceived(QNetworkReply* reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "TaroModpack: could not reach Modrinth:" << reply->errorString();
        m_checked = false;  // try again next time
        return;
    }

    const auto versions = QJsonDocument::fromJson(reply->readAll()).array();
    // Modrinth returns the newest version first; skip anything that is not a release if possible
    QJsonObject latest;
    for (const auto& value : versions) {
        const auto obj = value.toObject();
        if (latest.isEmpty()) {
            latest = obj;
        }
        if (obj.value("version_type").toString() == "release") {
            latest = obj;
            break;
        }
    }
    if (latest.isEmpty()) {
        qWarning() << "TaroModpack: Modrinth returned no versions for" << m_config.modrinthProject;
        return;
    }

    m_projectId = latest.value("project_id").toString(m_projectId);
    const auto versionId = latest.value("id").toString();
    const auto versionName = latest.value("version_number").toString(latest.value("name").toString());

    QString downloadUrl;
    for (const auto& file : latest.value("files").toArray()) {
        const auto obj = file.toObject();
        if (downloadUrl.isEmpty() || obj.value("primary").toBool()) {
            downloadUrl = obj.value("url").toString();
        }
    }
    if (downloadUrl.isEmpty()) {
        qWarning() << "TaroModpack: version" << versionId << "has no files";
        return;
    }

    QMap<QString, QString> extraInfo;
    extraInfo.insert("pack_id", m_projectId);
    extraInfo.insert("pack_version_id", versionId);

    auto* existing = installedInstance();
    if (!existing) {
        qInfo() << "TaroModpack: installing" << m_projectId << versionName;
        auto* task = new InstanceImportTask(QUrl(downloadUrl), true, m_window, extraInfo);
        task->setName(m_config.name);
        task->setIcon(TaroModpack::ICON_KEY);
        task->setGroup(QString());
        emit runInstanceTask(task);
        return;
    }

    if (existing->getManagedPackVersionID() == versionId) {
        qInfo() << "TaroModpack: up to date (" << versionName << ")";
        return;
    }
    if (existing->isRunning()) {
        return;
    }

    const auto answer = QMessageBox::question(
        m_window, tr("Actualización de %1").arg(m_config.name),
        tr("Hay una nueva versión de %1 (%2).\n\n¿Querés actualizar ahora? Tus mundos y ajustes se conservan.")
            .arg(existing->name(), versionName),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (answer != QMessageBox::Yes) {
        return;
    }

    extraInfo.insert("original_instance_id", existing->id());
    auto* task = new InstanceImportTask(QUrl(downloadUrl), true, m_window, extraInfo);
    task->setName(existing->name());
    task->setIcon(existing->iconKey());
    task->setGroup(APPLICATION->instances()->getInstanceGroup(existing->id()));
    task->setConfirmUpdate(false);
    emit runInstanceTask(task);
}
