// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include <qendian.h>
#include <QDebug>

#include "wavfile.h"
#include "utils.h"

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

WavFile::WavFile(QObject *parent)
    : QFile(parent)
    , _headerLength(0)
    , _payloadLength(0)
    , _numSamples(0)
{
}

bool WavFile::open(const QString &fileName)
{
    close();
    setFileName(fileName);
    return QFile::open(QIODevice::ReadOnly) && readFile();
}

bool WavFile::readFile()
{
    seek(0);
    CombinedHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result) {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
            || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
            && memcmp(&header.riff.type, "WAVE", 4) == 0
            && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
            && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                // Extended data available
                quint16 extraFormatBytes;
                if (peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                _format.setByteOrder(QAudioFormat::LittleEndian);
            else
                _format.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            _format.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            _format.setCodec("audio/pcm");
            _format.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            _format.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            _format.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            result = false;
        }
    }

    _headerLength = pos();

    _payloadLength = size() - pos();
    _buffer.resize(_payloadLength);
    read(_buffer.data(), _payloadLength);
    qDebug() << "WavFile::readed" << _payloadLength << "bytes from file" << fileName();

    _numSamples = _payloadLength / (2 * _format.channelCount());

    qint16* basePtr = reinterpret_cast<qint16*>(_buffer.data());
    qreal max = 0.0;
    for (int i=0; i<_numSamples; ++i)
    {
        const qint16* ptr = basePtr + i * _format.channelCount();
        const qreal realValue = abs(pcmToReal(*ptr));
        if (realValue > max)
            max = realValue;
    }

    float mult = 1.0 / max;
    for (int i=0; i<_numSamples; ++i)
    {
        qint16* ptr = basePtr + i * _format.channelCount();
        *ptr = realToPcm(mult * pcmToReal(*ptr));
    }

    return result;
}
