/*
 * Copyright (c) 2016-present Samsung Electronics Co., Ltd
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef __EscargotPointerValue__
#define __EscargotPointerValue__

namespace Escargot {

class String;
class Symbol;
class Object;
class FunctionObject;
class ArrayObject;
class StringObject;
class SymbolObject;
class NumberObject;
class BooleanObject;
class RegExpObject;
class DateObject;
class ErrorObject;
class GlobalObject;
#if ESCARGOT_ENABLE_PROMISE
class PromiseObject;
#endif
#if ESCARGOT_ENABLE_PROXY
class ProxyObject;
#endif
#if ESCARGOT_ENABLE_TYPEDARRAY
class ArrayBufferObject;
class ArrayBufferView;
#endif
class DoubleInSmallValue;
class JSGetterSetter;
class GlobalRegExpFunctionObject;
class IteratorObject;
class MapObject;
class SetObject;
class WeakMapObject;
class WeakSetObject;

#define POINTER_VALUE_STRING_SYMBOL_TAG_IN_DATA 0x3
// finding what is type of PointerValue operation is used in SmallValue <-> Value and interpreter
// Only Object, String are seen in regular runtime-code
// We figure what type of PointerValue by POINTER_VALUE_STRING_TAG_IN_DATA
//   - Every data area of Object starts with [<vtable>, m_structure...]
//   - Every data area of String starts with [<vtable>, POINTER_VALUE_STRING_TAG_IN_DATA ...]
// finding what is type of PointerValue(Object, String) without accessing vtable provides gives better performance
// but, it uses more memory for String type
// POINTER_VALUE_STRING_TAG_IN_DATA is not essential thing for implementing figure type(we can use isObject, isString)
// so, we can remove POINTER_VALUE_STRING_SYMBOL_TAG_IN_DATA in very small device future

class PointerValue : public gc {
public:
    virtual ~PointerValue() {}
    virtual bool isString() const
    {
        return false;
    }

    virtual bool isSymbol() const
    {
        return false;
    }

    virtual bool isObject() const
    {
        return false;
    }

    virtual bool isFunctionObject() const
    {
        return false;
    }

    virtual bool isArrayObject() const
    {
        return false;
    }

    virtual bool isStringObject() const
    {
        return false;
    }

    virtual bool isSymbolObject() const
    {
        return false;
    }

    virtual bool isNumberObject() const
    {
        return false;
    }

    virtual bool isBooleanObject() const
    {
        return false;
    }

    virtual bool isDateObject() const
    {
        return false;
    }

    virtual bool isRegExpObject() const
    {
        return false;
    }

    virtual bool isErrorObject() const
    {
        return false;
    }

    virtual bool isGlobalObject() const
    {
        return false;
    }

#if ESCARGOT_ENABLE_PROMISE
    virtual bool isPromiseObject() const
    {
        return false;
    }
#endif

#if ESCARGOT_ENABLE_PROXY
    virtual bool isProxyObject() const
    {
        return false;
    }
#endif

#if ESCARGOT_ENABLE_TYPEDARRAY
    virtual bool isArrayBufferView() const
    {
        return false;
    }

    virtual bool isDataViewObject() const
    {
        return false;
    }

    virtual bool isTypedArrayObject() const
    {
        return false;
    }

    virtual bool isArrayBufferObject() const
    {
        return false;
    }
#endif

    virtual bool isDoubleInSmallValue() const
    {
        return false;
    }

    virtual bool isJSGetterSetter() const
    {
        return false;
    }

    virtual bool isGlobalRegExpFunctionObject()
    {
        return false;
    }

    virtual bool isIteratorObject() const
    {
        return false;
    }

    virtual bool isMapObject() const
    {
        return false;
    }

    virtual bool isSetObject() const
    {
        return false;
    }

    virtual bool isWeakMapObject() const
    {
        return false;
    }

    virtual bool isWeakSetObject() const
    {
        return false;
    }

    String* asString()
    {
        ASSERT(isString());
        return (String*)this;
    }

    Symbol* asSymbol()
    {
        ASSERT(isSymbol());
        return (Symbol*)this;
    }

    Object* asObject()
    {
        ASSERT(isObject());
        return (Object*)this;
    }

    ArrayObject* asArrayObject()
    {
        ASSERT(isArrayObject());
        return (ArrayObject*)this;
    }

    FunctionObject* asFunctionObject()
    {
        ASSERT(isFunctionObject());
        return (FunctionObject*)this;
    }

    StringObject* asStringObject()
    {
        ASSERT(isStringObject());
        return (StringObject*)this;
    }

    SymbolObject* asSymbolObject()
    {
        ASSERT(isSymbolObject());
        return (SymbolObject*)this;
    }

    NumberObject* asNumberObject()
    {
        ASSERT(isNumberObject());
        return (NumberObject*)this;
    }

    BooleanObject* asBooleanObject()
    {
        ASSERT(isBooleanObject());
        return (BooleanObject*)this;
    }

    RegExpObject* asRegExpObject()
    {
        ASSERT(isRegExpObject());
        return (RegExpObject*)this;
    }

    DateObject* asDateObject()
    {
        ASSERT(isDateObject());
        return (DateObject*)this;
    }

    ErrorObject* asErrorObject()
    {
        ASSERT(isErrorObject());
        return (ErrorObject*)this;
    }

    GlobalObject* asGlobalObject()
    {
        ASSERT(isGlobalObject());
        return (GlobalObject*)this;
    }

#if ESCARGOT_ENABLE_PROMISE
    PromiseObject* asPromiseObject()
    {
        ASSERT(isPromiseObject());
        return (PromiseObject*)this;
    }
#endif
#if ESCARGOT_ENABLE_PROXY
    ProxyObject* asProxyObject()
    {
        ASSERT(isProxyObject());
        return (ProxyObject*)this;
    }
#endif
#if ESCARGOT_ENABLE_TYPEDARRAY
    ArrayBufferObject* asArrayBufferObject()
    {
        ASSERT(isArrayBufferObject());
        return (ArrayBufferObject*)this;
    }

    ArrayBufferView* asArrayBufferView()
    {
        ASSERT(isArrayBufferView());
        return (ArrayBufferView*)this;
    }
#endif

    DoubleInSmallValue* asDoubleInSmallValue()
    {
        ASSERT(isDoubleInSmallValue());
        return (DoubleInSmallValue*)this;
    }

    JSGetterSetter* asJSGetterSetter()
    {
        ASSERT(isJSGetterSetter());
        return (JSGetterSetter*)this;
    }

    GlobalRegExpFunctionObject* asGlobalRegExpFunctionObject()
    {
        ASSERT(isGlobalRegExpFunctionObject());
        return (GlobalRegExpFunctionObject*)this;
    }

    IteratorObject* asIteratorObject()
    {
        ASSERT(isIteratorObject());
        return (IteratorObject*)this;
    }

    MapObject* asMapObject()
    {
        ASSERT(isMapObject());
        return (MapObject*)this;
    }

    SetObject* asSetObject()
    {
        ASSERT(isSetObject());
        return (SetObject*)this;
    }

    WeakMapObject* asWeakMapObject()
    {
        ASSERT(isWeakMapObject());
        return (WeakMapObject*)this;
    }

    WeakSetObject* asWeakSetObject()
    {
        ASSERT(isWeakSetObject());
        return (WeakSetObject*)this;
    }

    bool hasTag(const size_t tag) const
    {
        return tag == *((size_t*)(this));
    }

    bool hasTagInFirstDataArea(const size_t tag) const
    {
        return tag == *((size_t*)(this) + 1);
    }

    size_t getTagInFirstDataArea() const
    {
        return *((size_t*)(this) + 1);
    }
};
}

#endif
