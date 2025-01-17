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

#include "pcodereader.h"

#include <boost/regex.hpp>

#include "../../common/exception/validation.h"
#include "../../script/instrutil.h"
#include "../../script/routines.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace script {

void PcodeReader::load() {
    vector<string> insLines;
    map<int, string> labelByLineIdx;
    map<int, uint32_t> addrByLineIdx;

    fs::ifstream pcode(_path);
    string line;
    boost::smatch what;
    boost::regex re("^([_\\d\\w]+):$");
    uint32_t addr = 13;
    while (getline(pcode, line)) {
        boost::trim(line);
        if (line.empty()) {
            continue;
        }
        int lineIdx = static_cast<int>(insLines.size());
        if (boost::regex_match(line, what, re)) {
            labelByLineIdx[lineIdx] = what[1].str();
            continue;
        }
        addrByLineIdx[lineIdx] = addr;
        addr += getInstructionSize(line);
        insLines.push_back(line);
    }

    fs::path filename(_path.filename());
    filename.replace_extension(); // drop .pcode
    filename.replace_extension(); // drop .ncs

    _addrByLabel.clear();
    for (auto &pair : labelByLineIdx) {
        _addrByLabel[pair.second] = addrByLineIdx[pair.first];
    }

    _program = make_shared<ScriptProgram>(filename.string());
    for (size_t i = 0; i < insLines.size(); ++i) {
        uint32_t insAddr = addrByLineIdx.find(static_cast<int>(i))->second;
        _program->add(parseInstruction(insLines[i], insAddr));
    }
}

int PcodeReader::getInstructionSize(const string &line) {
    int result = 2;

    size_t spaceIdx = line.find(" ");
    string typeDesc(spaceIdx != string::npos ? line.substr(0, spaceIdx) : line);
    string argsLine(line.substr(typeDesc.length()));
    InstructionType type = parseInstructionType(typeDesc);

    switch (type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
    case InstructionType::DESTRUCT:
        result += 6;
        break;
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTO:
    case InstructionType::MOVSP:
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ:
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        result += 4;
        break;
    case InstructionType::CONSTS:
        result += 2;
        applyArguments(argsLine, "^ \"(.*)\"$", 1, [&result](auto &args) {
            result += args[0].length();
        });
        break;
    case InstructionType::ACTION:
        result += 3;
        break;
    case InstructionType::STORE_STATE:
        result += 8;
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        result += 2;
        break;
    default:
        break;
    };

    return result;
}

Instruction PcodeReader::parseInstruction(const string &line, uint32_t addr) const {
    size_t spaceIdx = line.find(" ");
    string typeDesc(spaceIdx != string::npos ? line.substr(0, spaceIdx) : line);
    string argsLine(line.substr(typeDesc.length()));
    InstructionType type = parseInstructionType(typeDesc);

    Instruction ins;
    ins.offset = addr;
    ins.type = type;

    switch (type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        applyArguments(argsLine, "^ ([-\\d]+), (\\d+)$", 2, [&ins](auto &args) {
            ins.stackOffset = atoi(args[0].c_str());
            ins.size = atoi(args[1].c_str());
        });
        break;
    case InstructionType::CONSTI:
        applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
            ins.intValue = atoi(args[0].c_str());
        });
        break;
    case InstructionType::CONSTF:
        applyArguments(argsLine, "^ ([-\\.\\d]+)$", 1, [&ins](auto &args) {
            ins.floatValue = atof(args[0].c_str());
        });
        break;
    case InstructionType::CONSTS:
        applyArguments(argsLine, "^ \"(.*)\"$", 1, [&ins](auto &args) {
            ins.strValue = args[0];
        });
        break;
    case InstructionType::CONSTO:
        applyArguments(argsLine, "^ (\\d+)$", 1, [&ins](auto &args) {
            ins.objectId = atoi(args[0].c_str());
        });
        break;
    case InstructionType::ACTION:
        applyArguments(argsLine, "^ (\\w+), (\\d+)$", 2, [this, &ins](auto &args) {
            ins.routine = _routines.getIndexByName(args[0]);
            ins.argCount = atoi(args[1].c_str());
        });
        break;
    case InstructionType::MOVSP:
        applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
            ins.stackOffset = atoi(args[0].c_str());
        });
        break;
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ:
        applyArguments(argsLine, "^ ([_\\d\\w]+)$", 1, [this, &ins](auto &args) {
            const string &label = args[0];
            auto maybeAddr = _addrByLabel.find(label);
            if (maybeAddr == _addrByLabel.end()) {
                throw ValidationException("Instruction address not found by label '" + label + "'");
            }
            ins.jumpOffset = maybeAddr->second - ins.offset;
        });
        break;
    case InstructionType::DESTRUCT:
        applyArguments(argsLine, "^ (\\d+), ([-\\d]+), (\\d+)$", 3, [&ins](auto &args) {
            ins.size = atoi(args[0].c_str());
            ins.stackOffset = atoi(args[1].c_str());
            ins.sizeNoDestroy = atoi(args[2].c_str());
        });
        break;
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
            ins.stackOffset = atoi(args[0].c_str());
        });
        break;
    case InstructionType::STORE_STATE:
        applyArguments(argsLine, "^ (\\d+), (\\d+)$", 2, [&ins](auto &args) {
            ins.size = atoi(args[0].c_str());
            ins.sizeLocals = atoi(args[1].c_str());
        });
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        applyArguments(argsLine, "^ (\\d+)$", 1, [&ins](auto &args) {
            ins.size = atoi(args[0].c_str());
        });
        break;
    default:
        break;
    };

    return move(ins);
}

void PcodeReader::applyArguments(const string &line, const string &restr, int numArgs, const function<void(const vector<string> &)> &fn) const {
    boost::smatch what;
    boost::regex re(restr);
    if (!boost::regex_match(line, what, re)) {
        throw invalid_argument(str(boost::format("Arguments line '%s' must match regular expression '%s'") % line % restr));
    }
    vector<string> args;
    for (int i = 0; i < numArgs; ++i) {
        args.push_back(what[1 + i].str());
    }
    fn(move(args));
}

} // namespace script

} // namespace reone
