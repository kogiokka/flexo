FetchContent_Declare(
    wxWidgets
    URL "https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.1/wxWidgets-3.2.1.7z"
    URL_HASH MD5=0cebf436913d12172efbe79cba02ce96
)

set(wxBUILD_SHARED ON CACHE BOOL "Build shared or static libraries")
set(wxBUILD_TESTS OFF CACHE STRING "CONSOLE_ONLY, ALL or OFF")
set(wxBUILD_SAMPLES OFF CACHE STRING "SOME, ALL or OFF")
set(wxBUILD_DEMOS OFF CACHE BOOL "Build demo applications")
set(wxUSE_GUI ON CACHE BOOL "Build the UI libraries")
set(wxBUILD_COMPATIBILITY 3.1 CACHE STRING "2.8, 3.0 or 3.1 API compatibility")
set(wxBUILD_PRECOMP ON CACHE BOOL "Use precompiled headers")
set(wxBUILD_MONOLITHIC OFF CACHE BOOL "Build a single library" )

FetchContent_MakeAvailable(wxWidgets)
