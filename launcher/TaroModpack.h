// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default modpack installer
 *
 *  The launcher ships a template instance called "TaroModpack" next to the
 *  executable. On every start this code makes sure that the user has it in
 *  their instances folder:
 *
 *   - If the instance does not exist yet, the whole template is copied.
 *   - If it exists but the template has a higher "version" (taro_modpack.json),
 *     the modpack is updated: the mods folder is replaced, config files and the
 *     other bundled folders are overwritten, mmc-pack.json is refreshed.
 *     Worlds, screenshots, options.txt and servers.dat are kept.
 */

#pragma once

#include <QString>
#include <QStringList>

namespace TaroModpack {

constexpr auto INSTANCE_ID = "TaroModpack";
constexpr auto ICON_KEY = "taromodpack";
constexpr auto VERSION_FILE = "taro_modpack.json";

// Returns the folder that contains the bundled template, or an empty string if it was not installed.
QString findTemplate(const QStringList& searchRoots);

// Copies/updates the modpack. Returns true if something was installed or updated.
bool ensureInstalled(const QString& templateDir, const QString& instancesDir, const QString& iconsDir);

}  // namespace TaroModpack
