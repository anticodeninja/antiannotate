// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "waveform.h"
#include "wavfile.h"
#include "utils.h"
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

template<int N> class PowerOfTwo
{ public: static const int Result = PowerOfTwo<N-1>::Result * 2; };

template<> class PowerOfTwo<0>
{ public: static const int Result = 1; };

const int SpectrumLengthSamples = PowerOfTwo<FFTLengthPowerOfTwo>::Result;

Waveform::Waveform(QWidget *parent)
    :   QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(50);
}

Waveform::~Waveform()
{
}

void Waveform::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), _pixmap, _pixmap.rect());
}

void Waveform::resizeEvent(QResizeEvent *event)
{
    if (event->size() != event->oldSize())
        updatePixmap(event->size());
}

void Waveform::fileChanged(WavFile* file)
{
    _file = file;
    if (_file != nullptr)
    {
        qDebug() << "Waveform::bufferChanged"
                 << "format" << file->format()
                 << "payloadLength" << file->payloadLength();
        updateSpectrum();
        updatePixmap(size());
    } else {
        qDebug() << "Waveform::reset";
    }
}

void Waveform::updateSpectrum()
{
    const int spectrumHalf = SpectrumLengthSamples / 2;
    _spectrum = QImage(_file->numSamples() / spectrumHalf - 2, spectrumHalf, QImage::Format_ARGB32);

    float input[SpectrumLengthSamples];
    float output[SpectrumLengthSamples];

    int offset = 0;
    float max = 0;
    for (int i=0; i<_spectrum.width(); ++i) {
        for (int j = 0; j < SpectrumLengthSamples; ++j) {
            const qint16* ptr = _file->data() + (offset + j) * _file->format().channelCount();
            input[j] = pcmToReal(*ptr);
        }
        _fft.calculateFFT(output, input);
        for (int j = 0; j < spectrumHalf; ++j) {
            float power = qMin(qAbs(output[j]), 1.0f);
            _spectrum.setPixelColor(i, j, QColor::fromHsv(0, 0, static_cast<int>(power * 255)));
        }

        offset += spectrumHalf;
    }
}

void Waveform::updatePixmap(const QSize &newSize)
{
    if (_file == nullptr)
        return;

    const int redSamples = newSize.width();
    const int half = _pixmap.height() / 2;

    qDebug() << "Waveform::updatePixmap"
             << "numSamples" << _file->numSamples()
             << "redSamples" << redSamples;

    _pixmap = QPixmap(newSize);
    QPainter painter(&_pixmap);

    painter.fillRect(_pixmap.rect(), Qt::black);
    painter.setPen(QPen(Qt::white));

    const int originY = ((pcmToReal(*_file->data()) + 1.0) / 2) * half;
    const QPoint origin(0, originY);

    QLine line(origin, origin);

    for (int i=0; i<_file->numSamples(); ++i) {
        const qint16* ptr = _file->data() + i * _file->format().channelCount();

        const float realValue = pcmToReal(*ptr);

        const int x = static_cast<qreal>(i) / _file->numSamples() * redSamples;
        const int y = ((realValue + 1.0) / 2) * half;

        line.setP2(QPoint(x, y));
        painter.drawLine(line);
        line.setP1(line.p2());
    }

    painter.drawImage(QRect(0, half, newSize.width(), half), _spectrum);
}
