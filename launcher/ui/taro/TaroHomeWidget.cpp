// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - home screen ("menu principal").
 */

#include "TaroHomeWidget.h"

#include <QDateTime>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

// Palette (Taro: grass green + dirt brown on a dark background)
constexpr auto C_BG = "#0e100f";
constexpr auto C_PANEL = "#161917";
constexpr auto C_PANEL_HOVER = "#1d211e";
constexpr auto C_BORDER = "#2b312c";
constexpr auto C_TEXT = "#ececec";
constexpr auto C_MUTED = "#98a29a";
constexpr auto C_ACCENT = "#8bc34a";
constexpr auto C_ACCENT_DARK = "#5d8a2c";
constexpr auto C_DIRT = "#a8743f";

// 9x9 pixel-art glyphs for the sidebar and buttons ('#' = pixel)
const char* const GLYPH_HOME[] = { "....#....", "...###...", "..#####..", ".#######.", "#########",
                                   ".##...##.", ".##.#.##.", ".##.#.##.", ".#######." };
const char* const GLYPH_PROFILES[] = { "#########", "#.......#", "#########", ".........", "#########",
                                       "#.......#", "#########", ".........", "#########" };
const char* const GLYPH_MODS[] = { "...###...", ".##...##.", "#.......#", "##.....##", "#.##.##.#",
                                   "#...#...#", "#...#...#", ".##.#.##.", "...###..." };
const char* const GLYPH_FOLDER[] = { ".........", "####.....", "#...####.", "#.......#", "#########",
                                     "#.......#", "#.......#", "#.......#", "#########" };
const char* const GLYPH_ACCOUNT[] = { "..#####..", "..#...#..", "..#...#..", "..#####..", ".........",
                                      ".#######.", "#.......#", "#.......#", "#########" };
const char* const GLYPH_GEAR[] = { "....#....", ".#.###.#.", "..#####..", ".##...##.", "###...###",
                                   ".##...##.", "..#####..", ".#.###.#.", "....#...." };
const char* const GLYPH_HELP[] = { "..#####..", ".##...##.", ".......##", "......##.", "....##...",
                                   "....##...", ".........", "....##...", "....##..." };
const char* const GLYPH_PLAY[] = { ".##......", ".####....", ".######..", ".########", ".########",
                                   ".######..", ".####....", ".##......", "........." };
const char* const GLYPH_STOP[] = { ".........", ".#######.", ".#######.", ".#######.", ".#######.",
                                   ".#######.", ".#######.", ".#######.", "........." };
const char* const GLYPH_PLUS[] = { ".........", "....#....", "....#....", "....#....", ".#######.",
                                   "....#....", "....#....", "....#....", "........." };
const char* const GLYPH_DIAMOND[] = { ".........", "....#....", "...###...", "..#####..", ".#######.",
                                      "..#####..", "...###...", "....#....", "........." };

QPixmap glyph(const char* const rows[9], const QColor& color, int scale)
{
    QPixmap pix(9 * scale, 9 * scale);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            if (rows[y][x] == '#') {
                p.fillRect(x * scale, y * scale, scale, scale, color);
            }
        }
    }
    return pix;
}

QIcon glyphIcon(const char* const rows[9], int scale = 2)
{
    QIcon icon;
    icon.addPixmap(glyph(rows, QColor(C_MUTED), scale), QIcon::Normal, QIcon::Off);
    icon.addPixmap(glyph(rows, QColor(C_ACCENT), scale), QIcon::Normal, QIcon::On);
    icon.addPixmap(glyph(rows, QColor(C_ACCENT), scale), QIcon::Active, QIcon::Off);
    icon.addPixmap(glyph(rows, QColor(C_ACCENT), scale), QIcon::Active, QIcon::On);
    return icon;
}

const char* const* glyphByName(const QString& name)
{
    if (name == "home")
        return GLYPH_HOME;
    if (name == "profiles")
        return GLYPH_PROFILES;
    if (name == "mods")
        return GLYPH_MODS;
    if (name == "folder")
        return GLYPH_FOLDER;
    if (name == "account")
        return GLYPH_ACCOUNT;
    if (name == "gear")
        return GLYPH_GEAR;
    return GLYPH_HELP;
}

