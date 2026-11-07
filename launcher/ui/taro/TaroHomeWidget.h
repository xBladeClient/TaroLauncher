// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - home screen ("menu principal").
 *
 *  Pure view: it only depends on Qt. MainWindow fills it with data (profiles, player, news) and reacts to
 *  its signals. This keeps the launcher logic in the existing Prism code.
 */

#pragma once

#include <QFrame>
#include <QList>
#include <QPixmap>
#include <QPointer>
#include <QString>
#include <QWidget>

class QLabel;
class QMenu;
class QPushButton;
class QToolButton;
class QVBoxLayout;
class TaroHeroWidget;

struct TaroHomeProfile {
    QString id;
    QString name;
    QString loader;        // e.g. "Fabric"
    QString gameVersion;   // e.g. "26.3"
    qint64 lastLaunch = 0; // ms since epoch, 0 = never
    QPixmap icon;
    bool running = false;
    bool canLaunch = true;
};

struct TaroHomeNews {
    QString tag;    // small label, e.g. "LANZADOR"
    QString title;
    QString date;   // free text, e.g. "Hace 3 días" or "2026-11-07"
    QString url;    // opened when clicked (optional)
};

class TaroHomeWidget : public QWidget {
    Q_OBJECT
   public:
    explicit TaroHomeWidget(QWidget* parent = nullptr);

    void setPlayer(const QString& name, const QPixmap& face);
    void setAccountMenu(QMenu* menu);
    void setProfiles(const QList<TaroHomeProfile>& profiles);
    void setNews(const QList<TaroHomeNews>& news);
    void setStatusText(const QString& text);  // small line under the profile list (e.g. "Instalando TaroModpack...")

    // Formats "Hace 10 días" etc.
    static QString relativeTime(qint64 msSinceEpoch);
    // Registers the bundled pixel font once; returns its family name (or an empty string).
    static QString pixelFontFamily(bool bold = false);

   signals:
    void launchRequested(const QString& instanceId);
    void killRequested(const QString& instanceId);
    void editRequested(const QString& instanceId);
    void manageProfilesRequested();
    void addModsRequested();
    void foldersRequested();
    void accountsRequested();
    void settingsRequested();
    void helpRequested();
    void newsClicked(const QString& url);

   private:
    QWidget* buildSidebar();
    QWidget* buildTopBar();
    QWidget* buildProfileCard(const TaroHomeProfile& profile, bool featured);
    QWidget* buildNewsCard(const TaroHomeNews& news);
    QToolButton* sideButton(const QString& glyph, const QString& tooltip, bool active = false);

    TaroHeroWidget* m_hero = nullptr;
    QVBoxLayout* m_profilesLayout = nullptr;
    QVBoxLayout* m_newsLayout = nullptr;
    QLabel* m_status = nullptr;
    QToolButton* m_accountButton = nullptr;
    QPointer<QMenu> m_accountMenu;
};

// The big banner with the player's name and face.
class TaroHeroWidget : public QWidget {
    Q_OBJECT
   public:
    explicit TaroHeroWidget(QWidget* parent = nullptr);
    void setPlayer(const QString& name, const QPixmap& face);
    QSize sizeHint() const override { return { 640, 300 }; }
    QSize minimumSizeHint() const override { return { 320, 220 }; }

   signals:
    void clicked();

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

   private:
    QPixmap m_banner;
    QPixmap m_logo;
    QString m_name;
    QPixmap m_face;
};
