// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - Discord Rich Presence.
 */

#include "TaroDiscordPresence.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcessEnvironment>
#include <QUuid>
#include <QtEndian>

namespace {
enum Opcode { Handshake = 0, Frame = 1, Close = 2, Ping = 3, Pong = 4 };
constexpr int RETRY_MS = 20000;
}  // namespace

TaroDiscordPresence::TaroDiscordPresence(const QString& clientId, QObject* parent) : QObject(parent), m_clientId(clientId.trimmed())
{
    m_retryTimer.setSingleShot(true);
    m_retryTimer.setInterval(RETRY_MS);
    connect(&m_retryTimer, &QTimer::timeout, this, [this] {
        m_pipeIndex = 0;
        tryConnect();
    });
    connect(&m_socket, &QLocalSocket::connected, this, &TaroDiscordPresence::onConnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &TaroDiscordPresence::onReadyRead);
    connect(&m_socket, &QLocalSocket::disconnected, this, &TaroDiscordPresence::onDisconnected);
    connect(&m_socket, &QLocalSocket::errorOccurred, this, [this](QLocalSocket::LocalSocketError) {
        if (m_socket.state() == QLocalSocket::ConnectedState) {
            return;
        }
        // try the next pipe (Discord, PTB and Canary use discord-ipc-0..9)
        m_pipeIndex++;
        if (m_pipeIndex < 10) {
            QTimer::singleShot(0, this, &TaroDiscordPresence::tryConnect);
        } else if (m_enabled) {
            m_retryTimer.start();
        }
    });
    if (isEnabled()) {
        QTimer::singleShot(0, this, &TaroDiscordPresence::tryConnect);
    }
}

TaroDiscordPresence::~TaroDiscordPresence()
{
    if (m_socket.state() == QLocalSocket::ConnectedState) {
        m_socket.abort();
    }
}

QString TaroDiscordPresence::socketName(int index)
{
#ifdef Q_OS_WIN
    return QString("discord-ipc-%1").arg(index);  // QLocalSocket adds \\.\pipe\ on Windows
#else
    const auto env = QProcessEnvironment::systemEnvironment();
    QString base;
    for (const auto* var : { "XDG_RUNTIME_DIR", "TMPDIR", "TMP", "TEMP" }) {
        base = env.value(var);
        if (!base.isEmpty()) {
            break;
        }
    }
    if (base.isEmpty()) {
        base = "/tmp";
    }
    // Flatpak and Snap versions of Discord put the socket in a sub folder
    for (const auto* sub : { "", "app/com.discordapp.Discord/", "snap.discord/" }) {
        const auto path = QDir(base).filePath(QString("%1discord-ipc-%2").arg(sub).arg(index));
        if (QFileInfo::exists(path)) {
            return path;
        }
    }
    return QDir(base).filePath(QString("discord-ipc-%1").arg(index));
#endif
}

void TaroDiscordPresence::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!enabled) {
        m_retryTimer.stop();
        if (m_socket.state() != QLocalSocket::UnconnectedState) {
            m_socket.abort();
        }
    } else if (isEnabled() && m_socket.state() == QLocalSocket::UnconnectedState) {
        m_pipeIndex = 0;
        tryConnect();
    }
}

void TaroDiscordPresence::tryConnect()
{
    if (!isEnabled() || !m_enabled || m_socket.state() != QLocalSocket::UnconnectedState) {
        return;
    }
    m_ready = false;
    m_buffer.clear();
    m_socket.connectToServer(socketName(m_pipeIndex));
}

void TaroDiscordPresence::onConnected()
{
    qInfo() << "Discord: connected to" << m_socket.serverName();
    sendFrame(Handshake, QJsonObject{ { "v", 1 }, { "client_id", m_clientId } });
}

void TaroDiscordPresence::onDisconnected()
{
    if (m_ready) {
        qInfo() << "Discord: disconnected";
    }
    m_ready = false;
    if (m_enabled && isEnabled()) {
        m_retryTimer.start();
    }
}

void TaroDiscordPresence::onReadyRead()
{
    m_buffer.append(m_socket.readAll());
    while (m_buffer.size() >= 8) {
        const auto opcode = qFromLittleEndian<qint32>(m_buffer.constData());
        const auto length = qFromLittleEndian<qint32>(m_buffer.constData() + 4);
        if (length < 0 || m_buffer.size() < 8 + length) {
            return;
        }
        const auto payload = QJsonDocument::fromJson(m_buffer.mid(8, length)).object();
        m_buffer.remove(0, 8 + length);

        if (opcode == Ping) {
            sendFrame(Pong, payload);
        } else if (opcode == Close) {
            qWarning() << "Discord: connection closed:" << payload.value("message").toString();
            m_socket.disconnectFromServer();
            return;
        } else if (opcode == Frame) {
            const auto evt = payload.value("evt").toString();
            if (evt == "READY") {
                m_ready = true;
                if (m_hasActivity) {
                    sendActivity();
                }
            } else if (evt == "ERROR") {
                qWarning() << "Discord:" << payload.value("data").toObject().value("message").toString();
            }
        }
    }
}

void TaroDiscordPresence::sendFrame(int opcode, const QJsonObject& payload)
{
    const auto json = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QByteArray frame(8, Qt::Uninitialized);
    qToLittleEndian<qint32>(opcode, frame.data());
    qToLittleEndian<qint32>(static_cast<qint32>(json.size()), frame.data() + 4);
    frame.append(json);
    m_socket.write(frame);
    m_socket.flush();
}

void TaroDiscordPresence::setActivity(const Activity& activity)
{
    if (m_hasActivity && m_activity == activity) {
        return;
    }
    m_activity = activity;
    m_hasActivity = true;
    if (m_ready) {
        sendActivity();
    }
}

void TaroDiscordPresence::clearActivity()
{
    m_hasActivity = false;
    if (m_ready) {
        sendActivity();
    }
}

void TaroDiscordPresence::sendActivity()
{
    QJsonObject args{ { "pid", static_cast<qint64>(QCoreApplication::applicationPid()) } };
    if (m_hasActivity) {
        QJsonObject activity;
        activity["type"] = 0;  // "Playing"
        if (!m_activity.details.isEmpty()) {
            activity["details"] = m_activity.details.left(128);
        }
        if (!m_activity.state.isEmpty()) {
            activity["state"] = m_activity.state.left(128);
        }
        if (m_activity.startSecs > 0) {
            activity["timestamps"] = QJsonObject{ { "start", m_activity.startSecs } };
        }
        QJsonObject assets;
        if (!m_activity.largeImage.isEmpty()) {
            assets["large_image"] = m_activity.largeImage;
        }
        if (!m_activity.largeText.isEmpty()) {
            assets["large_text"] = m_activity.largeText.left(128);
        }
        if (!assets.isEmpty()) {
            activity["assets"] = assets;
        }
        if (!m_activity.buttonLabel.isEmpty() && m_activity.buttonUrl.startsWith("https://")) {
            activity["buttons"] = QJsonArray{ QJsonObject{ { "label", m_activity.buttonLabel.left(32) }, { "url", m_activity.buttonUrl } } };
        }
        args["activity"] = activity;
    }
    sendFrame(Frame, QJsonObject{ { "cmd", "SET_ACTIVITY" },
                                  { "args", args },
                                  { "nonce", QUuid::createUuid().toString(QUuid::WithoutBraces) } });
}
