#include "Escargot.h"
#include "Object.h"
#include "ExecutionContext.h"
#include "Context.h"
#include "ErrorObject.h"

namespace Escargot {

Object::Object(ExecutionState& state, size_t defaultSpace, bool initPlainArea)
    : m_structure(state.context()->defaultStructureForObject())
    , m_rareData(nullptr)
{
    m_values.resizeWithUninitializedValues(defaultSpace);
    if (initPlainArea) {
        initPlainObject(state);
    }
}

Object::Object(ExecutionState& state)
    : Object(state, ESCARGOT_OBJECT_BUILTIN_PROPERTY_NUMBER, true)
{
}

void Object::initPlainObject(ExecutionState& state)
{
    m_values[0] = Value(state.context()->globalObject()->objectPrototype());
}

Object* Object::createBuiltinObjectPrototype(ExecutionState& state)
{
    Object* obj = new Object(state, 1, false);
    obj->m_structure = state.context()->defaultStructureForObject();
    obj->m_values[0] = Value(Value::Null);
    return obj;
}

Object* Object::createFunctionPrototypeObject(ExecutionState& state, FunctionObject* function)
{
    Object* obj = new Object(state, 2, false);
    obj->m_structure = state.context()->defaultStructureForFunctionPrototypeObject();
    obj->m_values[0] = Value();
    obj->m_values[1] = Value(function);

    return obj;
}

Value Object::getPrototypeSlowCase(ExecutionState& state)
{
    return getOwnProperty(state, state.context()->staticStrings().__proto__);
}

bool Object::setPrototypeSlowCase(ExecutionState& state, const Value& value)
{
    return defineOwnProperty(state, state.context()->staticStrings().__proto__, ObjectPropertyDescriptorForDefineOwnProperty(value));
}

Value Object::getOwnProperty(ExecutionState& state, String* P)
{
    PropertyName propertyName(state, P);
    return getOwnProperty(state, propertyName);
}

// http://www.ecma-international.org/ecma-262/6.0/#sec-ordinarygetownproperty
Value Object::getOwnProperty(ExecutionState& state, const PropertyName& P)
{
    size_t idx = m_structure->findProperty(state, P);
    if (LIKELY(idx != SIZE_MAX)) {
        const ObjectStructureItem& item = m_structure->readProperty(state, idx);
        if (item.m_descriptor.isDataProperty()) {
            if (LIKELY(!item.m_descriptor.isNativeAccessorProperty())) {
                return m_values[idx];
            } else {
                ObjectPropertyNativeGetterSetterData* data = item.m_descriptor.nativeGetterSetterData();
                return data->m_getter(state, this);
            }
        } else {
            // TODO
            // implement js getter, setter
            RELEASE_ASSERT_NOT_REACHED();
        }
    }

    return Value();
}

size_t Object::findOwnProperty(ExecutionState& state, String* P)
{
    return m_structure->findProperty(state, P);
}

size_t Object::findOwnProperty(ExecutionState& state, const PropertyName& P)
{
    return m_structure->findProperty(state, P);
}

bool Object::checkPropertyAlreadyDefinedWithNonWritableInPrototype(ExecutionState& state, const PropertyName& P)
{
    Value __proto__Value = getPrototype(state);
    while (true) {
        if (!__proto__Value.isObject()) {
            break;
        }
        Object* targetObj = __proto__Value.asObject();
        size_t t = targetObj->m_structure->findProperty(state, P);
        if (t != SIZE_MAX) {
            // http://www.ecma-international.org/ecma-262/5.1/#sec-8.12.5
            // If IsAccessorDescriptor(desc) is true, then
            // Let setter be desc.[[Set]] which cannot be undefined.
            // Call the [[Call]] internal method of setter providing O as the this value and providing V as the sole argument.
            const ObjectStructureItem& item = targetObj->m_structure->readProperty(state, t);
            if (!item.m_descriptor.isDataProperty()) {
                // TODO
                // implement js getter, setter
                RELEASE_ASSERT_NOT_REACHED();
                /*
                if (!foundInPrototype) {
                    ESPropertyAccessorData* data = targetObj->accessorData(t);
                    if (data->isAccessorDescriptor()) {
                        if (data->getJSSetter()) {
                            ESValue receiverVal(this);
                            if (receiver)
                                receiverVal = *receiver;
                            ESValue args[] = {val};
                            ESFunctionObject::call(ESVMInstance::currentInstance(), data->getJSSetter(), receiverVal, args, 1, false);
                            return true;
                        }
                        return false;
                    }
                    if (data->getNativeSetter()) {
                        if (!targetObj->hiddenClass()->m_propertyInfo[t].writable()) {
                            return false;
                        }
                        foundInPrototype = true;
                        break;
                    } else {
                        return false;
                    }
                }*/
            } else {
                if (!item.m_descriptor.isWritable()) {
                    return true;
                }
            }
        }
        // TODO
        /*else if (targetObj->isESStringObject()) {
            uint32_t idx = key.toIndex();
            if (idx != ESValue::ESInvalidIndexValue)
                if (idx < targetObj->asESStringObject()->length())
                    return false;
        }*/
        __proto__Value = targetObj->getPrototype(state);
    }
    return false;
}

bool Object::defineOwnProperty(ExecutionState& state, const PropertyName& propertyName, const ObjectPropertyDescriptorForDefineOwnProperty& desc)
{
    if (isEverSetAsPrototypeObject()) {
        // TODO
        // implement bad time
        RELEASE_ASSERT_NOT_REACHED();
    }

    size_t oldIdx = m_structure->findProperty(state, propertyName);
    if (oldIdx == SIZE_MAX) {
        if (checkPropertyAlreadyDefinedWithNonWritableInPrototype(state, propertyName)) {
            return false;
        }

        if (UNLIKELY(!isExtensible()))
            return false;

        m_structure = m_structure->addProperty(state, propertyName, desc.descriptor());

        // TODO implement JS getter setter
        RELEASE_ASSERT(desc.descriptor().isDataProperty());

        m_values.pushBack(desc.value());
        return true;
    } else {
        size_t idx = oldIdx;
        const ObjectStructureItem& item = m_structure->readProperty(state, idx);
        if (!item.m_descriptor.isWritable())
            return false;

        if (item.m_descriptor != desc.descriptor()) {
            if (!item.m_descriptor.isConfigurable()) {
                return false;
            }

            deleteOwnProperty(state, idx);
            defineOwnProperty(state, propertyName, desc);
            return true;
        }

        if (item.m_descriptor.isDataProperty()) {
            if (LIKELY(!item.m_descriptor.isNativeAccessorProperty())) {
                m_values[idx] = desc.value();
                return true;
            } else {
                ObjectPropertyNativeGetterSetterData* data = item.m_descriptor.nativeGetterSetterData();
                return data->m_setter(state, this, desc.value());
            }
        } else {
            // TODO
            // implement js getter, setter
            RELEASE_ASSERT_NOT_REACHED();
        }

        return true;
    }
}

bool Object::defineOwnProperty(ExecutionState& state, String* P, const ObjectPropertyDescriptorForDefineOwnProperty& desc)
{
    PropertyName propertyName(state, P);
    return defineOwnProperty(state, propertyName, desc);
}

Object::ObjectGetResult Object::get(ExecutionState& state, String* P, Object* receiver)
{
    PropertyName propertyName(state, P);
    return get(state, propertyName, receiver);
}

Object::ObjectGetResult Object::get(ExecutionState& state, const PropertyName& propertyName, Object* receiver)
{
    Object* target = this;
    while (true) {
        size_t idx = target->findOwnProperty(state, propertyName);
        if (idx != SIZE_MAX) {
            return ObjectGetResult(target->getOwnProperty(state, idx, receiver));
        }
        Value __proto__ = target->getPrototype(state);
        if (__proto__.isObject()) {
            target = __proto__.asObject();
        } else {
            return ObjectGetResult();
        }
    }
}

bool Object::set(ExecutionState& state, String* P, const Value& v, Object* receiver)
{
    PropertyName propertyName(state, P);
    return set(state, propertyName, v, receiver);
}

// http://www.ecma-international.org/ecma-262/6.0/#sec-ordinary-object-internal-methods-and-internal-slots-set-p-v-receiver
bool Object::set(ExecutionState& state, const PropertyName& propertyName, const Value& v, Object* receiver)
{
    size_t idx = findOwnProperty(state, propertyName);
    if (idx == SIZE_MAX) {
        Value target = this->getPrototype(state);
        while (target.isObject()) {
            Object* O = target.asObject();
            size_t idx = O->findOwnProperty(state, propertyName);
            if (idx != SIZE_MAX) {
                return set(state, propertyName, v, receiver);
            }
            target = O->getPrototype(state);
        }
        ObjectPropertyDescriptorForDefineOwnProperty desc(v);
        return defineOwnProperty(state, propertyName, desc);
    } else {
        const ObjectStructureItem& ownDesc = m_structure->readProperty(state, idx);
        // If IsDataDescriptor(ownDesc) is true, then
        if (ownDesc.m_descriptor.isDataProperty()) {
            // If ownDesc.[[Writable]] is false, return false.
            if (!ownDesc.m_descriptor.isWritable()) {
                return false;
            }
            // TODO If Type(Receiver) is not Object, return false.
            // Let existingDescriptor be Receiver.[[GetOwnProperty]](P).
            size_t receiverIdx = receiver->m_structure->findProperty(propertyName);
            // If existingDescriptor is not undefined, then
            if (receiverIdx != SIZE_MAX) {
                const ObjectStructureItem& existingDescriptor = receiver->m_structure->readProperty(state, receiverIdx);
                // If IsAccessorDescriptor(existingDescriptor) is true, return false.
                if (existingDescriptor.m_descriptor.isAccessorProperty()) {
                    return false;
                }
                // If existingDescriptor.[[Writable]] is false, return false
                if (!existingDescriptor.m_descriptor.isWritable()) {
                    return false;
                }
                // Let valueDesc be the PropertyDescriptor{[[Value]]: V}.
                ObjectPropertyDescriptorForDefineOwnProperty desc(v);
                return receiver->defineOwnProperty(state, propertyName, desc);
            } else {
                // Else Receiver does not currently have a property P,
                // Return CreateDataProperty(Receiver, P, V).
                ObjectPropertyDescriptorForDefineOwnProperty desc(v);
                return defineOwnProperty(state, propertyName, desc);
            }
        } else {
            // TODO
            RELEASE_ASSERT_NOT_REACHED();
        }
    }
}

void Object::setThrowsException(ExecutionState& state, const PropertyName& P, const Value& v, Object* receiver)
{
    if (UNLIKELY(!set(state, P, v, receiver))) {
        ErrorObject::throwBuiltinError(state, ErrorObject::Code::TypeError, P.string(), false, String::emptyString, errorMessage_DefineProperty_NotWritable);
    }
}

void Object::setThrowsExceptionWhenStrictMode(ExecutionState& state, const PropertyName& P, const Value& v, Object* receiver)
{
    if (UNLIKELY(!set(state, P, v, receiver)) && state.inStrictMode()) {
        ErrorObject::throwBuiltinError(state, ErrorObject::Code::TypeError, P.string(), false, String::emptyString, errorMessage_DefineProperty_NotWritable);
    }
}

void Object::throwCannotDefineError(ExecutionState& state, const PropertyName& P)
{
    ErrorObject::throwBuiltinError(state, ErrorObject::Code::TypeError, P.string(), false, String::emptyString, errorMessage_DefineProperty_RedefineNotConfigurable);
}

void Object::throwCannotWriteError(ExecutionState& state, const PropertyName& P)
{
    ErrorObject::throwBuiltinError(state, ErrorObject::Code::TypeError, P.string(), false, String::emptyString, errorMessage_DefineProperty_NotWritable);
}

void Object::deleteOwnProperty(ExecutionState& state, size_t idx)
{
    if (isPlainObject()) {
        const ObjectStructureItem& ownDesc = m_structure->readProperty(state, idx);
        if (ownDesc.m_descriptor.isNativeAccessorProperty()) {
            ensureObjectRareData();
            // TODO
            RELEASE_ASSERT_NOT_REACHED();
            m_rareData->m_isPlainObject = false;
        }
    }

    m_structure = m_structure->removeProperty(state, idx);
    m_values.erase(idx);

}

}
