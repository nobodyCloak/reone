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

#include "../../src/common/stream/bytearrayoutput.h"
#include "../../src/resource/2da.h"
#include "../../src/resource/2das.h"
#include "../../src/resource/resources.h"

using namespace std;

using namespace reone;
using namespace reone::resource;

class StubProvider : public IResourceProvider {
public:
    void add(ResourceId id, shared_ptr<ByteArray> res) {
        _resources.insert(make_pair(id, move(res)));
    }

    shared_ptr<ByteArray> find(const ResourceId &id) override { return _resources.at(id); }

    int id() const override { return 0; };

private:
    unordered_map<ResourceId, shared_ptr<ByteArray>, ResourceIdHasher> _resources;
};

BOOST_AUTO_TEST_SUITE(two_das)

BOOST_AUTO_TEST_CASE(should_get_2da_with_caching) {
    // given

    auto resBytes = make_shared<ByteArray>();
    auto res = ByteArrayOutputStream(*resBytes);
    res.write("2DA V2.b\n");
    res.write("label\t\0");
    res.write("\x00\x00\x00\x00", 4);
    res.write("\x00\x00\x00\x00", 4);

    auto provider = make_unique<StubProvider>();
    provider->add(ResourceId("sample", ResourceType::TwoDa), resBytes);

    auto resources = Resources();
    resources.indexProvider(move(provider), "[stub]", false);

    auto twoDas = TwoDas(resources);

    // when

    auto twoDa1 = twoDas.get("sample");

    resources.clearAllProviders();

    auto twoDa2 = twoDas.get("sample");

    // then

    BOOST_CHECK(static_cast<bool>(twoDa1));
    BOOST_CHECK(static_cast<bool>(twoDa2));
    BOOST_CHECK_EQUAL(twoDa1.get(), twoDa2.get());
}

BOOST_AUTO_TEST_SUITE_END()
