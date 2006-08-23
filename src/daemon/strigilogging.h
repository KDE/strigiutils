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

#include <fstream>
#include <stdlib.h>
#include <string>

#define STRIGI_LOG_INIT() { \
    std::string logconffile = daemondir+"/log.conf"; \
    std::fstream confFile; \
    confFile.open(logconffile.c_str(), std::ios::in); \
    if (!confFile.is_open()){ \
        /*create the default configuration file*/ \
        confFile.open(logconffile.c_str(), std::ios::out); \
        confFile << "# Set root logger level to DEBUG and its only appender to A1.\n"; \
        confFile << "log4j.rootLogger=DEBUG, A1\n\n"; \
        confFile << "# A1 is set to be a ConsoleAppender.\n"; \
        confFile << "log4j.appender.A1=org.apache.log4j.ConsoleAppender\n"; \
        confFile << "# A1 uses PatternLayout.\n"; \
        confFile << "log4j.appender.A1.layout=org.apache.log4j.PatternLayout\n"; \
        confFile << "log4j.appender.A1.layout.ConversionPattern=%d [%t] %-5p %c - %m%n\n"; \
    } \
    confFile.close(); \
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

#define STRIGI_LOG_INIT();

#define STRIGI_LOG(loggerName, message) { \
    std::cerr << loggerName << ": " << message << std::endl; \
}

#define STRIGI_LOG_DEBUG(loggerName, message) { STRIGI_LOG (loggerName, message) }
#define STRIGI_LOG_INFO(loggerName, message)  { STRIGI_LOG (loggerName, message) }
#define STRIGI_LOG_WARNING(loggerName, message)  { STRIGI_LOG (loggerName, message) }
#define STRIGI_LOG_ERROR(loggerName, message)  { STRIGI_LOG (loggerName, message) }
#define STRIGI_LOG_FATAL(loggerName, message)  { STRIGI_LOG (loggerName, message) }
#endif

#endif
