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

#include "../../resource/format/erfreader.h"

#include "../tool.h"

namespace reone {

class ErfTool : public ITool {
public:
    void invoke(
        Operation operation,
        const boost::filesystem::path &target,
        const boost::filesystem::path &gamePath,
        const boost::filesystem::path &destPath) override;

    bool supports(Operation operation, const boost::filesystem::path &target) const override;

private:
    void list(const resource::ErfReader &erf);
    void extract(resource::ErfReader &erf, const boost::filesystem::path &erfPath, const boost::filesystem::path &destPath);
    void toERF(Operation operation, const boost::filesystem::path &target);
};

} // namespace reone