// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#include "progressbar.h"
#include "wavfile.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

ProgressBar::ProgressBar(QWidget *parent)
    :   QWidget(parent)
    ,   _multiplier(0)
    ,   _bufferLength(0)
    ,   _playPosition(0)
{
    setAutoFillBackground(false);
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    const int pos = static_cast<qreal>(_playPosition) / _bufferLength * width();
    painter.fillRect(rect(), QColor(0, 0, 0, 0));
    painter.setPen(QPen(Qt::white));
    painter.drawLine(pos, 0, pos, height());
    painter.drawText(0, 0, width(), height(), 0, QString("%0/%1")
                     .arg(static_cast<qint64>(1000 * _playPosition / _multiplier))
                     .arg(static_cast<qint64>(1000 * _bufferLength / _multiplier)));
}

void ProgressBar::mousePressEvent(QMouseEvent *event)
{
    _playPosition = static_cast<qreal>(event->pos().x()) / width() * _bufferLength;
    emit selectionPositionChanged(_playPosition);
    update();
}

void ProgressBar::fileChanged(WavFile* file)
{
    if (file) {
        _multiplier = file->format().sampleRate() * file->format().channelCount() * file->format().sampleSize() / 8;
        _playPosition = 0;
        _bufferLength = file->payloadLength();
    } else {
        _multiplier = 0;
        _playPosition = 0;
        _bufferLength = 0;
    }
    update();
}

void ProgressBar::playPositionChanged(qint64 playPosition)
{
    Q_ASSERT(playPosition >= 0);
    Q_ASSERT(playPosition <= _bufferLength);
    _playPosition = playPosition;
    repaint();
}
