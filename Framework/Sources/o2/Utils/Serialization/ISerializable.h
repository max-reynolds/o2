#pragma once

#include "o2/Utils/Basic/IObject.h"
#include "o2/Utils/Types/String.h"

namespace o2
{
    template<typename __type>                                                                                   \
    class TObjectType;

    class DataValue;
    class DataDocument;

    // -----------------------------
    // Serializable object interface
    // -----------------------------
    class ISerializable: public IObject
    {
    public:
        // Serializing object into data node
        virtual void Serialize(DataValue& node) const {}

        // Deserializing object from data node
        virtual void Deserialize(const DataValue& node) {};

        // Serializes data to string
        String SerializeToString() const;

        // Deserializes data from string
        void DeserializeFromString(const String& str);

        // Assign operator from data node
        ISerializable& operator=(const DataValue& node) { return *this; };

        // DataDocument converting operator
        operator DataDocument() const;

        // Beginning serialization callback
        virtual void OnSerialize(DataValue& node) const {}

        // Completion deserialization callback
        virtual void OnDeserialized(const DataValue& node) {}

        IOBJECT(ISerializable);

    protected:
        // Serializing object into data node
        void SerializeBasic(const IObject& thisObject, DataValue& node) const;

        // Deserializing object from data node
        void DeserializeBasic(IObject& thisObject, const DataValue& node);
    };


}