QString taroStyleSheet(const QString& pixel, const QString& pixelBold)
{
    return QString(R"(
#taroHome { background: %1; }
#taroHome QLabel { color: %5; background: transparent; }
#taroSidebar { background: %2; border-right: 1px solid %4; }
#taroSidebar QToolButton { background: transparent; border: 1px solid transparent; border-radius: 0px; padding: 10px; }
#taroSidebar QToolButton:hover { background: %3; border: 1px solid %4; }
#taroSidebar QToolButton:checked { background: %3; border: 1px solid %7; }
#taroTopBar { background: transparent; }
QLabel#taroBrand { font-family: '%9'; font-size: 20px; color: %5; letter-spacing: 2px; }
QLabel#taroBrandSub { font-family: '%8'; font-size: 11px; color: %6; }
#taroPanel { background: %2; border: 1px solid %4; }
QLabel#taroSection { font-family: '%9'; font-size: 13px; color: %5; letter-spacing: 1px; }
#taroSectionMark { background: %7; }
#taroCard { background: %2; border: 1px solid %4; }
#taroCard:hover { background: %3; border: 1px solid #3b443d; }
#taroCardFeatured { border: 1px solid %4; }
#taroCardFeatured:hover { border: 1px solid #3b443d; }
QLabel#taroCardTitle { font-family: '%9'; font-size: 15px; color: %5; }
QLabel#taroCardTitleBig { font-family: '%9'; font-size: 22px; color: #ffffff; }
QLabel#taroCardSub { font-family: '%8'; font-size: 12px; color: %6; }
QLabel#taroCardSubBig { font-family: '%8'; font-size: 13px; color: #d8dcd9; }
QPushButton#taroPlay { font-family: '%9'; font-size: 15px; color: %7; background: rgba(139,195,74,0.08);
    border: 2px solid %7; padding: 8px 16px; min-width: 96px; }
QPushButton#taroPlay:hover { background: %7; color: #0e100f; }
QPushButton#taroPlay:disabled { color: %6; border-color: %4; background: transparent; }
QPushButton#taroPlayBig { font-family: '%9'; font-size: 19px; color: %7; background: rgba(14,16,15,0.75);
    border: 2px solid %7; padding: 12px 20px; min-width: 120px; }
QPushButton#taroPlayBig:hover { background: %7; color: #0e100f; }
QPushButton#taroStop { font-family: '%9'; font-size: 15px; color: #e57373; background: rgba(229,115,115,0.08);
    border: 2px solid #e57373; padding: 8px 16px; min-width: 96px; }
QPushButton#taroStop:hover { background: #e57373; color: #0e100f; }
QPushButton#taroIconButton { background: %2; border: 1px solid %4; padding: 10px; }
QPushButton#taroIconButton:hover { background: %3; border: 1px solid %7; }
QPushButton#taroWide { font-family: '%9'; font-size: 13px; color: %7; background: %2; border: 1px solid %4;
    padding: 12px; letter-spacing: 1px; }
QPushButton#taroWide:hover { background: %3; border: 1px solid %7; }
QPushButton#taroWideAlt { font-family: '%9'; font-size: 13px; color: %5; background: %2; border: 1px solid %4;
    padding: 12px; letter-spacing: 1px; }
QPushButton#taroWideAlt:hover { background: %3; border: 1px solid %5; }
QToolButton#taroAccount { font-family: '%9'; font-size: 13px; color: %5; background: %2; border: 1px solid %4;
    padding: 6px 12px; }
QToolButton#taroAccount:hover { background: %3; border: 1px solid %7; }
QToolButton#taroAccount::menu-indicator { image: none; }
#taroNewsCard { background: %2; border: 1px solid %4; }
#taroNewsCard:hover { background: %3; border: 1px solid #3b443d; }
QLabel#taroNewsTag { font-family: '%9'; font-size: 10px; color: #0e100f; background: %10; padding: 3px 8px; }
QLabel#taroNewsTitle { font-family: '%9'; font-size: 14px; color: %5; }
QLabel#taroNewsDate { font-family: '%8'; font-size: 12px; color: %6; }
QLabel#taroStatus { font-family: '%8'; font-size: 12px; color: %6; }
QLabel#taroEmpty { font-family: '%8'; font-size: 13px; color: %6; }
QScrollArea { background: transparent; border: none; }
QScrollArea > QWidget > QWidget { background: transparent; }
QScrollBar:vertical { background: transparent; width: 8px; }
QScrollBar::handle:vertical { background: %4; min-height: 30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)")
        .arg(C_BG, C_PANEL, C_PANEL_HOVER, C_BORDER, C_TEXT, C_MUTED, C_ACCENT, pixel, pixelBold, C_DIRT);
}

