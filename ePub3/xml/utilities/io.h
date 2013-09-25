//
//  io.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-16.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __ePub3_xml_io__
#define __ePub3_xml_io__

#include <ePub3/xml/base.h>
#include <iostream>
#include <cstdint>
#if EPUB_USE(LIBXML2)
#include <libxml/xmlIO.h>
#include <libxml/HTMLtree.h>
#endif

EPUB3_XML_BEGIN_NAMESPACE

/**
 @ingroup xml-utils
 */
class InputBuffer
#if EPUB_USE(LIBXML2)
	: public WrapperBase
#endif
{
public:
    InputBuffer();
	InputBuffer(InputBuffer && o);
    virtual ~InputBuffer();
#if EPUB_USE(LIBXML2)
    xmlParserInputBuffer * xmlBuffer() { return _buf; }
    const xmlParserInputBuffer * xmlBuffer() const { return _buf; }
    operator xmlParserInputBuffer * () { return xmlBuffer(); }
    operator const xmlParserInputBuffer * () const { return xmlBuffer(); }
    
    xmlDocPtr xmlReadDocument(const char * url, const char * encoding, int options);
    xmlDocPtr htmlReadDocument(const char * url, const char * encoding, int options);
#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^ File() { return _store; }
	operator ::Windows::Storage::IStorageFile^() { return _store; }

	::Windows::Data::Xml::Dom::XmlDocument^ ReadDocument(const char* url, const char* encoding, int options);
#endif

	virtual size_t size() const = 0;
	virtual size_t offset() const = 0;

protected:
#if EPUB_USE(LIBXML2)
    xmlParserInputBufferPtr _buf;
#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^ _store;
#endif
    
    virtual size_t read(uint8_t * buf, size_t len) = 0;
    virtual bool close() { return false; }
    
    static int read_cb(void * context, char * buffer, int len);
    static int close_cb(void * context);
    
};

/**
 @ingroup xml-utils
 */
class OutputBuffer : public WrapperBase
{
public:
    OutputBuffer(const std::string & encoding = std::string());
    OutputBuffer(OutputBuffer && o) : _buf(o._buf) { o._buf = nullptr; }
    virtual ~OutputBuffer();
    
    xmlOutputBuffer * xmlBuffer() { return _buf; }
    const xmlOutputBuffer * xmlBuffer() const { return _buf; }
    operator xmlOutputBuffer * () { return xmlBuffer(); }
    operator const xmlOutputBuffer * () const { return xmlBuffer(); }
    
	int writeDocument(xmlDocPtr doc);

	virtual size_t size() const = 0;
	virtual size_t offset() const = 0;
    
protected:
    xmlOutputBufferPtr  _buf;
    
    virtual bool write(const uint8_t * buf, size_t len) = 0;
    virtual bool close() = 0;
    
    static int write_cb(void * context, const char * buffer, int len);
    static int close_cb(void * context);
    
};

/**
 @ingroup xml-utils
 */
class StreamInputBuffer : public InputBuffer
{
public:
    StreamInputBuffer(std::istream & input) : InputBuffer(), _input(input) {}
    StreamInputBuffer(StreamInputBuffer && o) : InputBuffer(std::move(o)), _input(o._input) {}
    virtual ~StreamInputBuffer() {}
    
protected:
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
    
    std::istream &  _input;
    
};

/**
 @ingroup xml-utils
 */
class StreamOutputBuffer : public OutputBuffer
{
public:
    StreamOutputBuffer(std::ostream & output, const std::string & encoding = std::string()) : OutputBuffer(), _output(output) {}
    StreamOutputBuffer(StreamOutputBuffer && o) : OutputBuffer(std::move(o)), _output(o._output) {}
    virtual ~StreamOutputBuffer() {}
    
protected:
    virtual bool write(const uint8_t * buffer, size_t len);
    virtual bool close();
    
    std::ostream &  _output;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_io__) */
