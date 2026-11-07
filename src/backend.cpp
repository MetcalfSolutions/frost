#include "backend.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

Backend::Backend(const QString &configFile, const QString &hyprctl, QObject *parent)
    : QObject(parent),
      m_configFile(configFile.isEmpty() ? QDir::homePath() + "/.config/hypr/frost.lua" : configFile),
      m_hyprctl(hyprctl.isEmpty() ? QStandardPaths::findExecutable("hyprctl") : hyprctl) {
    m_saveTimer.setSingleShot(true);
    m_saveTimer.setInterval(400);
    connect(&m_saveTimer, &QTimer::timeout, this, [this] {
        if (saveToFile()) {
            m_statusMessage = tr("Saved to config");
            emit statusMessageChanged();
        }
    });

    loadFromHyprland();
}

void Backend::loadFromHyprland() {
    if (m_hyprctl.isEmpty())
        return;

    auto queryOption = [this](const QString &opt) -> QJsonValue {
        QProcess proc;
        proc.start(m_hyprctl, {"-j", "getoption", opt});
        if (proc.waitForFinished(500) && proc.exitCode() == 0) {
            return QJsonDocument::fromJson(proc.readAllStandardOutput()).object().value("float");
        }
        return {};
    };

    auto queryIntOption = [this](const QString &opt) -> int {
        QProcess proc;
        proc.start(m_hyprctl, {"-j", "getoption", opt});
        if (proc.waitForFinished(500) && proc.exitCode() == 0) {
            return QJsonDocument::fromJson(proc.readAllStandardOutput()).object().value("int").toInt(-1);
        }
        return -1;
    };

    const auto act = queryOption("decoration:active_opacity");
    if (act.isDouble() && act.toDouble() > 0.0)
        m_activeOpacity = act.toDouble();

    const auto inact = queryOption("decoration:inactive_opacity");
    if (inact.isDouble() && inact.toDouble() > 0.0)
        m_inactiveOpacity = inact.toDouble();

    const int size = queryIntOption("decoration:blur:size");
    if (size > 0)
        m_blurSize = size;

    const int passes = queryIntOption("decoration:blur:passes");
    if (passes > 0)
        m_blurPasses = passes;

    const auto noise = queryOption("decoration:blur:noise");
    if (noise.isDouble())
        m_blurNoise = noise.toDouble();

    const auto contrast = queryOption("decoration:blur:contrast");
    if (contrast.isDouble())
        m_blurContrast = contrast.toDouble();

    const auto brightness = queryOption("decoration:blur:brightness");
    if (brightness.isDouble())
        m_blurBrightness = brightness.toDouble();

    const auto vibrancy = queryOption("decoration:blur:vibrancy");
    if (vibrancy.isDouble())
        m_blurVibrancy = vibrancy.toDouble();
}

void Backend::applyLive() {
    if (m_hyprctl.isEmpty())
        return;

    const QString lua = QString(
        "hl.config({ decoration = { active_opacity = %1, inactive_opacity = %2, "
        "blur = { enabled = true, size = %3, passes = %4, noise = %5, contrast = %6, brightness = %7, vibrancy = %8 } } })")
        .arg(m_activeOpacity, 0, 'f', 2)
        .arg(m_inactiveOpacity, 0, 'f', 2)
        .arg(m_blurSize)
        .arg(m_blurPasses)
        .arg(m_blurNoise, 0, 'f', 4)
        .arg(m_blurContrast, 0, 'f', 2)
        .arg(m_blurBrightness, 0, 'f', 2)
        .arg(m_blurVibrancy, 0, 'f', 2);

    QProcess proc;
    proc.start(m_hyprctl, {"eval", lua});
    proc.waitForFinished(500);

    m_saveTimer.start();
}

