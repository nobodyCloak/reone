# Copyright (c) 2020-2022 The reone project contributors

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set(RESOURCE_HEADERS
    2da.h
    2das.h
    gff.h
    gffs.h
    format/2dareader.h
    format/2dawriter.h
    format/bifreader.h
    format/binreader.h
    format/erfreader.h
    format/erfwriter.h
    format/gffreader.h
    format/gffwriter.h
    format/keyreader.h
    format/pereader.h
    format/rimreader.h
    format/rimwriter.h
    format/tlkreader.h
    format/tlkwriter.h
    id.h
    provider.h
    provider/erf.h
    provider/folder.h
    provider/keybif.h
    provider/rim.h
    resources.h
    services.h
    strings.h
    talktable.h
    types.h
    typeutil.h)

set(RESOURCE_SOURCES
    2da.cpp
    2das.cpp
    gff.cpp
    gffs.cpp
    format/2dareader.cpp
    format/2dawriter.cpp
    format/bifreader.cpp
    format/binreader.cpp
    format/erfreader.cpp
    format/erfwriter.cpp
    format/gffreader.cpp
    format/gffwriter.cpp
    format/keyreader.cpp
    format/pereader.cpp
    format/rimreader.cpp
    format/rimwriter.cpp
    format/tlkreader.cpp
    format/tlkwriter.cpp
    provider/erf.cpp
    provider/folder.cpp
    provider/keybif.cpp
    provider/rim.cpp
    resources.cpp
    strings.cpp
    talktable.cpp
    typeutil.cpp)

add_library(resource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(resource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(resource PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