QLabel* sectionTitle(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text.toUpper(), parent);
    label->setObjectName("taroSection");
    return label;
}

QWidget* sectionHeader(const QString& text, QWidget* right, QWidget* parent)
{
    auto* w = new QWidget(parent);
    auto* l = new QHBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(10);
    auto* mark = new QFrame(w);
    mark->setObjectName("taroSectionMark");
    mark->setFixedSize(8, 8);
    l->addWidget(mark);
    l->addWidget(sectionTitle(text, w));
    l->addStretch(1);
    if (right) {
        l->addWidget(right);
    }
    return w;
}

void clearLayout(QLayout* layout)
{
    while (auto* item = layout->takeAt(0)) {
        if (auto* w = item->widget()) {
            w->hide();
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete item;
    }
}

}  // namespace

// ---------------------------------------------------------------------------------------------------------------

QString TaroHomeWidget::pixelFontFamily(bool bold)
{
    static QString regular;
    static QString heavy;
    static bool loaded = false;
    if (!loaded) {
        loaded = true;
        int id = QFontDatabase::addApplicationFont(":/taro/fonts/Monocraft.ttf");
        if (id >= 0 && !QFontDatabase::applicationFontFamilies(id).isEmpty()) {
            regular = QFontDatabase::applicationFontFamilies(id).first();
        }
        id = QFontDatabase::addApplicationFont(":/taro/fonts/Monocraft-Bold.ttf");
        if (id >= 0 && !QFontDatabase::applicationFontFamilies(id).isEmpty()) {
            heavy = QFontDatabase::applicationFontFamilies(id).first();
        }
    }
    return bold ? (heavy.isEmpty() ? regular : heavy) : regular;
}

QString TaroHomeWidget::relativeTime(qint64 msSinceEpoch)
{
    if (msSinceEpoch <= 0) {
        return tr("Nunca jugado");
    }
    const qint64 secs = QDateTime::fromMSecsSinceEpoch(msSinceEpoch).secsTo(QDateTime::currentDateTime());
    if (secs < 60) {
        return tr("Hace un momento");
    }
    if (secs < 3600) {
        const auto n = secs / 60;
        return n == 1 ? tr("Hace 1 minuto") : tr("Hace %1 minutos").arg(n);
    }
    if (secs < 86400) {
        const auto n = secs / 3600;
        return n == 1 ? tr("Hace 1 hora") : tr("Hace %1 horas").arg(n);
    }
    const auto days = secs / 86400;
    if (days < 30) {
        return days == 1 ? tr("Ayer") : tr("Hace %1 días").arg(days);
    }
    const auto months = days / 30;
    if (months < 12) {
        return months == 1 ? tr("Hace 1 mes") : tr("Hace %1 meses").arg(months);
    }
    const auto years = days / 365;
    return years <= 1 ? tr("Hace 1 año") : tr("Hace %1 años").arg(years);
}

