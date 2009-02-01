/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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

#ifndef STRIGI_ANALYZERPLUGIN_H
#define STRIGI_ANALYZERPLUGIN_H
#include <strigi/strigiconfig.h>

#include <list>

namespace Strigi {
class StreamEndAnalyzerFactory;
class StreamThroughAnalyzerFactory;
class StreamSaxAnalyzerFactory;
class StreamLineAnalyzerFactory;
class StreamEventAnalyzerFactory;

/**
 * @brief Provides a list of analyzer factories present within a plugin.
 *
 * Each loadable plugin has one AnalyzerFactoryFactory. This factory
 * can pass the available factories to the application that loads the plugin.
 */
class AnalyzerFactoryFactory {
public:
    /** Destructor */
    virtual ~AnalyzerFactoryFactory() {}

    /**
     * @brief Return instances of the StreamEndAnalyzerFactories available
     * in this plugin.
     *
     * The default implementation returns an empty list. A particular plugin
     * should subclass the AnalyzerFactoryFactory and overload this function
     * if it implements any StreamEndAnalyzers.
     */
    virtual std::list<StreamEndAnalyzerFactory*>
            streamEndAnalyzerFactories() const {
        std::list<StreamEndAnalyzerFactory*> af;
        return af;
    }
    /**
     * @brief Return instances of the StreamThroughAnalyzerFactories available
     * in this plugin.
     *
     *  The default implementation returns an empty list. A particular plugin
     *  should subclass the AnalyzerFactoryFactory and overload this function
     *  if it implements any StreamThroughAnalyzers.
     */
    virtual std::list<StreamThroughAnalyzerFactory*>
            streamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        return af;
    }
    /**
     * @brief Return instances of the StreamSaxAnalyzerFactories available
     * in this plugin.
     *
     * The default implementation returns an empty list. A particular plugin
     * should subclass the AnalyzerFactoryFactory and overload this function
     * if it implements any StreamSaxAnalyzers.
     */
    virtual std::list<StreamSaxAnalyzerFactory*>
            streamSaxAnalyzerFactories() const {
        std::list<StreamSaxAnalyzerFactory*> af;
        return af;
    }
    /**
     * @brief Return instances of the StreamLineAnalyzerFactories available
     * in this plugin.
     *
     * The default implementation returns an empty list. A particular plugin
     * should subclass the AnalyzerFactoryFactory and overload this function
     * if it implements any StreamLineAnalyzers.
     */
    virtual std::list<StreamLineAnalyzerFactory*>
            streamLineAnalyzerFactories() const {
        std::list<StreamLineAnalyzerFactory*> af;
        return af;
    }
    /**
     * @brief Return instances of the StreamEventAnalyzerFactories available
     * in this plugin.
     *
     * The default implementation returns an empty list. A particular plugin
     * should subclass the AnalyzerFactoryFactory and overload this function
     * if it implements any StreamEventAnalyzers.
     */
    virtual std::list<StreamEventAnalyzerFactory*>
            streamEventAnalyzerFactories() const {
        std::list<StreamEventAnalyzerFactory*> af;
        return af;
    }
};
}

/** @deprecated use STRIGI_EXPORT instead */
#define STRIGI_PLUGIN_API STRIGI_EXPORT

/**
 * @brief Export a Strigi::AnalyzerFactoryFactory in a plugin
 *
 * @param CLASS the name of the subclass of Strigi::AnalyzerFactoryFactory
 * that provides the analyzer factories available in this plugin.
 */
#define STRIGI_ANALYZER_FACTORY(CLASS) extern "C" { STRIGI_EXPORT \
    const Strigi::AnalyzerFactoryFactory* strigiAnalyzerFactory() { \
        return new CLASS(); \
    } \
    STRIGI_EXPORT \
    void deleteStrigiAnalyzerFactory(const Strigi::AnalyzerFactoryFactory* f) { \
        delete f; \
    } \
}
#endif

