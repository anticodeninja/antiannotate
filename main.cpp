// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "antiannotate.h"
#include <QApplication>
#include <QCommandLineParser>

static QtMsgType verbosity;

void nullOutput(QtMsgType, const QMessageLogContext&, const QString&) {}

void debugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (type < verbosity)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }

    fflush(stderr);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("antiannotate");

    verbosity = app.arguments().contains("-v") ? QtDebugMsg : QtCriticalMsg;
    qInstallMessageHandler(debugOutput);
    MainWidget w;
    w.show();

    return app.exec();
}
