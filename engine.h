// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef ENGINE_H
#define ENGINE_H

#include "wavfile.h"

#include <QAudio>
#include <QAudioDeviceInfo>
#include <QBuffer>

class QAudioOutput;

class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    QAudio::State state() const { return _state; }
    void reset();
    bool loadFile(const QString &fileName);
    qint64 playPosition() const { return _playPosition; }

public slots:
    void selectionPositionChanged(qint64 position);
    void startPlayback();
    void suspend();

signals:
    void fileChanged(WavFile* file);
    void stateChanged(QAudio::State state);
    void playPositionChanged(qint64 position);
    void errorMessage(const QString &heading, const QString &detail);

private slots:
    void audioNotify();
    void audioStateChanged(QAudio::State state);

private:
    void resetAudioDevices();
    bool initialize();
    void stopPlayback();
    void setState(QAudio::State state);
    void setPlayPosition(qint64 position, bool forceEmit = false);

private:
    QAudio::State _state;
    WavFile *_file;
    QAudioDeviceInfo _audioOutputDevice;
    QBuffer _audioOutputIODevice;
    QAudioOutput* _audioOutput;
    qint64 _playPosition;

};

#endif // ENGINE_H