TaroHomeWidget::TaroHomeWidget(QWidget* parent) : QWidget(parent)
{
    setObjectName("taroHome");
    setAttribute(Qt::WA_StyledBackground, true);
    QString pixel = pixelFontFamily(false);
    QString pixelBold = pixelFontFamily(true);
    if (pixel.isEmpty()) {
        pixel = font().family();
    }
    if (pixelBold.isEmpty()) {
        pixelBold = pixel;
    }
    setStyleSheet(taroStyleSheet(pixel, pixelBold));

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    root->addWidget(buildSidebar());

    auto* content = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(24, 18, 24, 18);
    contentLayout->setSpacing(16);
    contentLayout->addWidget(buildTopBar());

    auto* columns = new QHBoxLayout();
    columns->setSpacing(20);

    // ---- left column: hero + profiles
    auto* left = new QVBoxLayout();
    left->setSpacing(14);
    m_hero = new TaroHeroWidget(content);
    connect(m_hero, &TaroHeroWidget::clicked, this, &TaroHomeWidget::accountsRequested);
    left->addWidget(m_hero, 4);

    left->addWidget(sectionHeader(tr("Últimos perfiles jugados"), nullptr, content));

    auto* profilesHolder = new QWidget(content);
    m_profilesLayout = new QVBoxLayout(profilesHolder);
    m_profilesLayout->setContentsMargins(0, 0, 0, 0);
    m_profilesLayout->setSpacing(10);
    auto* profilesScroll = new QScrollArea(content);
    profilesScroll->setWidgetResizable(true);
    profilesScroll->setFrameShape(QFrame::NoFrame);
    profilesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    profilesScroll->setWidget(profilesHolder);
    left->addWidget(profilesScroll, 5);

    m_status = new QLabel(content);
    m_status->setObjectName("taroStatus");
    m_status->hide();
    left->addWidget(m_status);

    auto* bottom = new QHBoxLayout();
    bottom->setSpacing(14);
    auto* manage = new QPushButton("  " + tr("Administrar todos los perfiles").toUpper(), content);
    manage->setObjectName("taroWide");
    manage->setIcon(glyphIcon(GLYPH_PROFILES));
    manage->setCursor(Qt::PointingHandCursor);
    connect(manage, &QPushButton::clicked, this, &TaroHomeWidget::manageProfilesRequested);
    auto* mods = new QPushButton("  " + tr("Agregar modificaciones").toUpper(), content);
    mods->setObjectName("taroWideAlt");
    mods->setIcon(QIcon(glyph(GLYPH_PLUS, QColor(C_TEXT), 2)));
    mods->setCursor(Qt::PointingHandCursor);
    connect(mods, &QPushButton::clicked, this, &TaroHomeWidget::addModsRequested);
    bottom->addWidget(manage, 1);
    bottom->addWidget(mods, 1);
    left->addLayout(bottom);

    // ---- right column: news + quick actions
    auto* rightWidget = new QWidget(content);
    rightWidget->setMinimumWidth(250);
    rightWidget->setMaximumWidth(420);
    auto* right = new QVBoxLayout(rightWidget);
    right->setContentsMargins(0, 0, 0, 0);
    right->setSpacing(12);
    right->addWidget(sectionHeader(tr("Últimas noticias"), nullptr, rightWidget));
    auto* newsHolder = new QWidget(rightWidget);
    m_newsLayout = new QVBoxLayout(newsHolder);
    m_newsLayout->setContentsMargins(0, 0, 0, 0);
    m_newsLayout->setSpacing(10);
    auto* newsScroll = new QScrollArea(rightWidget);
    newsScroll->setWidgetResizable(true);
    newsScroll->setFrameShape(QFrame::NoFrame);
    newsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    newsScroll->setWidget(newsHolder);
    right->addWidget(newsScroll, 1);

    auto* accounts = new QPushButton(tr("Gestionar cuentas").toUpper(), rightWidget);
    accounts->setObjectName("taroWideAlt");
    accounts->setCursor(Qt::PointingHandCursor);
    connect(accounts, &QPushButton::clicked, this, &TaroHomeWidget::accountsRequested);
    auto* folders = new QPushButton(tr("Abrir carpeta del launcher").toUpper(), rightWidget);
    folders->setObjectName("taroWideAlt");
    folders->setCursor(Qt::PointingHandCursor);
    connect(folders, &QPushButton::clicked, this, &TaroHomeWidget::foldersRequested);
    right->addWidget(accounts);
    right->addWidget(folders);

    columns->addLayout(left, 3);
    columns->addWidget(rightWidget, 1);
    contentLayout->addLayout(columns, 1);
    root->addWidget(content, 1);

    setNews({});
    setProfiles({});
}

QToolButton* TaroHomeWidget::sideButton(const QString& glyphName, const QString& tooltip, bool active)
{
    auto* b = new QToolButton(this);
    b->setIcon(glyphIcon(glyphByName(glyphName), 3));
    b->setIconSize({ 27, 27 });
    b->setToolTip(tooltip);
    b->setCheckable(active);
    b->setChecked(active);
    b->setCursor(Qt::PointingHandCursor);
    b->setFixedSize(52, 52);
    return b;
}

