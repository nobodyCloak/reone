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

#include "../../src/common/pathutil.h"

using namespace std;

using namespace reone;

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(path_util)

BOOST_AUTO_TEST_CASE(should_get_path_ignoring_case) {
    // given
    auto tmpDirPath = fs::temp_directory_path();
    tmpDirPath.append("reone_test_path_util");
    auto tmpFilePath = tmpDirPath;
    tmpFilePath.append("MiXeD");
    fs::create_directory(tmpDirPath);
    auto tmpFile = fs::ofstream(tmpFilePath, ios::binary);
    tmpFile.flush();
    tmpFile.close();

    // when
    auto lowerPath = getPathIgnoreCase(tmpDirPath, "mixed", false);
    auto upperPath = getPathIgnoreCase(tmpDirPath, "MIXED", false);
    auto superPath = getPathIgnoreCase(tmpDirPath, "MiXeDs", false);

    // then
    BOOST_CHECK_EQUAL(tmpFilePath, lowerPath);
    BOOST_CHECK(upperPath.empty());
    BOOST_CHECK(superPath.empty());

    // cleanup
    fs::remove_all(tmpDirPath);
}

BOOST_AUTO_TEST_SUITE_END()
