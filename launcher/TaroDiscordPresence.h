// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - Discord Rich Presence.
 *
 *  Talks to the local Discord client through its IPC socket (\\.\pipe\discord-ipc-N on Windows,
 *  $XDG_RUNTIME_DIR/discord-ipc-N elsewhere), so no Discord SDK is needed. Shows "Jugando TaroLauncher"
 *  with the launcher icon (also visible next to the user in voice channels) and what is being played.
 *
 *  Needs the Application ID of a Discord application (discord.com/developers/applications); it is read from
 *  TaroModpack/modpack.json ("discord_client_id"). Without it, presence is disabled.
 */

#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QLocalSocket>
#include <QObject>
#include <QTimer>

class TaroDiscordPresence : public QObject {
    Q_OBJECT
   public:
    struct Activity {
        QString details;          // first line, e.g. "Jugando TaroModpack"
        QString state;            // second line, e.g. "Fabric 26.3"
        qint64 startSecs = 0;     // unix time; shows "elapsed" when set
        QString largeImage;       // asset key or https URL
        QString largeText;
        QString buttonLabel;      // optional button
        QString buttonUrl;
        bool operator==(const Activity& o) const
        {
            return details == o.details && state == o.state && startSecs == o.startSecs && largeImage == o.largeImage &&
                   largeText == o.largeText && buttonLabel == o.buttonLabel && buttonUrl == o.buttonUrl;
        }
    };

    explicit TaroDiscordPresence(const QString& clientId, QObject* parent = nullptr);
    ~TaroDiscordPresence() override;

    bool isEnabled() const { return !m_clientId.isEmpty(); }
    void setEnabled(bool enabled);
    void setActivity(const Activity& activity);
    void clearActivity();

   private:
    void tryConnect();
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void sendFrame(int opcode, const QJsonObject& payload);
    void sendActivity();
    static QString socketName(int index);

    QString m_clientId;
    bool m_enabled = true;
    QLocalSocket m_socket;
    QTimer m_retryTimer;
    QByteArray m_buffer;
    int m_pipeIndex = 0;
    bool m_ready = false;
    bool m_hasActivity = false;
    Activity m_activity;
};
