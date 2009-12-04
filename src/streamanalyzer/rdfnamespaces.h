/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#ifndef RDFNAMESAPCES
#define RDFNAMESPACES

// This is a list of the most often encountered rdf namespaces used by analyzers
// For internal use ONLY and should not be installed
// Typical use: fieldName = NIE "title"

// General
#define RDF	"http://www.w3.org/1999/02/22-rdf-syntax-ns#"

//Nepomuk
#define NAO	"http://www.semanticdesktop.org/ontologies/2007/08/15/nao#"
#define NIE	"http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"
#define NFO	"http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#"
#define NCO	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"
#define NMO	"http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#"
#define NCAL	"http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#"
#define NEXIF	"http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"// obsolete
#define NID3	"http://www.semanticdesktop.org/ontologies/2007/05/10/nid3#"// obsolete

//Draft
#define NMM_DRAFT	"http://www.semanticdesktop.org/ontologies/2009/02/19/nmm#"


#endif
