// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default modpack
 */

#include "TaroModpack.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

namespace TaroModpack {

namespace {

bool copyFile(const QString& src, const QString& dst)
{
    if (QFileInfo::exists(dst)) {
        QFile::remove(dst);
    }
    QDir().mkpath(QFileInfo(dst).absolutePath());
    if (!QFile::copy(src, dst)) {
        qWarning() << "TaroModpack: could not copy" << src << "to" << dst;
        return false;
    }
    // files copied from a read-only install location keep their read-only flag; drop it
    QFile::setPermissions(dst, QFile::permissions(dst) | QFileDevice::WriteOwner | QFileDevice::WriteUser);
    return true;
}

// TaroLauncher 1.0.6 created an empty "TaroModpack" instance from a template (marked by taro_modpack.json).
// It is removed so the real modpack can take its place, but only if the player never added anything to it.
void removeLegacyPlaceholder(const QString& instancesDir)
{
    const QDir instance(QDir(QDir(instancesDir).absolutePath()).filePath("TaroModpack"));
    if (!instance.exists("taro_modpack.json")) {
        return;
    }
    for (const auto* sub : { "minecraft/mods", "minecraft/saves", ".minecraft/mods", ".minecraft/saves" }) {
        const QDir dir(instance.filePath(sub));
        if (dir.exists() && !dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden).isEmpty()) {
            qInfo() << "TaroModpack: old placeholder instance has player content, keeping it";
            QFile::remove(instance.filePath("taro_modpack.json"));
            return;
        }
    }
    qInfo() << "TaroModpack: removing old placeholder instance" << instance.absolutePath();
    QDir(instance.absolutePath()).removeRecursively();
}

}  // namespace

QString findTemplate(const QStringList& searchRoots)
{
    for (const auto& root : searchRoots) {
        const QDir candidate(QDir(root).filePath("TaroModpack"));
        if (candidate.exists("modpack.json")) {
            return candidate.absolutePath();
        }
    }
    return {};
}

Config readConfig(const QString& templateDir)
{
    Config config;
    if (templateDir.isEmpty()) {
        return config;
    }
    QFile file(QDir(templateDir).filePath("modpack.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        return config;
    }
    const auto obj = QJsonDocument::fromJson(file.readAll()).object();
    config.name = obj.value("name").toString(config.name).trimmed();
    if (config.name.isEmpty()) {
        config.name = "TaroModpack";
    }
    config.modrinthProject = obj.value("modrinth_project").toString().trimmed();
    config.gameVersion = obj.value("game_version").toString().trimmed();
    config.loader = obj.value("loader").toString().trimmed().toLower();
    config.newsUrl = obj.value("news_url").toString().trimmed();
    config.discordClientId = obj.value("discord_client_id").toString().trimmed();
    config.discordImage = obj.value("discord_image").toString().trimmed();
    config.downloadUrl = obj.value("download_url").toString().trimmed();
    config.microsoftClientId = obj.value("microsoft_client_id").toString().trimmed();
    return config;
}

void prepareStartup(const QString& templateDir, const QString& instancesDir, const QString& iconsDir)
{
    if (templateDir.isEmpty()) {
        qWarning() << "TaroModpack: bundled TaroModpack folder not found";
        return;
    }
    const auto icon = QDir(templateDir).filePath("icon.png");
    if (QFileInfo::exists(icon)) {
        copyFile(icon, QDir(iconsDir).filePath(QString(ICON_KEY) + ".png"));
    }
    removeLegacyPlaceholder(instancesDir);
}

}  // namespace TaroModpack
