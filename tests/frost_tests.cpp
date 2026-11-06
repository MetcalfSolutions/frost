#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include "backend.h"
#include "theme.h"
#include "translator.h"

class FrostTests : public QObject {
    Q_OBJECT

private slots:
    void backendOpacityAndBlurSetters()
    {
        QTemporaryDir dir;
        const QString conf = dir.filePath("looknfeel.lua");
        Backend backend(conf, ""); // pass empty hyprctl so it doesn't call real compositor in test

        backend.setActiveOpacity(0.85);
        QCOMPARE(backend.activeOpacity(), 0.85);

        backend.setInactiveOpacity(0.70);
        QCOMPARE(backend.inactiveOpacity(), 0.70);

        backend.setBlurSize(8);
        QCOMPARE(backend.blurSize(), 8);

        backend.setBlurPasses(3);
        QCOMPARE(backend.blurPasses(), 3);

        backend.setSyncOpacity(true);
        QCOMPARE(backend.syncOpacity(), true);
        backend.setActiveOpacity(0.92);
        QCOMPARE(backend.inactiveOpacity(), 0.92);
    }

    void backendSavesToFile()
    {
        QTemporaryDir dir;
        const QString conf = dir.filePath("looknfeel.lua");
        Backend backend(conf, "");

        backend.setActiveOpacity(0.88);
        backend.setBlurSize(6);
        QVERIFY(backend.saveToFile());

        QFile file(conf);
        QVERIFY(file.open(QIODevice::ReadOnly));
        const QString content = QString::fromUtf8(file.readAll());
        QVERIFY(content.contains("-- frost begin"));
        QVERIFY(content.contains("active_opacity = 0.88"));
        QVERIFY(content.contains("size = 6"));
        QVERIFY(content.contains("-- frost end"));
    }

    void backendResetDefaults()
    {
        QTemporaryDir dir;
        const QString conf = dir.filePath("looknfeel.lua");
        Backend backend(conf, "");

        backend.setActiveOpacity(0.50);
        backend.setBlurSize(12);
        backend.resetDefaults();

        QCOMPARE(backend.activeOpacity(), 1.0);
        QCOMPARE(backend.inactiveOpacity(), 1.0);
        QCOMPARE(backend.blurSize(), 4);
        QCOMPARE(backend.blurPasses(), 2);
    }

    void themeFallsBack()
    {
        QTemporaryDir dir;
        Theme theme(dir.path());
        QCOMPARE(theme.accent(), QString("#FFD60A"));
        QCOMPARE(theme.accentForeground(), QString("black"));
    }

    void themeReadsAccent()
    {
        QTemporaryDir dir;
        QDir(dir.path()).mkpath("theme");
        QFile colors(dir.path() + "/theme/colors.toml");
        QVERIFY(colors.open(QIODevice::WriteOnly));
        colors.write("accent = \"#112233\"\n");
        colors.close();

        Theme theme(dir.path());
        QCOMPARE(theme.accent(), QString("#112233"));
        QCOMPARE(theme.accentForeground(), QString("white"));
    }

    void themeFollowsHyprlandRounding()
    {
        QTemporaryDir dir;
        const QString response = dir.filePath("response.json"), command = dir.filePath("hyprctl");
        auto write = [](const QString &path, const QByteArray &data) {
            QFile file(path);
            return file.open(QIODevice::WriteOnly) && file.write(data) == data.size();
        };
        QVERIFY(write(command, "#!/bin/sh\ncat '" + response.toUtf8() + "'\n"));
        QVERIFY(QFile::setPermissions(command, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
        QVERIFY(write(response, "{\"int\":0}"));

        Theme theme(dir.path(), nullptr, command);
        QCOMPARE(theme.radius(), 0);
        QVERIFY(write(response, "{\"int\":6}"));
        QTRY_COMPARE(theme.radius(), 6);
    }

    void translatorFallsBackToEnglish()
    {
        QTemporaryDir dir;
        Translator translator(dir.path() + "/language", dir.path());
        QCOMPARE(translator.language(), QString("en"));
        QVERIFY(translator.translate("Main", "Reset").isNull());
    }

    void translatorFollowsTheLanguageFile()
    {
        QTemporaryDir dir;
        write(dir.path() + "/language", "da\n");
        write(dir.path() + "/da.json", R"({"Reset": "Nulstil", "Save": "Gem"})");

        Translator translator(dir.path() + "/language", dir.path());
        QCOMPARE(translator.language(), QString("da"));
        QCOMPARE(translator.translate("Main", "Reset"), QString("Nulstil"));
        QCOMPARE(translator.translate("Main", "Save"), QString("Gem"));
        QVERIFY(translator.translate("Main", "Quit").isNull());
    }

private:
    static void write(const QString &path, const QByteArray &contents)
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(contents);
    }
};

QTEST_MAIN(FrostTests)
#include "frost_tests.moc"
