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

#include "resources.h"

#include "../common/logutil.h"
#include "../common/pathutil.h"
#include "../common/stream/fileinput.h"

#include "format/bifreader.h"
#include "format/erfreader.h"
#include "format/rimreader.h"
#include "provider/erf.h"
#include "provider/folder.h"
#include "provider/keybif.h"
#include "provider/rim.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

void Resources::indexKeyFile(const fs::path &path) {
    if (!fs::exists(path)) {
        return;
    }
    auto keyBif = make_unique<KeyBifResourceProvider>(path, static_cast<int>(_providers.size()));
    keyBif->init();
    indexProvider(move(keyBif), path);
}

void Resources::indexErfFile(const fs::path &path, bool transient) {
    if (!fs::exists(path)) {
        return;
    }
    auto erf = make_unique<ErfResourceProvider>(path, static_cast<int>(_providers.size()));
    erf->init();
    indexProvider(move(erf), path, transient);
}

void Resources::indexRimFile(const fs::path &path, bool transient) {
    if (!fs::exists(path)) {
        return;
    }
    auto rim = make_unique<RimResourceProvider>(path, static_cast<int>(_providers.size()));
    rim->init();
    indexProvider(move(rim), path, transient);
}

void Resources::indexDirectory(const fs::path &path) {
    if (!fs::exists(path)) {
        return;
    }
    auto folder = make_unique<Folder>(path, static_cast<int>(_providers.size()));
    folder->init();
    indexProvider(move(folder), path);
}

void Resources::indexExeFile(const fs::path &path) {
    if (!fs::exists(path)) {
        return;
    }
    _exePath = path;
    debug("Index executable " + path.string(), LogChannels::resources);
}

void Resources::indexProvider(unique_ptr<IResourceProvider> &&provider, const fs::path &path, bool transient) {
    debug(boost::format("Index provider %d at '%s'") % provider->id() % path.string(), LogChannels::resources);
    if (transient) {
        _transientProviders.push_back(move(provider));
    } else {
        _providers.push_back(move(provider));
    }
}

void Resources::clearAllProviders() {
    _transientProviders.clear();
    _providers.clear();
}

void Resources::clearTransientProviders() {
    for (auto &provider : _transientProviders) {
        debug("Remove provider " + to_string(provider->id()), LogChannels::resources);
    }
    _transientProviders.clear();
}

shared_ptr<ByteArray> Resources::get(const string &resRef, ResourceType type, bool logNotFound) {
    if (resRef.empty()) {
        return nullptr;
    }
    ResourceId id(resRef, type);
    shared_ptr<ByteArray> data(getFromProviders(id, _providers));
    if (!data) {
        data = getFromProviders(id, _transientProviders);
    }
    if (!data && logNotFound) {
        warn("Resource '" + id.string() + "' not found", LogChannels::resources);
    }
    return move(data);
}

shared_ptr<ByteArray> Resources::getFromExe(uint32_t name, PEResourceType type) {
    auto pe = FileInputStream(_exePath, OpenMode::Binary);

    auto peReader = PeReader();
    peReader.load(pe);

    auto data = peReader.find(name, type);
    if (!data) {
        warn(boost::format("Resource %u of type %d not found in EXE") % name % static_cast<int>(type), LogChannels::resources);
        return nullptr;
    }

    return move(data);
}

shared_ptr<ByteArray> Resources::getFromProviders(const ResourceId &id, const vector<unique_ptr<IResourceProvider>> &providers) {
    for (auto provider = providers.rbegin(); provider != providers.rend(); ++provider) {
        shared_ptr<ByteArray> data((*provider)->find(id));
        if (data) {
            debug(boost::format("Resource '%s' found in provider %d") % id.string() % (*provider)->id(), LogChannels::resources2);
            return data;
        }
    }
    return nullptr;
}

} // namespace resource

} // namespace reone
