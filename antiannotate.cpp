// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "engine.h"
#include "antiannotate.h"
#include "waveform.h"
#include "progressbar.h"
#include "utils.h"

#include <QApplication>
#include <QFileInfo>
#include <QScreen>
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QKeyEvent>

MainWidget::MainWidget(QWidget *parent)
    :   QWidget(parent)
    ,   _engine(new Engine(this))
    ,   _waveform(new Waveform(this))
    ,   _progressBar(new ProgressBar(this))
{
    setAttribute(Qt::WA_ShowWithoutActivating);
    auto geometry = QApplication::screens().at(0)->availableGeometry();
    resize(geometry.size());
    move(geometry.topLeft());

    createUi();
    connectUi();

    if (QApplication::arguments().length() < 2) {
        qFatal("Filename is not provided");
    }

    auto filename = QApplication::arguments().at(1);
    qDebug() << "Try to load file" << filename;

    setWindowTitle(QFileInfo(filename).absoluteFilePath());
    _engine->loadFile(filename);
    _engine->startPlayback();
}

MainWidget::~MainWidget()
{
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (_engine->state() == QAudio::ActiveState)
            _engine->suspend();
        else
            _engine->startPlayback();
    }
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi()
{
    setWindowTitle("antiannotate");

    QVBoxLayout *windowLayout = new QVBoxLayout(this);

    QScopedPointer<QHBoxLayout> waveformLayout(new QHBoxLayout);
    waveformLayout->addWidget(_progressBar);
    _progressBar->setMinimumHeight(600);
    waveformLayout->setMargin(0);
    _waveform->setLayout(waveformLayout.data());
    _waveform->setMinimumHeight(600);
    waveformLayout.take();
    windowLayout->addWidget(_waveform);

    QScopedPointer<QHBoxLayout> analysisLayout(new QHBoxLayout);
    windowLayout->addLayout(analysisLayout.data());
    analysisLayout.take();

    setLayout(windowLayout);
}

void MainWidget::connectUi()
{
    connect(_engine, &Engine::errorMessage,
            [this] (const QString &heading, const QString &detail) {
        QMessageBox::warning(this, heading, detail, QMessageBox::Close);
    });

    connect(_engine, &Engine::fileChanged,
            _progressBar, &ProgressBar::fileChanged);

    connect(_engine, &Engine::playPositionChanged,
            _progressBar, &ProgressBar::playPositionChanged);

    connect(_engine, &Engine::fileChanged,
            _waveform, &Waveform::fileChanged);

    connect(_progressBar, &ProgressBar::selectionPositionChanged,
            _engine, &Engine::selectionPositionChanged);
}

void MainWidget::reset()
{
    _engine->reset();
}
