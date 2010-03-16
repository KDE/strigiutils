/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <strigi/substreamproviderprovider.h>
#include <strigi/strigiconfig.h>
#include <strigi/mailinputstream.h>
#include <strigi/tarinputstream.h>
#include <strigi/zipinputstream.h>
#include <strigi/sdfinputstream.h>

using namespace Strigi;

SubStreamProviderProvider::SubStreamProviderProvider() {
}
SubStreamProvider*
SubStreamProviderProvider::subStreamProvider(InputStream* input) {
    // read the header
    const char* header;
    int64_t pos = input->position();
    int32_t headersize = input->read(header, 1024, 0);
    input->reset(pos);
    if (headersize <= 0) {
        return 0;
    }
    fprintf(stderr, "%x%x%x%x\n", header[0], header[1], header[2], header[3]);
    if (MailInputStream::checkHeader(header, headersize)) {
        return new MailInputStream(input);
    }
    if (ZipInputStream::checkHeader(header, headersize)) {
        fprintf(stderr, "zip!\n");
        return new ZipInputStream(input);
    }
    if (SdfInputStream::checkHeader(header, headersize)) {
        fprintf(stderr, "sdf!\n");
        return new SdfInputStream(input);
    }
    fprintf(stderr, "no match %i\n", headersize);
    return 0;
}
