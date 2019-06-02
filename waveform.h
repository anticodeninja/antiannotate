// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QPixmap>
#include <QWidget>

#include "fftreal_wrapper.h"

class WavFile;

class Waveform : public QWidget
{
    Q_OBJECT

public:
    explicit Waveform(QWidget *parent = 0);
    ~Waveform();

    // QWidget
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void fileChanged(WavFile* file);
    void updateSpectrum();
    void updatePixmap(const QSize &newSize);

private:
    WavFile* _file;
    FFTRealWrapper _fft;
    QPixmap _pixmap;
    QImage _spectrum;
};

#endif // WAVEFORM_H
