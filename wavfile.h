// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>

class WavFile : public QFile
{
public:
    WavFile(QObject *parent = 0);

    using QFile::open;
    bool open(const QString &fileName);
    const QAudioFormat &format() const { return _format; }
    const QByteArray &buffer() const { return _buffer; }
    const qint16 *data() const { return reinterpret_cast<const qint16*>(_buffer.constData()); }
    qint64 headerLength() const { return _headerLength; }
    qint64 payloadLength() const { return _payloadLength; }
    qint64 numSamples() const { return _numSamples; }

private:
    bool readFile();

private:
    QByteArray _buffer;
    QAudioFormat _format;
    qint64 _headerLength;
    qint64 _payloadLength;
    qint64 _numSamples;
};

#endif // WAVFILE_H
