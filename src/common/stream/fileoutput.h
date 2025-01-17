/*
 * Copyright (c) 2020-2022 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "output.h"

namespace reone {

class FileOutputStream : public IOutputStream {
public:
    FileOutputStream(const boost::filesystem::path &path, OpenMode mode = OpenMode::Text) :
        _stream(path, mode == OpenMode::Binary ? std::ios::binary : static_cast<std::ios::openmode>(0)) {
    }

    void writeByte(char c) override {
        _stream.put(c);
    }

    void write(const ByteArray &bytes) override {
        _stream.write(&bytes[0], bytes.size());
    }

    void write(const char *data, int length) override {
        _stream.write(data, length);
    }

    void close() {
        _stream.close();
    }

    size_t position() override {
        return _stream.tellp();
    }

private:
    boost::filesystem::ofstream _stream;
};

} // namespace reone
