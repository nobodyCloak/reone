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

#include "2da.h"

#include "../../common/binarywriter.h"
#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/stream/fileinput.h"
#include "../../resource/2da.h"
#include "../../resource/format/2dareader.h"
#include "../../resource/format/2dawriter.h"

#include "tinyxml2.h"

using namespace std;

using namespace tinyxml2;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void TwoDaTool::invoke(
    Operation operation,
    const fs::path &input,
    const fs::path &outputDir,
    const fs::path &gamePath) {

    return invokeBatch(operation, vector<fs::path> {input}, outputDir, gamePath);
}

void TwoDaTool::invokeBatch(
    Operation operation,
    const std::vector<fs::path> &input,
    const fs::path &outputDir,
    const fs::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToXML) {
            toXML(path, outDir);
        } else {
            to2DA(path, outDir);
        }
    });
}

void TwoDaTool::toXML(const fs::path &input, const fs::path &outputDir) {
    auto stream = FileInputStream(input, OpenMode::Binary);

    auto reader = TwoDaReader();
    reader.load(stream);

    auto table = reader.twoDa();

    auto xmlPath = outputDir;
    xmlPath.append(input.filename().string() + ".xml");
    auto fp = fopen(xmlPath.string().c_str(), "wb");

    auto printer = XMLPrinter(fp);
    printer.PushHeader(false, true);
    printer.OpenElement("rows");
    for (int row = 0; row < table->getRowCount(); ++row) {
        printer.OpenElement("row");
        printer.PushAttribute("_index", row);
        for (int col = 0; col < table->getColumnCount(); ++col) {
            printer.PushAttribute(
                table->columns()[col].c_str(),
                table->rows()[row].values[col].c_str());
        }
        printer.CloseElement();
    }
    printer.CloseElement();

    fclose(fp);
}

void TwoDaTool::to2DA(const fs::path &path, const fs::path &destPath) {
    auto fp = fopen(path.string().c_str(), "rb");

    auto document = XMLDocument();
    document.LoadFile(fp);

    auto rootElement = document.RootElement();
    if (!rootElement) {
        cerr << "XML is empty" << endl;
        fclose(fp);
        return;
    }

    auto columns = vector<string>();
    auto rows = vector<TwoDa::Row>();

    // Columns
    auto firstElement = rootElement->FirstChildElement();
    for (auto attribute = firstElement->FirstAttribute(); attribute; attribute = attribute->Next()) {
        if (strncmp(attribute->Name(), "_index", 6) == 0) {
            continue;
        }
        columns.push_back(attribute->Name());
    }

    // Rows
    for (auto element = rootElement->FirstChildElement(); element; element = element->NextSiblingElement()) {
        auto row = TwoDa::Row();
        for (auto attribute = element->FirstAttribute(); attribute; attribute = attribute->Next()) {
            if (strncmp(attribute->Name(), "_index", 6) == 0) {
                continue;
            }
            row.values.push_back(attribute->Value());
        }
        rows.push_back(move(row));
    }

    auto twoDa = TwoDa(move(columns), move(rows));

    vector<string> tokens;
    boost::split(
        tokens,
        path.filename().string(),
        boost::is_any_of("."),
        boost::token_compress_on);

    auto twoDaPath = fs::path(destPath);
    twoDaPath.append(tokens[0] + ".2da");

    auto writer = TwoDaWriter(twoDa);
    writer.save(twoDaPath);
}

bool TwoDaTool::supports(Operation operation, const fs::path &input) const {
    return !fs::is_directory(input) &&
           ((input.extension() == ".2da" && operation == Operation::ToXML) ||
            (input.extension() == ".xml" && operation == Operation::To2DA));
}

} // namespace reone
