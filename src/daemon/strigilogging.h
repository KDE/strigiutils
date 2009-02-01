/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef STRIGI_LOGGING_H
#define STRIGI_LOGGING_H

#ifdef HAVE_LOG4CXX
// log4cxx libraries
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/basicconfigurator.h>

#define STRIGI_LOG_INIT_BASIC() { \
    log4cxx::BasicConfigurator::configure();\
}

#define STRIGI_LOG_INIT(logconffile) { \
    log4cxx::PropertyConfigurator::configure(logconffile);\
}

#define STRIGI_LOG_DEBUG(loggerName, message) { \
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(loggerName); \
    LOG4CXX_DEBUG(logger, message) \
}

#define STRIGI_LOG_INFO(loggerName, message) { \
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(loggerName); \
    LOG4CXX_INFO(logger, message) \
}

#define STRIGI_LOG_WARNING(loggerName, message) { \
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(loggerName); \
    LOG4CXX_WARN(logger, message) \
}

#define STRIGI_LOG_ERROR(loggerName, message) { \
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(loggerName); \
    LOG4CXX_ERROR(logger, message) \
}

#define STRIGI_LOG_FATAL(loggerName, message) { \
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(loggerName); \
    LOG4CXX_FATAL(logger, message) \
}

#else
// no log4cxx support

#include <iostream>

#define STRIGI_LOG_INIT(logconffile) { }

#define STRIGI_LOG_INIT_BASIC() { }

#define STRIGI_LOG(loggerName, message) { \
    std::cerr << loggerName << ": " << message << std::endl; \
}

#define STRIGI_LOG_DEBUG(loggerName, msg) { }
#define STRIGI_LOG_INFO(loggerName, msg)  { }
#define STRIGI_LOG_WARNING(loggerName, msg) { STRIGI_LOG (loggerName, msg) }
#define STRIGI_LOG_ERROR(loggerName, msg) { STRIGI_LOG (loggerName, msg) }
#define STRIGI_LOG_FATAL(loggerName, msg) { STRIGI_LOG (loggerName, msg) }
#endif

#endif
