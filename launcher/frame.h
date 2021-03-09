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

#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace reone {

struct WindowID {
    static constexpr wxWindowID btnLaunch = wxID_HIGHEST + 1;
    static constexpr wxWindowID choiceResolution = wxID_HIGHEST + 2;
};

class LauncherFrame : public wxFrame {
public:
    LauncherFrame();

private:
    wxChoice *_choiceResolution;

    void OnLaunch(wxCommandEvent &event);

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone