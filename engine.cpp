// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "engine.h"
#include "utils.h"

#include <math.h>

#include <QAudioOutput>
#include <QCoreApplication>
#include <QDebug>

const qint64 BufferDurationUs       = 10 * 1000000;
const int    NotifyIntervalMs       = 100;

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   _state(QAudio::StoppedState)
    ,   _file(0)
    ,   _audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    ,   _audioOutput(0)
    ,   _playPosition(0)
{
}

Engine::~Engine()
{

}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

bool Engine::loadFile(const QString &fileName)
{
    reset();

    Q_ASSERT(!_file);
    Q_ASSERT(!fileName.isEmpty());
    _file = new WavFile(this);
    if (!_file->open(fileName)) {
        emit errorMessage(tr("Could not open file"), fileName);
        return false;
    }

    if (_file->format().codec() != "audio/pcm" ||
        _file->format().sampleType() != QAudioFormat::SignedInt ||
        _file->format().sampleSize() != 16 ||
        _file->format().byteOrder() != QAudioFormat::LittleEndian ||
        _file->format().sampleRate() != 8000 && _file->format().sampleRate() != 16000) {
        emit errorMessage(tr("Audio format not supported"),
                          formatToString(_file->format()));
        return false;
    }

    if (!initialize())
        return false;  // Error message is generated inside

    emit fileChanged(_file);

    return true;
}

//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void Engine::selectionPositionChanged(qint64 position)
{
    _playPosition = position - (position % (_file->format().sampleSize() * _file->format().channelCount()));
    _audioOutputIODevice.seek(_playPosition);
}

void Engine::startPlayback()
{
    if (!_audioOutput)
        return;

    if (QAudio::SuspendedState == _state) {
#ifdef Q_OS_WIN
        // The Windows backend seems to internally go back into ActiveState
        // while still returning SuspendedState, so to ensure that it doesn't
        // ignore the resume() call, we first re-suspend
        _audioOutput->suspend();
#endif
        _audioOutput->resume();
    } else {
        setPlayPosition(_playPosition, true);

        _audioOutputIODevice.close();
        _audioOutputIODevice.setData(_file->buffer());
        _audioOutputIODevice.open(QIODevice::ReadOnly);
        _audioOutputIODevice.seek(_playPosition);

        _audioOutput->start(&_audioOutputIODevice);
    }
}

void Engine::suspend()
{
    if (QAudio::ActiveState == _state ||
        QAudio::IdleState == _state) {
        _audioOutput->suspend();
    }
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void Engine::audioNotify()
{
    setPlayPosition(qMin(_file->payloadLength(), _audioOutputIODevice.pos()));
}

void Engine::audioStateChanged(QAudio::State state)
{
    qDebug() << "Engine::audioStateChanged from" << _state
             << "to" << state;

    if (QAudio::IdleState == state &&
            _audioOutputIODevice.pos() == _audioOutputIODevice.size()) {
        stopPlayback();
    } else {
        if (QAudio::StoppedState == state) {
            // Check error
            if (QAudio::NoError != _audioOutput->error()) {
                reset();
                return;
            }
        }
        setState(state);
    }
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void Engine::resetAudioDevices()
{
    delete _audioOutput;
    _audioOutput = nullptr;
    setPlayPosition(0);
}

void Engine::reset()
{
    stopPlayback();
    setState(QAudio::StoppedState);
    delete _file;
    _file = nullptr;
    emit fileChanged(_file);
    resetAudioDevices();
}

bool Engine::initialize()
{
    if (_audioOutput && _audioOutput->format() == _file->format())
        return true;

    qDebug() << "supportedCodecs:" << _audioOutputDevice.supportedCodecs();
    qDebug() << "supportedSampleRates:" << _audioOutputDevice.supportedSampleRates();
    qDebug() << "supportedSampleSizes:" << _audioOutputDevice.supportedSampleSizes();
    qDebug() << "supportedSampleTypes:" << _audioOutputDevice.supportedSampleTypes();
    qDebug() << "supportedByteOrders:" << _audioOutputDevice.supportedByteOrders();
    qDebug() << "supportedChannelCounts:" << _audioOutputDevice.supportedChannelCounts();

    if (!_audioOutputDevice.isFormatSupported(_file->format())) {
        qCritical() << "notSupportedFormat:" << formatToString(_file->format());
        emit errorMessage(tr("Audio format not supported"),
                          formatToString(_file->format()));
        return false;
    }

    resetAudioDevices();
    _audioOutput = new QAudioOutput(_audioOutputDevice, _file->format(), this);
    _audioOutput->setNotifyInterval(NotifyIntervalMs);
    connect(_audioOutput, &QAudioOutput::stateChanged,
            this, &Engine::audioStateChanged);
    connect(_audioOutput, &QAudioOutput::notify,
            this, &Engine::audioNotify);

    qDebug() << "Engine::initialize" << "dataLength" << _file->payloadLength();
    qDebug() << "Engine::initialize" << "format" << _file->format();

    return true;
}

void Engine::stopPlayback()
{
    if (_audioOutput) {
        _audioOutput->stop();
        QCoreApplication::instance()->processEvents();
        setPlayPosition(0);
    }
}

void Engine::setState(QAudio::State state)
{
    const bool changed = (_state != state);
    _state = state;
    if (changed)
        emit stateChanged(_state);
}

void Engine::setPlayPosition(qint64 position, bool forceEmit)
{
    const bool changed = _playPosition != position;
    _playPosition = position;
    if (changed || forceEmit)
        emit playPositionChanged(_playPosition);
}
