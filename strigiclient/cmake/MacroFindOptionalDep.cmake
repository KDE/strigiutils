# FIND_OPTIONAL_DEP macro implements two typical optional dependency handling
# approaches:
#
# Best-effort approach(FORCE_DEPS=OFF):
#	Link to all enabled optional dependencies if found. Turn off not found
# ones, and keep compiling. This greatly benefits hand-compiling from source
# if all suggested dependencies are turned on by default. Newly installed
# software conveniently integrates with whatever environment it's compiled in.
#
# Strict dependencies approach(FORCE_DEPS=ON):
#	All enabled optional dependencies must be found, or compilation aborts.
# This approach lets request and ensure specific functionality. The compilation
# is deterministic in the sense that everything that's requested is provided
# or the process fails. This is the preferred behaviour for automated building
# by package managers/distro maintainers.
#
# Parameters:
# _package:	the package to load
# _found:	the name of *_FOUND variable which is set by find_package()
#               if ${_package} is found.
# _enabled:	option/variable name which along with FORCE_DEPS options
#               controls macro behaviour:
#	${_enabled}	FORCE_DEPS	Behaviour
#		OFF	any		${_package} is not loaded
#		ON	ON		Try loading ${_package}. If package is
#                                       not found, abort(fatal error).
#		ON	OFF		Try loading ${_package}. If package is
#                                       not found, continue.
# _description:	a short description of features provided by ${_package}.
#		Used to display human-readable diagnostic messages

# macro name changed from FIND_OPTIONAL_PACKAGE to FIND_OPTIONAL_DEP due to
# clash with a macro from KDE4

# if ON, requested optional deps become required
# if OFF, requested optional deps are linked to if found

OPTION(FORCE_DEPS "Enforce strict dependencies" OFF)

macro(FIND_OPTIONAL_DEP _package _enabled _found _description)

    if(${_enabled})
        if(FORCE_DEPS)
            find_package(${_package} REQUIRED)
        else(FORCE_DEPS)
            find_package(${_package})
        endif(FORCE_DEPS)
    endif(${_enabled})

    REPORT_OPTIONAL_PACKAGE_STATUS(${_package} ${_enabled} ${_found} ${_description})

endmacro(FIND_OPTIONAL_DEP)


macro(REPORT_OPTIONAL_PACKAGE_STATUS _package _enabled _found _description)

    if(${_enabled})
        if(${_found})
            MESSAGE("** ${_package} is found. Support for ${_description} is enabled")
        else(${_found})
            MESSAGE("** ${_package} not found. Support for ${_description} is disabled")
        endif(${_found})
    else(${_enabled})
        MESSAGE("** ${_package} is disabled. No support for ${_description}")
    endif(${_enabled})

endmacro(REPORT_OPTIONAL_PACKAGE_STATUS)
