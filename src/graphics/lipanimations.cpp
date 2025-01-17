
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

#include "lipanimations.h"

#include "../../common/stream/bytearrayinput.h"

#include "../resource/resources.h"

#include "format/lipreader.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace graphics {

shared_ptr<LipAnimation> LipAnimations::doGet(string resRef) {
    shared_ptr<ByteArray> lipData(_resources.get(resRef, ResourceType::Lip));
    if (!lipData) {
        return nullptr;
    }
    auto lip = ByteArrayInputStream(*lipData);
    auto reader = LipReader(resRef);
    reader.load(lip);

    return reader.animation();
}

} // namespace graphics

} // namespace reone
