#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(double activeOpacity READ activeOpacity WRITE setActiveOpacity NOTIFY activeOpacityChanged)
    Q_PROPERTY(double inactiveOpacity READ inactiveOpacity WRITE setInactiveOpacity NOTIFY inactiveOpacityChanged)
    Q_PROPERTY(bool syncOpacity READ syncOpacity WRITE setSyncOpacity NOTIFY syncOpacityChanged)
    Q_PROPERTY(int blurSize READ blurSize WRITE setBlurSize NOTIFY blurSizeChanged)
    Q_PROPERTY(int blurPasses READ blurPasses WRITE setBlurPasses NOTIFY blurPassesChanged)
    Q_PROPERTY(double blurNoise READ blurNoise WRITE setBlurNoise NOTIFY blurNoiseChanged)
    Q_PROPERTY(double blurContrast READ blurContrast WRITE setBlurContrast NOTIFY blurContrastChanged)
    Q_PROPERTY(double blurBrightness READ blurBrightness WRITE setBlurBrightness NOTIFY blurBrightnessChanged)
    Q_PROPERTY(double blurVibrancy READ blurVibrancy WRITE setBlurVibrancy NOTIFY blurVibrancyChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    explicit Backend(const QString &configFile = {}, const QString &hyprctl = {}, QObject *parent = nullptr);

    double activeOpacity() const { return m_activeOpacity; }
    double inactiveOpacity() const { return m_inactiveOpacity; }
    bool syncOpacity() const { return m_syncOpacity; }
    int blurSize() const { return m_blurSize; }
    int blurPasses() const { return m_blurPasses; }
    double blurNoise() const { return m_blurNoise; }
    double blurContrast() const { return m_blurContrast; }
    double blurBrightness() const { return m_blurBrightness; }
    double blurVibrancy() const { return m_blurVibrancy; }
    QString statusMessage() const { return m_statusMessage; }

    Q_INVOKABLE void setActiveOpacity(double val);
    Q_INVOKABLE void setInactiveOpacity(double val);
    Q_INVOKABLE void setSyncOpacity(bool sync);
    Q_INVOKABLE void setBlurSize(int val);
    Q_INVOKABLE void setBlurPasses(int val);
    Q_INVOKABLE void setBlurNoise(double val);
    Q_INVOKABLE void setBlurContrast(double val);
    Q_INVOKABLE void setBlurBrightness(double val);
    Q_INVOKABLE void setBlurVibrancy(double val);
    Q_INVOKABLE void resetDefaults();
    Q_INVOKABLE void saveNow();

    void applyLive();
    bool saveToFile();
    void loadFromHyprland();

signals:
    void activeOpacityChanged();
    void inactiveOpacityChanged();
    void syncOpacityChanged();
    void blurSizeChanged();
    void blurPassesChanged();
    void blurNoiseChanged();
    void blurContrastChanged();
    void blurBrightnessChanged();
    void blurVibrancyChanged();
    void statusMessageChanged();

private:
    QString m_configFile;
    QString m_hyprctl;

    double m_activeOpacity = 1.0;
    double m_inactiveOpacity = 1.0;
    bool m_syncOpacity = false;
    int m_blurSize = 4;
    int m_blurPasses = 2;
    double m_blurNoise = 0.01;
    double m_blurContrast = 1.0;
    double m_blurBrightness = 0.75;
    double m_blurVibrancy = 0.35;

    QString m_statusMessage;
    QTimer m_saveTimer;
};
