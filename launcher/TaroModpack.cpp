// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default modpack installer
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

// Files inside the game folder that belong to the player and must survive an update.
const QStringList s_keepOnUpdate = { "options.txt", "servers.dat", "servers.dat_old" };

int readVersion(const QString& dir)
{
    QFile file(QDir(dir).filePath(VERSION_FILE));
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    const auto doc = QJsonDocument::fromJson(file.readAll());
    return doc.object().value("version").toInt(0);
}

bool copyFile(const QString& src, const QString& dst, bool overwrite)
{
    if (QFileInfo::exists(dst)) {
        if (!overwrite) {
            return true;
        }
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

// Recursively copies the contents of src into dst. Placeholder ".gitkeep" files are skipped.
bool copyTree(const QString& src, const QString& dst, bool overwrite, const QStringList& keepIfExists = {})
{
    bool ok = true;
    QDir srcDir(src);
    QDir().mkpath(dst);
    QDirIterator it(src, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        const auto info = it.fileInfo();
        const auto relative = srcDir.relativeFilePath(info.absoluteFilePath());
        const auto target = QDir(dst).filePath(relative);
        if (info.isDir()) {
            QDir().mkpath(target);
            continue;
        }
        if (info.fileName() == ".gitkeep") {
            continue;
        }
        const bool keep = keepIfExists.contains(relative);
        ok &= copyFile(info.absoluteFilePath(), target, overwrite && !keep);
    }
    return ok;
}

QString gameFolderOf(const QString& instanceRoot)
{
    // same rule as MinecraftInstance::gameRoot()
    QDir root(instanceRoot);
    if (root.exists(".minecraft") && !root.exists("minecraft")) {
        return root.filePath(".minecraft");
    }
    return root.filePath("minecraft");
}

void installIcon(const QString& templateDir, const QString& iconsDir)
{
    const auto icon = QDir(templateDir).filePath("icon.png");
    if (!QFileInfo::exists(icon)) {
        return;
    }
    QDir().mkpath(iconsDir);
    copyFile(icon, QDir(iconsDir).filePath(QString(ICON_KEY) + ".png"), true);
}

bool updateExisting(const QString& templateDir, const QString& target)
{
    bool ok = true;

    // Launcher-side files: component list (Minecraft / Fabric versions) and the version marker.
    // instance.cfg is NOT overwritten so the player's instance settings (RAM, Java, play time...) are kept.
    for (const auto* name : { "mmc-pack.json", VERSION_FILE }) {
        const auto src = QDir(templateDir).filePath(name);
        if (QFileInfo::exists(src)) {
            ok &= copyFile(src, QDir(target).filePath(name), true);
        }
    }
    // Component patches (e.g. custom jar mods), if the template has any
    if (QDir(templateDir).exists("patches")) {
        ok &= copyTree(QDir(templateDir).filePath("patches"), QDir(target).filePath("patches"), true);
    }

    const auto srcGame = gameFolderOf(templateDir);
    const auto dstGame = gameFolderOf(target);
    if (!QDir(srcGame).exists()) {
        return ok;
    }

    // The mods folder is replaced as a whole, so removed/renamed mods do not stay behind.
    const auto srcMods = QDir(srcGame).filePath("mods");
    if (QDir(srcMods).exists()) {
        QDir(QDir(dstGame).filePath("mods")).removeRecursively();
        ok &= copyTree(srcMods, QDir(dstGame).filePath("mods"), true);
    }

    // Everything else (config, resourcepacks, shaderpacks, ...) is overwritten file by file.
    QDir srcGameDir(srcGame);
    for (const auto& entry : srcGameDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden)) {
        if (entry.fileName() == "mods") {
            continue;
        }
        const auto dst = QDir(dstGame).filePath(entry.fileName());
        if (entry.isDir()) {
            ok &= copyTree(entry.absoluteFilePath(), dst, true);
        } else if (entry.fileName() != ".gitkeep") {
            ok &= copyFile(entry.absoluteFilePath(), dst, !s_keepOnUpdate.contains(entry.fileName()));
        }
    }
    return ok;
}

}  // namespace

QString findTemplate(const QStringList& searchRoots)
{
    for (const auto& root : searchRoots) {
        const auto candidate = QDir(root).filePath(INSTANCE_ID);
        if (QFileInfo::exists(QDir(candidate).filePath("instance.cfg"))) {
            return QDir(candidate).absolutePath();
        }
    }
    return {};
}

bool ensureInstalled(const QString& templateDir, const QString& instancesDir, const QString& iconsDir)
{
    if (templateDir.isEmpty()) {
        qWarning() << "TaroModpack: template not found, skipping";
        return false;
    }

    const auto target = QDir(QDir(instancesDir).absolutePath()).filePath(INSTANCE_ID);
    const int templateVersion = readVersion(templateDir);

    if (!QFileInfo::exists(QDir(target).filePath("instance.cfg"))) {
        qInfo() << "TaroModpack: installing version" << templateVersion << "into" << target;
        const bool ok = copyTree(templateDir, target, true);
        installIcon(templateDir, iconsDir);
        return ok;
    }

    const int installedVersion = readVersion(target);
    if (templateVersion <= installedVersion) {
        qInfo() << "TaroModpack: up to date (version" << installedVersion << ")";
        if (!QFileInfo::exists(QDir(iconsDir).filePath(QString(ICON_KEY) + ".png"))) {
            installIcon(templateDir, iconsDir);
        }
        return false;
    }

    qInfo() << "TaroModpack: updating from version" << installedVersion << "to" << templateVersion;
    const bool ok = updateExisting(templateDir, target);
    installIcon(templateDir, iconsDir);
    return ok;
}

}  // namespace TaroModpack
