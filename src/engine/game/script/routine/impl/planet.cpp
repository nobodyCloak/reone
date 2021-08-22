/*
 * Copyright (c) 2020-2021 The reone project contributors
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

/** @file
 *  Implementation of planet-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/notimplemented.h"
#include "../../../../script/types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable setPlanetSelectable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPlanetSelectable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable setPlanetAvailable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getPlanetAvailable(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getSelectedPlanet(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone