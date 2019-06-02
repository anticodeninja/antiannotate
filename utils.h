// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// Copyright 2019 Artem Yamshanov, me [at] anticode.ninja

#ifndef UTILS_H
#define UTILS_H

#include <QtCore/qglobal.h>

QString formatToString(const QAudioFormat &format);

float pcmToReal(qint16 pcm);

qint16 realToPcm(float real);

#endif // UTILS_H