QWidget* TaroHomeWidget::buildSidebar()
{
    auto* bar = new QFrame(this);
    bar->setObjectName("taroSidebar");
    bar->setFixedWidth(76);
    auto* l = new QVBoxLayout(bar);
    l->setContentsMargins(12, 18, 12, 18);
    l->setSpacing(10);

    auto* home = sideButton("home", tr("Inicio"), true);
    connect(home, &QToolButton::clicked, home, [home] { home->setChecked(true); });
    auto* profiles = sideButton("profiles", tr("Perfiles"));
    connect(profiles, &QToolButton::clicked, this, &TaroHomeWidget::manageProfilesRequested);
    auto* mods = sideButton("mods", tr("Mods"));
    connect(mods, &QToolButton::clicked, this, &TaroHomeWidget::addModsRequested);
    auto* account = sideButton("account", tr("Cuentas"));
    connect(account, &QToolButton::clicked, this, &TaroHomeWidget::accountsRequested);
    auto* folder = sideButton("folder", tr("Carpetas"));
    connect(folder, &QToolButton::clicked, this, &TaroHomeWidget::foldersRequested);
    auto* settings = sideButton("gear", tr("Ajustes"));
    connect(settings, &QToolButton::clicked, this, &TaroHomeWidget::settingsRequested);
    auto* help = sideButton("help", tr("Ayuda"));
    connect(help, &QToolButton::clicked, this, &TaroHomeWidget::helpRequested);

    for (auto* b : { home, profiles, mods, account, folder }) {
        l->addWidget(b, 0, Qt::AlignHCenter);
    }
    l->addStretch(1);
    l->addWidget(settings, 0, Qt::AlignHCenter);
    l->addWidget(help, 0, Qt::AlignHCenter);
    return bar;
}

