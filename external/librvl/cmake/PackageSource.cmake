set(CPACK_PACKAGE_NAME "librvl")

# Files to be ignored
set(SOURCE_IGNORE
    # Build directory
    "[Bb]uild/"
    "[Oo]ut/"

    # Downloads
    "[Dd]ownloads/"

    # IDE-generated files
    ".vs/"
    ".*.kdev4"
    ".kdev4/"
    ".vscode/"
    ".idea/"

    # Files specific to version control.
    ".fslckout"
    ".git/"
)

set(CPACK_SOURCE_IGNORE_FILES ${SOURCE_IGNORE})
set(CPACK_SOURCE_GENERATOR "ZIP")

include(CPack)
