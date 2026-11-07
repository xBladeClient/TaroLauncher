// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default modpack
 *
 *  The installer ships a small folder called "TaroModpack" next to the executable:
 *
 *    TaroModpack/icon.png       icon of the instance
 *    TaroModpack/modpack.json   which Modrinth modpack to install, e.g.
 *                               { "name": "TaroModpack", "modrinth_project": "oePruTVg",
 *                                 "game_version": "26.3", "loader": "fabric",
 *                                 "news_url": "https://raw.githubusercontent.com/<user>/<repo>/main/taro/news.json" }
 *
 *  At startup (before icons/instances are loaded) prepareStartup() installs the icon and removes the
 *  empty placeholder instance created by TaroLauncher 1.0.6. The actual download/update of the modpack
 *  from Modrinth is done by ui/taro/TaroModpackManager once the main window is shown.
 */

#pragma once

#include <QString>
#include <QStringList>

namespace TaroModpack {

constexpr auto ICON_KEY = "taromodpack";

struct Config {
    QString name = "TaroModpack";
    QString modrinthProject;  // Modrinth project id or slug
    QString gameVersion;      // optional filter, e.g. "26.3"
    QString loader;           // optional filter, e.g. "fabric"
    QString newsUrl;          // optional JSON list of news shown on the home screen
    bool isValid() const { return !modrinthProject.isEmpty(); }
};

// Returns the bundled "TaroModpack" folder, or an empty string if it is not installed.
QString findTemplate(const QStringList& searchRoots);

// Reads TaroModpack/modpack.json. Returns an invalid config if the file is missing or broken.
Config readConfig(const QString& templateDir);

// Copies the icon into the icons folder and removes the old empty placeholder instance (if untouched).
void prepareStartup(const QString& templateDir, const QString& instancesDir, const QString& iconsDir);

}  // namespace TaroModpack