bool Backend::saveToFile() {
    if (m_configFile.isEmpty())
        return false;

    QFileInfo info(m_configFile);
    QDir().mkpath(info.absolutePath());

    QString existingContent;
    QFile file(m_configFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        existingContent = QString::fromUtf8(file.readAll());
        file.close();
    }

    const QString block = QString(
        "-- frost begin\n"
        "hl.config({\n"
        "  decoration = {\n"
        "    active_opacity = %1,\n"
        "    inactive_opacity = %2,\n"
        "    blur = {\n"
        "      enabled = true,\n"
        "      size = %3,\n"
        "      passes = %4,\n"
        "      noise = %5,\n"
        "      contrast = %6,\n"
        "      brightness = %7,\n"
        "      vibrancy = %8,\n"
        "    },\n"
        "  },\n"
        "})\n"
        "-- frost end\n")
        .arg(m_activeOpacity, 0, 'f', 2)
        .arg(m_inactiveOpacity, 0, 'f', 2)
        .arg(m_blurSize)
        .arg(m_blurPasses)
        .arg(m_blurNoise, 0, 'f', 4)
        .arg(m_blurContrast, 0, 'f', 2)
        .arg(m_blurBrightness, 0, 'f', 2)
        .arg(m_blurVibrancy, 0, 'f', 2);

    static const QRegularExpression blockRegex(R"re(-- frost begin[\s\S]*?-- frost end\n?)re");
    QString newContent;
    if (existingContent.contains(blockRegex)) {
        newContent = existingContent;
        newContent.replace(blockRegex, block);
    } else {
        newContent = existingContent;
        if (!newContent.isEmpty() && !newContent.endsWith('\n'))
            newContent += '\n';
        newContent += block;
    }

    const QString tempPath = m_configFile + ".tmp";
    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    if (tempFile.write(newContent.toUtf8()) == -1) {
        tempFile.close();
        tempFile.remove();
        return false;
    }
    tempFile.close();

    // Also write zz-frost.lua to toggles dir so Hyprland loads it after window-glassy.lua
    QString togglesPath = QDir::homePath() + "/.local/state/omarchy/toggles/hypr/zz-frost.lua";
    if (m_configFile.contains(".config/hypr")) {
        QFileInfo togglesInfo(togglesPath);
        QDir().mkpath(togglesInfo.absolutePath());
        QFile tf(togglesPath + ".tmp");
        if (tf.open(QIODevice::WriteOnly | QIODevice::Text)) {
            tf.write(block.toUtf8());
            tf.close();
            QFile::remove(togglesPath);
            QFile::rename(togglesPath + ".tmp", togglesPath);
        }
    }

    return QFile::rename(tempPath, m_configFile) || (QFile::remove(m_configFile) && QFile::rename(tempPath, m_configFile));
}

void Backend::saveNow() {
    m_saveTimer.stop();
    if (saveToFile()) {
        m_statusMessage = tr("Saved to config");
        emit statusMessageChanged();
    }
}

void Backend::setActiveOpacity(double val) {
    val = qBound(0.20, val, 1.00);
    if (qFuzzyCompare(m_activeOpacity, val))
        return;
    m_activeOpacity = val;
    emit activeOpacityChanged();

    if (m_syncOpacity) {
        m_inactiveOpacity = val;
        emit inactiveOpacityChanged();
    }
    applyLive();
}

void Backend::setInactiveOpacity(double val) {
    val = qBound(0.20, val, 1.00);
    if (qFuzzyCompare(m_inactiveOpacity, val))
        return;
    m_inactiveOpacity = val;
    emit inactiveOpacityChanged();

    if (m_syncOpacity) {
        m_activeOpacity = val;
        emit activeOpacityChanged();
    }
    applyLive();
}

void Backend::setSyncOpacity(bool sync) {
    if (m_syncOpacity == sync)
        return;
    m_syncOpacity = sync;
    emit syncOpacityChanged();
    if (m_syncOpacity) {
        m_inactiveOpacity = m_activeOpacity;
        emit inactiveOpacityChanged();
        applyLive();
    }
}

void Backend::setBlurSize(int val) {
    val = qBound(1, val, 15);
    if (m_blurSize == val)
        return;
    m_blurSize = val;
    emit blurSizeChanged();
    applyLive();
}

void Backend::setBlurPasses(int val) {
    val = qBound(1, val, 4);
    if (m_blurPasses == val)
        return;
    m_blurPasses = val;
    emit blurPassesChanged();
    applyLive();
}

void Backend::setBlurNoise(double val) {
    val = qBound(0.00, val, 0.10);
    if (qFuzzyCompare(m_blurNoise, val))
        return;
    m_blurNoise = val;
    emit blurNoiseChanged();
    applyLive();
}

void Backend::setBlurContrast(double val) {
    val = qBound(0.50, val, 2.00);
    if (qFuzzyCompare(m_blurContrast, val))
        return;
    m_blurContrast = val;
    emit blurContrastChanged();
    applyLive();
}

void Backend::setBlurBrightness(double val) {
    val = qBound(0.50, val, 1.50);
    if (qFuzzyCompare(m_blurBrightness, val))
        return;
    m_blurBrightness = val;
    emit blurBrightnessChanged();
    applyLive();
}

void Backend::setBlurVibrancy(double val) {
    val = qBound(0.00, val, 1.00);
    if (qFuzzyCompare(m_blurVibrancy, val))
        return;
    m_blurVibrancy = val;
    emit blurVibrancyChanged();
    applyLive();
}

void Backend::resetDefaults() {
    m_activeOpacity = 1.0;
    m_inactiveOpacity = 1.0;
    m_syncOpacity = false;
    m_blurSize = 4;
    m_blurPasses = 2;
    m_blurNoise = 0.01;
    m_blurContrast = 1.00;
    m_blurBrightness = 0.75;
    m_blurVibrancy = 0.35;

    emit activeOpacityChanged();
    emit inactiveOpacityChanged();
    emit syncOpacityChanged();
    emit blurSizeChanged();
    emit blurPassesChanged();
    emit blurNoiseChanged();
    emit blurContrastChanged();
    emit blurBrightnessChanged();
    emit blurVibrancyChanged();

    applyLive();
    saveNow();
    m_statusMessage = tr("Reset to defaults");
    emit statusMessageChanged();
}
