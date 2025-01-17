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

#include <boost/test/unit_test.hpp>

#include "../../src/common/logutil.h"
#include "../../src/common/stream/fileoutput.h"
#include "../../src/resource/resources.h"

#include "../checkutil.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(resources)

BOOST_AUTO_TEST_CASE(should_index_providers_and_get_resources_without_caching) {
    // given

    setLogLevel(LogLevel::None);

    auto tmpDirPath = fs::temp_directory_path();
    tmpDirPath.append("reone_test_resources");
    fs::create_directory(tmpDirPath);

    auto keyPath = tmpDirPath;
    keyPath.append("sample.key");
    auto key = FileOutputStream(keyPath, OpenMode::Binary);
    key.write("KEY V1  ");
    key.write("\x00\x00\x00\x00");
    key.write("\x00\x00\x00\x00");
    key.write("\x00\x00\x00\x00");
    key.write("\x00\x00\x00\x00");
    key.write("\x00\x00\x00\x00");
    key.write("\x00\x00\x00\x00");
    key.write(ByteArray(32, '\0'));
    key.close();

    auto erfPath = tmpDirPath;
    erfPath.append("sample.erf");
    auto erf = FileOutputStream(erfPath, OpenMode::Binary);
    erf.write("ERF V1.0");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write("\x00\x00\x00\x00");
    erf.write(ByteArray(116, '\0'));
    erf.close();

    auto rimPath = tmpDirPath;
    rimPath.append("sample.rim");
    auto rim = FileOutputStream(rimPath, OpenMode::Binary);
    rim.write("RIM V1.0");
    rim.write("\x00\x00\x00\x00");
    rim.write("\x00\x00\x00\x00");
    rim.write("\x00\x00\x00\x00");
    rim.close();

    auto overridePath = tmpDirPath;
    overridePath.append("override");
    fs::create_directory(overridePath);

    auto resPath = overridePath;
    resPath.append("sample.txt");
    auto res = FileOutputStream(resPath, OpenMode::Binary);
    res.write("Hello, world!");
    res.close();

    auto resources = Resources();

    auto expectedResData = ByteArray("Hello, world!");

    // when

    resources.indexKeyFile(keyPath);
    resources.indexErfFile(erfPath);
    resources.indexDirectory(overridePath);
    resources.indexRimFile(rimPath, true);

    auto numProviders = resources.providers().size();
    auto numTransientProviders = resources.transientProviders().size();

    auto actualResData1 = resources.get("sample", ResourceType::Txt, false);

    resources.clearAllProviders();

    auto actualResData2 = resources.get("sample", ResourceType::Txt, false);

    // then

    BOOST_CHECK_EQUAL(3ll, numProviders);
    BOOST_CHECK_EQUAL(1ll, numTransientProviders);
    BOOST_CHECK(static_cast<bool>(actualResData1));
    BOOST_TEST((expectedResData == (*actualResData1)), notEqualMessage(expectedResData, *actualResData1));
    BOOST_CHECK(!static_cast<bool>(actualResData2));

    // cleanup

    fs::remove_all(tmpDirPath);
}

BOOST_AUTO_TEST_SUITE_END()
