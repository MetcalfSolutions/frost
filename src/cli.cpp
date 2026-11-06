#include "cli.h"
#include "backend.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

int Cli::run(int argc, char *argv[]) {
    QString cmd = argc > 1 ? QString::fromUtf8(argv[1]) : QString();

    if (cmd == "--help" || cmd == "-h" || cmd == "help") {
        std::cout << "Usage: frost [command] [options]\n\n"
                  << "Commands:\n"
                  << "  open                        Launch the graphical window (default)\n"
                  << "  get [--json]                Print current window opacity and blur settings\n"
                  << "  set [options]               Update settings and apply live\n"
                  << "  reset                       Reset settings to Omarchy defaults\n\n"
                  << "Set Options:\n"
                  << "  --opacity <0.20-1.00>       Set both active and inactive opacity\n"
                  << "  --active-opacity <f>        Set active (focused) window opacity\n"
                  << "  --inactive-opacity <f>      Set inactive (unfocused) window opacity\n"
                  << "  --blur-size <1-15>          Set blur radius size\n"
                  << "  --blur-passes <1-4>         Set blur passes\n"
                  << "  --noise <0.00-0.10>         Set noise amount\n"
                  << "  --contrast <0.50-2.00>      Set contrast\n"
                  << "  --brightness <0.50-1.50>    Set brightness\n"
                  << "  --vibrancy <0.00-1.00>      Set vibrancy\n";
        return 0;
    }

    Backend backend;

    if (cmd == "get") {
        bool json = false;
        for (int i = 2; i < argc; ++i) {
            if (QString::fromUtf8(argv[i]) == "--json")
                json = true;
        }

        if (json) {
            QJsonObject obj{
                {"active_opacity", backend.activeOpacity()},
                {"inactive_opacity", backend.inactiveOpacity()},
                {"sync_opacity", backend.syncOpacity()},
                {"blur_size", backend.blurSize()},
                {"blur_passes", backend.blurPasses()},
                {"blur_noise", backend.blurNoise()},
                {"blur_contrast", backend.blurContrast()},
                {"blur_brightness", backend.blurBrightness()},
                {"blur_vibrancy", backend.blurVibrancy()}
            };
            std::cout << QJsonDocument(obj).toJson(QJsonDocument::Compact).toStdString() << "\n";
        } else {
            std::cout << "Active Opacity:   " << backend.activeOpacity() << "\n"
                      << "Inactive Opacity: " << backend.inactiveOpacity() << "\n"
                      << "Blur Size:        " << backend.blurSize() << "\n"
                      << "Blur Passes:      " << backend.blurPasses() << "\n"
                      << "Blur Noise:       " << backend.blurNoise() << "\n"
                      << "Blur Contrast:    " << backend.blurContrast() << "\n"
                      << "Blur Brightness:  " << backend.blurBrightness() << "\n"
                      << "Blur Vibrancy:    " << backend.blurVibrancy() << "\n";
        }
        return 0;
    }

    if (cmd == "reset") {
        backend.resetDefaults();
        std::cout << "Reset to Omarchy glass defaults.\n";
        return 0;
    }

    if (cmd == "set") {
        for (int i = 2; i < argc; ++i) {
            QString arg = QString::fromUtf8(argv[i]);
            if (i + 1 >= argc) break;
            QString val = QString::fromUtf8(argv[++i]);

            if (arg == "--opacity") {
                double op = val.toDouble();
                backend.setSyncOpacity(true);
                backend.setActiveOpacity(op);
            } else if (arg == "--active-opacity") {
                backend.setSyncOpacity(false);
                backend.setActiveOpacity(val.toDouble());
            } else if (arg == "--inactive-opacity") {
                backend.setSyncOpacity(false);
                backend.setInactiveOpacity(val.toDouble());
            } else if (arg == "--blur-size") {
                backend.setBlurSize(val.toInt());
            } else if (arg == "--blur-passes") {
                backend.setBlurPasses(val.toInt());
            } else if (arg == "--noise") {
                backend.setBlurNoise(val.toDouble());
            } else if (arg == "--contrast") {
                backend.setBlurContrast(val.toDouble());
            } else if (arg == "--brightness") {
                backend.setBlurBrightness(val.toDouble());
            } else if (arg == "--vibrancy") {
                backend.setBlurVibrancy(val.toDouble());
            }
        }
        backend.saveNow();
        std::cout << "Settings applied and saved.\n";
        return 0;
    }

    std::cerr << "Unknown command: " << cmd.toStdString() << "\nUse 'frost --help' for available commands.\n";
    return 1;
}