QWidget* TaroHomeWidget::buildTopBar()
{
    auto* bar = new QWidget(this);
    bar->setObjectName("taroTopBar");
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(12);

    auto* logo = new QLabel(bar);
    logo->setPixmap(QPixmap(":/taro/logo.png").scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setFixedSize(44, 44);
    l->addWidget(logo);

    auto* names = new QVBoxLayout();
    names->setSpacing(0);
    auto* brand = new QLabel("TAROLAUNCHER", bar);
    brand->setObjectName("taroBrand");
    auto* sub = new QLabel(tr("Minecraft · TaroModpack"), bar);
    sub->setObjectName("taroBrandSub");
    names->addWidget(brand);
    names->addWidget(sub);
    l->addLayout(names);
    l->addStretch(1);

    m_accountButton = new QToolButton(bar);
    m_accountButton->setObjectName("taroAccount");
    m_accountButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_accountButton->setIconSize({ 28, 28 });
    m_accountButton->setCursor(Qt::PointingHandCursor);
    m_accountButton->setPopupMode(QToolButton::InstantPopup);
    connect(m_accountButton, &QToolButton::clicked, this, [this] {
        if (!m_accountMenu) {
            emit accountsRequested();
        }
    });
    l->addWidget(m_accountButton);
    return bar;
}

void TaroHomeWidget::setAccountMenu(QMenu* menu)
{
    m_accountMenu = menu;
    m_accountButton->setMenu(menu);
}

void TaroHomeWidget::setPlayer(const QString& name, const QPixmap& face)
{
    const QString shown = name.isEmpty() ? tr("Agregar cuenta") : name;
    m_accountButton->setText(QString("%1  ▾").arg(shown.toUpper()));
    if (face.isNull()) {
        m_accountButton->setIcon(glyphIcon(GLYPH_ACCOUNT, 3));
    } else {
        m_accountButton->setIcon(QIcon(face.scaled(28, 28, Qt::KeepAspectRatio, Qt::FastTransformation)));
    }
    m_hero->setPlayer(name, face);
}

QWidget* TaroHomeWidget::buildProfileCard(const TaroHomeProfile& profile, bool featured)
{
    auto* card = new QFrame();
    card->setObjectName(featured ? "taroCardFeatured" : "taroCard");
    auto* l = new QHBoxLayout(card);
    l->setContentsMargins(featured ? 18 : 14, featured ? 16 : 10, featured ? 18 : 14, featured ? 16 : 10);
    l->setSpacing(featured ? 18 : 14);

    if (featured) {
        // dark green → dirt gradient background for the main profile
        card->setStyleSheet(QString("#taroCardFeatured { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1d2a17, "
                                    "stop:0.6 #18201a, stop:1 %1); }")
                                .arg(C_PANEL));
    }

    const int iconSize = featured ? 64 : 44;
    auto* icon = new QLabel(card);
    icon->setFixedSize(iconSize, iconSize);
    if (!profile.icon.isNull()) {
        icon->setPixmap(profile.icon.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    l->addWidget(icon);

    auto* texts = new QVBoxLayout();
    texts->setSpacing(featured ? 6 : 3);
    auto* title = new QLabel(profile.name.toUpper(), card);
    title->setObjectName(featured ? "taroCardTitleBig" : "taroCardTitle");
    title->setMinimumWidth(40);
    title->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    texts->addWidget(title);

    auto* subRow = new QHBoxLayout();
    subRow->setSpacing(6);
    auto* diamond = new QLabel(card);
    diamond->setPixmap(glyph(GLYPH_DIAMOND, QColor(C_DIRT), featured ? 2 : 1));
    subRow->addWidget(diamond);
    QStringList parts;
    QString loaderVersion = profile.loader;
    if (!profile.gameVersion.isEmpty()) {
        loaderVersion = (loaderVersion.isEmpty() ? QString("Minecraft") : loaderVersion) + " " + profile.gameVersion;
    }
    if (!loaderVersion.isEmpty()) {
        parts << loaderVersion;
    }
    parts << relativeTime(profile.lastLaunch);
    auto* sub = new QLabel(parts.join("  ·  "), card);
    sub->setObjectName(featured ? "taroCardSubBig" : "taroCardSub");
    sub->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    subRow->addWidget(sub, 1);
    subRow->addStretch(1);
    texts->addLayout(subRow);
    l->addLayout(texts, 1);

    const QString id = profile.id;
    QPushButton* play;
    if (profile.running) {
        play = new QPushButton("  " + tr("Detener").toUpper(), card);
        play->setObjectName("taroStop");
        play->setIcon(QIcon(glyph(GLYPH_STOP, QColor("#e57373"), 2)));
        connect(play, &QPushButton::clicked, this, [this, id] { emit killRequested(id); });
    } else {
        play = new QPushButton("  " + tr("Jugar").toUpper(), card);
        play->setObjectName(featured ? "taroPlayBig" : "taroPlay");
        play->setIcon(glyphIcon(GLYPH_PLAY, featured ? 2 : 2));
        play->setEnabled(profile.canLaunch);
        connect(play, &QPushButton::clicked, this, [this, id] { emit launchRequested(id); });
    }
    play->setCursor(Qt::PointingHandCursor);
    l->addWidget(play);

    auto* gear = new QPushButton(card);
    gear->setObjectName("taroIconButton");
    gear->setIcon(QIcon(glyph(GLYPH_GEAR, QColor(C_TEXT), 2)));
    gear->setIconSize({ 18, 18 });
    gear->setToolTip(tr("Editar perfil"));
    gear->setCursor(Qt::PointingHandCursor);
    connect(gear, &QPushButton::clicked, this, [this, id] { emit editRequested(id); });
    l->addWidget(gear);
    return card;
}

void TaroHomeWidget::setProfiles(const QList<TaroHomeProfile>& profiles)
{
    clearLayout(m_profilesLayout);
    if (profiles.isEmpty()) {
        auto* empty = new QLabel(tr("Todavía no hay perfiles. TaroModpack se descarga automáticamente la primera vez."));
        empty->setObjectName("taroEmpty");
        empty->setWordWrap(true);
        m_profilesLayout->addWidget(empty);
    }
    bool first = true;
    for (const auto& profile : profiles) {
        m_profilesLayout->addWidget(buildProfileCard(profile, first));
        first = false;
    }
    m_profilesLayout->addStretch(1);
}

QWidget* TaroHomeWidget::buildNewsCard(const TaroHomeNews& news)
{
    auto* card = new QPushButton();
    card->setObjectName("taroNewsCard");
    card->setFlat(true);
    card->setCursor(news.url.isEmpty() ? Qt::ArrowCursor : Qt::PointingHandCursor);
    auto* l = new QVBoxLayout(card);
    l->setContentsMargins(16, 14, 16, 14);
    l->setSpacing(8);
    if (!news.tag.isEmpty()) {
        auto* tagRow = new QHBoxLayout();
        auto* tag = new QLabel(news.tag.toUpper(), card);
        tag->setObjectName("taroNewsTag");
        tagRow->addWidget(tag);
        tagRow->addStretch(1);
        l->addLayout(tagRow);
    }
    auto* title = new QLabel(news.title.toUpper(), card);
    title->setObjectName("taroNewsTitle");
    title->setWordWrap(true);
    l->addWidget(title);
    if (!news.date.isEmpty()) {
        auto* date = new QLabel(news.date, card);
        date->setObjectName("taroNewsDate");
        l->addWidget(date);
    }
    card->setMinimumHeight(l->sizeHint().height());
    const QString url = news.url;
    connect(card, &QPushButton::clicked, this, [this, url] {
        if (!url.isEmpty()) {
            emit newsClicked(url);
        }
    });
    return card;
}

void TaroHomeWidget::setNews(const QList<TaroHomeNews>& news)
{
    clearLayout(m_newsLayout);
    if (news.isEmpty()) {
        auto* empty = new QLabel(tr("No hay noticias por ahora."));
        empty->setObjectName("taroEmpty");
        m_newsLayout->addWidget(empty);
    }
    for (const auto& item : news) {
        m_newsLayout->addWidget(buildNewsCard(item));
    }
    m_newsLayout->addStretch(1);
}

void TaroHomeWidget::setStatusText(const QString& text)
{
    m_status->setText(text);
    m_status->setVisible(!text.isEmpty());
}

// ---------------------------------------------------------------------------------------------------------------

TaroHeroWidget::TaroHeroWidget(QWidget* parent) : QWidget(parent)
{
    m_banner = QPixmap(":/taro/banner.png");
    m_logo = QPixmap(":/taro/logo.png");
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TaroHeroWidget::setPlayer(const QString& name, const QPixmap& face)
{
    m_name = name;
    m_face = face;
    update();
}

void TaroHeroWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void TaroHeroWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    const QRect r = rect();

    // banner, "cover" scaled and anchored to the bottom so the ground stays visible
    if (!m_banner.isNull()) {
        const QSize scaled = m_banner.size().scaled(r.size(), Qt::KeepAspectRatioByExpanding);
        const QRect target(r.center().x() - scaled.width() / 2, r.bottom() - scaled.height() + 1, scaled.width(), scaled.height());
        p.drawPixmap(target, m_banner);
    } else {
        p.fillRect(r, QColor("#3d7fd4"));
    }

    // soft dark vignette at the bottom
    QLinearGradient shade(r.topLeft(), r.bottomLeft());
    shade.setColorAt(0.0, QColor(0, 0, 0, 0));
    shade.setColorAt(0.65, QColor(0, 0, 0, 30));
    shade.setColorAt(1.0, QColor(0, 0, 0, 120));
    p.fillRect(r, shade);

    // player face (pixel art scaled without smoothing) or the Taro logo when there is no account
    const int size = qBound(72, int(r.height() * 0.42), 150);
    const QRect faceRect(r.center().x() - size / 2, r.center().y() - size / 2 + 14, size, size);
    p.setRenderHint(QPainter::SmoothPixmapTransform, m_face.isNull());
    p.fillRect(faceRect.translated(6, 6), QColor(0, 0, 0, 90));
    if (!m_face.isNull()) {
        p.drawPixmap(faceRect, m_face);
    } else if (!m_logo.isNull()) {
        p.drawPixmap(faceRect, m_logo);
    }
    p.setPen(QPen(QColor(255, 255, 255, 200), 3));
    p.setBrush(Qt::NoBrush);
    p.drawRect(faceRect.adjusted(-2, -2, 1, 1));

    // name tag above the face, like a Minecraft nametag
    p.setRenderHint(QPainter::Antialiasing, true);
    QFont nameFont(TaroHomeWidget::pixelFontFamily(true));
    nameFont.setPixelSize(20);
    QFont subFont(TaroHomeWidget::pixelFontFamily(false));
    subFont.setPixelSize(11);
    const QString name = m_name.isEmpty() ? tr("Sin cuenta") : m_name;
    const QString sub = m_name.isEmpty() ? tr("→ Agregar una cuenta") : tr("→ Cambiar de cuenta");
    const int w = qMax(QFontMetrics(nameFont).horizontalAdvance(name), QFontMetrics(subFont).horizontalAdvance(sub)) + 36;
    const int h = 54;
    const QRect tag(r.center().x() - w / 2, faceRect.top() - h - 16, w, h);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(20, 20, 20, 170));
    p.drawRoundedRect(tag, 4, 4);
    p.setPen(Qt::white);
    p.setFont(nameFont);
    p.drawText(tag.adjusted(0, 6, 0, -22), Qt::AlignCenter, name);
    p.setPen(QColor(220, 220, 220));
    p.setFont(subFont);
    p.drawText(tag.adjusted(0, 30, 0, -4), Qt::AlignCenter, sub);

    // thin border
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(QColor(C_BORDER));
    p.setBrush(Qt::NoBrush);
    p.drawRect(r.adjusted(0, 0, -1, -1));
}
