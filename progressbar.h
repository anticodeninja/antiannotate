// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

class WavFile;

class ProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = 0);
    ~ProgressBar();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void fileChanged(WavFile* file);
    void playPositionChanged(qint64 playPosition);

signals:
    void selectionPositionChanged(qint64 position);

private:
    qint64 _playPosition;
    qint64 _bufferLength;
    qint64 _multiplier;
};

#endif // PROGRESSBAR_H
