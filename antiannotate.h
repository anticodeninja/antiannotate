// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef ANTIANNOTATE_H
#define ANTIANNOTATE_H

#include <QWidget>

class Engine;
class ProgressBar;
class Waveform;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void keyPressEvent(QKeyEvent *event) override;

private:
    void createUi();
    void connectUi();
    void reset();

private:
    Engine* _engine;

    Waveform *_waveform;
    ProgressBar *_progressBar;
};

#endif // ANTIANNOTATE_H
