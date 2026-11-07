// SPDX-License-Identifier: GPL-3.0-only
/*
 *  TaroLauncher - default theme.
 */

#include "TaroTheme.h"

#include <QObject>

QString TaroTheme::id()
{
    return "taro";
}

QString TaroTheme::name()
{
    return QObject::tr("TaroLauncher");
}

QString TaroTheme::tooltip()
{
    return QObject::tr("Same look as the TaroLauncher main menu");
}

QPalette TaroTheme::colorScheme()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor(0x14, 0x17, 0x15));
    p.setColor(QPalette::WindowText, QColor(0xec, 0xec, 0xec));
    p.setColor(QPalette::Base, QColor(0x0e, 0x10, 0x0f));
    p.setColor(QPalette::AlternateBase, QColor(0x18, 0x1c, 0x19));
    p.setColor(QPalette::ToolTipBase, QColor(0x16, 0x19, 0x17));
    p.setColor(QPalette::ToolTipText, QColor(0xec, 0xec, 0xec));
    p.setColor(QPalette::Text, QColor(0xec, 0xec, 0xec));
    p.setColor(QPalette::Button, QColor(0x1b, 0x1f, 0x1c));
    p.setColor(QPalette::ButtonText, QColor(0xec, 0xec, 0xec));
    p.setColor(QPalette::BrightText, QColor(0xe5, 0x73, 0x73));
    p.setColor(QPalette::Link, QColor(0x8b, 0xc3, 0x4a));
    p.setColor(QPalette::LinkVisited, QColor(0x6f, 0xa3, 0x36));
    p.setColor(QPalette::Highlight, QColor(0x8b, 0xc3, 0x4a));
    p.setColor(QPalette::HighlightedText, QColor(0x0e, 0x10, 0x0f));
    p.setColor(QPalette::PlaceholderText, QColor(0x6b, 0x73, 0x6d));
    p.setColor(QPalette::Light, QColor(0x2b, 0x31, 0x2c));
    p.setColor(QPalette::Midlight, QColor(0x22, 0x27, 0x23));
    p.setColor(QPalette::Mid, QColor(0x1b, 0x1f, 0x1c));
    p.setColor(QPalette::Dark, QColor(0x0b, 0x0d, 0x0c));
    p.setColor(QPalette::Shadow, QColor(0x00, 0x00, 0x00));
    p.setColor(QPalette::Disabled, QPalette::Text, QColor(0x6b, 0x73, 0x6d));
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x6b, 0x73, 0x6d));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x6b, 0x73, 0x6d));
    return fadeInactive(p, fadeAmount(), fadeColor());
}

double TaroTheme::fadeAmount()
{
    return 0.5;
}

QColor TaroTheme::fadeColor()
{
    return QColor(0x14, 0x17, 0x15);
}

bool TaroTheme::hasStyleSheet()
{
    return true;
}

QString TaroTheme::appStyleSheet()
{
    // Only "safe" widgets are restyled; combo boxes and spin boxes keep the Fusion look so their arrows stay intact.
    return R"(
QToolTip { color: #ececec; background-color: #161917; border: 1px solid #8bc34a; padding: 4px; }

QPushButton { background-color: #1b1f1c; color: #ececec; border: 1px solid #2f3631; border-radius: 0px; padding: 6px 14px; min-height: 18px; }
QPushButton:hover { background-color: #222823; border-color: #8bc34a; }
QPushButton:pressed, QPushButton:checked { background-color: #2b3a22; border-color: #8bc34a; }
QPushButton:default { color: #8bc34a; border-color: #8bc34a; }
QPushButton:default:hover { background-color: #8bc34a; color: #0e100f; }
QPushButton:disabled { color: #5d645f; background-color: #161917; border-color: #232824; }
QPushButton::menu-indicator { subcontrol-position: right center; right: 6px; }

QLineEdit, QPlainTextEdit, QTextEdit, QTextBrowser { background-color: #0e100f; color: #ececec; border: 1px solid #2b312c;
    selection-background-color: #8bc34a; selection-color: #0e100f; }
QLineEdit { padding: 4px 6px; }
QLineEdit:focus, QPlainTextEdit:focus, QTextEdit:focus { border-color: #8bc34a; }

QTabWidget::pane { border: 1px solid #2b312c; top: -1px; }
QTabBar::tab { background: #161917; color: #98a29a; border: 1px solid #2b312c; padding: 6px 14px; margin-right: -1px; }
QTabBar::tab:selected { background: #1b1f1c; color: #ececec; border-bottom: 2px solid #8bc34a; }
QTabBar::tab:hover:!selected { color: #ececec; }

QHeaderView::section { background-color: #161917; color: #98a29a; border: none; border-right: 1px solid #2b312c;
    border-bottom: 1px solid #2b312c; padding: 4px 6px; }

QProgressBar { background-color: #0e100f; border: 1px solid #2b312c; text-align: center; color: #ececec; min-height: 16px; }
QProgressBar::chunk { background-color: #8bc34a; }

QMenu { background-color: #161917; color: #ececec; border: 1px solid #2b312c; padding: 4px; }
QMenu::item { padding: 5px 24px 5px 24px; }
QMenu::item:selected { background-color: #2b3a22; color: #ffffff; }
QMenu::item:disabled { color: #5d645f; }
QMenu::separator { height: 1px; background: #2b312c; margin: 4px 6px; }

QToolBar { background-color: #0e100f; border: none; spacing: 4px; padding: 2px; }
QToolBar QToolButton { border: 1px solid transparent; padding: 4px; }
QToolBar QToolButton:hover { background-color: #1b1f1c; border-color: #2f3631; }
QToolBar QToolButton:checked { background-color: #2b3a22; border-color: #8bc34a; }
QStatusBar { background-color: #0e100f; color: #98a29a; }

QGroupBox { border: 1px solid #2b312c; margin-top: 14px; padding-top: 6px; }
QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; color: #8bc34a; }

QScrollBar:vertical { background: #0e100f; width: 10px; margin: 0; }
QScrollBar::handle:vertical { background: #2b312c; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: #3b443d; }
QScrollBar:horizontal { background: #0e100f; height: 10px; margin: 0; }
QScrollBar::handle:horizontal { background: #2b312c; min-width: 30px; }
QScrollBar::handle:horizontal:hover { background: #3b443d; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }

QSplitter::handle { background-color: #1b1f1c; }
)";
}
