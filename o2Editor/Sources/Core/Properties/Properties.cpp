#include "stdafx.h"
#include "Properties.h"

#include "Core/EditorApplication.h"
#include "UI/Label.h"
#include "UI/Spoiler.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"
#include "UI/WidgetLayout.h"
#include "Utils/System/Time/Timer.h"
#include "Widgets/AssetProperty.h"
#include "Widgets/EnumProperty.h"
#include "Widgets/ObjectProperty.h"
#include "Widgets/ObjectPtrProperty.h"
#include "Widgets/VectorProperty.h"

DECLARE_SINGLETON(Editor::Properties);

namespace Editor
{	
	Editor::IPropertyField::OnChangeCompletedFunc Properties::mOnPropertyCompletedChangingUndoCreateDelegate;

	Properties::Properties()
	{
		InitializePropertiesFields();

		mOnPropertyCompletedChangingUndoCreateDelegate = Func<EditorApplication, void, const String&, const Vector<DataNode>&, const Vector<DataNode>&>(
			&o2EditorApplication, &EditorApplication::DoneActorPropertyChangeAction);
	}

	Properties::~Properties()
	{
		for (auto field : mAvailablePropertiesFields)
			delete field;
	}

	void Properties::InitializePropertiesFields()
	{
		auto a = TypeOf(IAssetProperty).GetDerivedTypes();
		auto b = TypeOf(IPropertyField).GetDerivedTypes();
		auto avaialbleTypes = a + b;

		avaialbleTypes.Remove(&TypeOf(IAssetProperty));
		avaialbleTypes.Remove(&TypeOf(ObjectProperty));
		avaialbleTypes.RemoveAll([](const Type* type) { return type->GetName().Contains("TPropertyField"); });

		for (auto x : avaialbleTypes)
		{
			auto sample = (IPropertyField*)x->CreateSample();
			mAvailablePropertiesFields.Add(sample);
		}
	}

	void Properties::BuildField(UIVerticalLayout* layout, FieldInfo* fieldInfo,
								FieldPropertiesInfo& propertiesInfo, const String& path,
								const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
								const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		Timer timer;

		const Type* fieldType = fieldInfo->GetType();

		String propertyName;

		propertyName = MakeSmartFieldName(fieldInfo->GetName());

		auto fieldWidget = CreateFieldProperty(fieldInfo->GetType(), propertyName, onChangeCompleted, onChanged);
		if (!fieldWidget)
			return;

		fieldWidget->SetValuePath(path + fieldInfo->GetName());
		fieldWidget->SpecializeType(fieldType);

		layout->AddChild(fieldWidget, false);

		propertiesInfo.properties.Add(fieldInfo, fieldWidget);

		o2Debug.Log("Field " + path + "/" + fieldInfo->GetName() + " for " + (String)timer.GetDeltaTime());
	}

	void Properties::BuildFields(UIVerticalLayout* layout, Vector<FieldInfo*> fields,
								 FieldPropertiesInfo& propertiesInfo, const String& path,
								 const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
								 const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		Timer t;
		for (auto fieldInfo : fields)
			BuildField(layout, fieldInfo, propertiesInfo, path, onChangeCompleted, onChanged);

		o2Debug.Log(">>> Fields created for " + (String)t.GetDeltaTime());
	}

	void Properties::SetPrivateFieldsVisible(bool visible)
	{
		mPrivateVisible = true;
	}

	bool Properties::IsPrivateFieldsVisible() const
	{
		return mPrivateVisible;
	}

	bool Properties::IsFieldTypeSupported(const Type* type) const
	{
		if (type->GetUsage() == Type::Usage::Vector)
			return IsFieldTypeSupported(dynamic_cast<const VectorType*>(type)->GetElementType());

		IPropertyField* fieldSample = GetFieldPropertyPrototype(type);
		if (fieldSample)
			return true;

		if (type->IsBasedOn(TypeOf(IObject)))
			return true;

		if (type->GetUsage() == Type::Usage::Pointer && ((PointerType*)type)->GetUnpointedType()->IsBasedOn((TypeOf(IObject))))
			return true;

		if (type->GetUsage() == Type::Usage::Enumeration)
			return true;

		if (type->GetUsage() == Type::Usage::Property)
		{
			auto valueType = ((const PropertyType*)type)->GetValueType();

			if (valueType->GetUsage() == Type::Usage::Enumeration)
				return true;

			fieldSample = GetFieldPropertyPrototype(valueType);
			if (fieldSample)
				return true;
		}

		return false;
	}

	void Properties::FreeProperties(FieldPropertiesInfo& propertiesInfo)
	{

	}

	bool Properties::IsPropertyVisible(FieldInfo* info, bool allowPrivate) const
	{
		if (info->HasAttribute<IgnoreEditorPropertyAttribute>())
			return false;

		if (info->GetProtectionSection() == ProtectSection::Public || allowPrivate)
			return true;

		if (info->HasAttribute<EditorPropertyAttribute>())
			return true;

		return false;
	}

	bool Properties::IsPropertyVisible(FieldInfo* info) const
	{
		return IsPropertyVisible(info, mPrivateVisible);
	}

	void Properties::BuildObjectProperties(UIVerticalLayout* layout, const Type* type,
										   FieldPropertiesInfo& propertiesInfo, const String& path,
										   const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
										   const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		BuildObjectProperties(layout, type->GetFieldsWithBaseClasses(), propertiesInfo, path, onChangeCompleted, onChanged);
	}

	void Properties::BuildObjectProperties(UIVerticalLayout* layout, Vector<FieldInfo*> fields,
										   FieldPropertiesInfo& propertiesInfo, const String& path,
										   const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
										   const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		Vector<FieldInfo*> regularFields = fields.FindAll(
			[&](FieldInfo* x) { return IsPropertyVisible(x, false); });

		BuildFields(layout, regularFields, propertiesInfo, path, onChangeCompleted, onChanged);

		if (mPrivateVisible)
		{
			Vector<FieldInfo*> privateFields = fields.FindAll(
				[&](FieldInfo* x) { return IsPropertyVisible(x, true) && !regularFields.Contains(x); });

			if (!privateFields.IsEmpty())
			{
				UISpoiler* privates = o2UI.CreateWidget<UISpoiler>("expand with caption");
				privates->SetCaption("Private");

				BuildFields(privates, privateFields, propertiesInfo, path, onChangeCompleted, onChanged);

				layout->AddChild(privates);
			}
		}
	}

	IPropertyField* Properties::CreateFieldProperty(const Type* type, const String& name,
													const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
													const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		if (type->GetUsage() == Type::Usage::Vector)
			return CreateVectorField(type, name, onChangeCompleted, onChanged);

		IPropertyField* fieldSample = GetFieldPropertyPrototype(type);
		if (fieldSample)
			return CreateRegularField(&fieldSample->GetType(), name, onChangeCompleted, onChanged);

		if (type->IsBasedOn(TypeOf(IObject)))
			return CreateObjectField(type, name, onChangeCompleted, onChanged);

		if (type->GetUsage() == Type::Usage::Pointer && ((PointerType*)type)->GetUnpointedType()->IsBasedOn((TypeOf(IObject))))
			return CreateObjectPtrField(type, name, onChangeCompleted, onChanged);

		if (type->GetUsage() == Type::Usage::Enumeration)
			return CreateRegularField(&TypeOf(EnumProperty), name, onChangeCompleted, onChanged);

		if (type->GetUsage() == Type::Usage::Property)
		{
			auto valueType = ((const PropertyType*)type)->GetValueType();

			if (valueType->GetUsage() == Type::Usage::Enumeration)
				return CreateRegularField(&TypeOf(EnumProperty), name, onChangeCompleted, onChanged);

			fieldSample = GetFieldPropertyPrototype(valueType);
			if (fieldSample)
				return CreateRegularField(&fieldSample->GetType(), name, onChangeCompleted, onChanged);
		}

		return nullptr;
	}

	IPropertyField* Properties::CreateRegularField(const Type* fieldPropertyType, const String& name,
												   const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
												   const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		IPropertyField* fieldProperty = dynamic_cast<IPropertyField*>(o2UI.CreateWidget(*fieldPropertyType, "with caption"));
		fieldProperty->onChanged = onChanged;
		fieldProperty->onChangeCompleted = onChangeCompleted;
		fieldProperty->SetCaption(name);

		return fieldProperty;
	}

	IPropertyField* Properties::CreateObjectField(const Type* type, const String& name,
												  const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
												  const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		IPropertyField* fieldProperty = mnew ObjectProperty();
		fieldProperty->onChanged = onChanged;
		fieldProperty->onChangeCompleted = onChangeCompleted;
		fieldProperty->SetCaption(name);

		return fieldProperty;
	}

	IPropertyField* Properties::CreateObjectPtrField(const Type* type, const String& name,
													 const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
													 const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		IPropertyField* fieldProperty = mnew ObjectPtrProperty();
		fieldProperty->onChanged = onChanged;
		fieldProperty->onChangeCompleted = onChangeCompleted;
		fieldProperty->SetCaption(name);

		return fieldProperty;
	}

	IPropertyField* Properties::CreateVectorField(const Type* type, const String& name,
												  const IPropertyField::OnChangeCompletedFunc& onChangeCompleted /*= mOnPropertyCompletedChangingUndoCreateDelegate*/,
												  const IPropertyField::OnChangedFunc& onChanged /*= IPropertyField::OnChangedFunc::empty*/)
	{
		if (!IsFieldTypeSupported(type))
			return nullptr;

		IPropertyField* fieldProperty = mnew VectorProperty();
		fieldProperty->onChanged = onChanged;
		fieldProperty->onChangeCompleted = onChangeCompleted;
		fieldProperty->SetCaption(name);

		return fieldProperty;
	}

	String Properties::MakeSmartFieldName(const String& fieldName)
	{
		String begn;

		if (fieldName[0] == 'm' && fieldName[1] >= 'A' && fieldName[1] <= 'Z')
			begn = fieldName.SubStr(1);
		else if (fieldName[0] == 'm' && fieldName[1] == '_')
			begn = fieldName.SubStr(2);
		else if (fieldName[0] == '_')
			begn = fieldName.SubStr(1);
		else
			begn = fieldName;

		if (begn.StartsWith("o2::"))
			begn.Erase(0, 4);

		if (begn.StartsWith("Editor::"))
			begn.Erase(0, 9);

		if (begn.StartsWith("UI"))
			begn = begn;

		String res;
		int len = begn.Length();
		bool newWord = true;
		bool lastUpper = false;
		for (int i = 0; i < len; i++)
		{
			if (begn[i] == '_')
			{
				res += ' ';
				newWord = true;
				lastUpper = false;
			}
			else if (newWord && begn[i] >= 'a' && begn[i] <= 'z')
			{
				res += begn[i] + ('A' - 'a');
				lastUpper = true;
			}
			else if (!newWord && begn[i] >= 'A' && begn[i] <= 'Z')
			{
				if (!lastUpper)
					res += ' ';

				res += begn[i];
				lastUpper = begn[i] >= 'A' && begn[i] <= 'Z';
			}
			else if (i < len - 1 && begn[i] == ':' && begn[i + 1] == ':')
			{
				res += ": ";
				lastUpper = false;
				i++;
			}
			else
			{
				res += begn[i];
				lastUpper = begn[i] >= 'A' && begn[i] <= 'Z';
			}

			newWord = begn[i] >= '0' && begn[i] <= '9';
		}

		return res;
	}

	IPropertyField* Properties::GetFieldPropertyPrototype(const Type* type) const
	{
		for (auto field : mAvailablePropertiesFields)
		{
			auto fieldType = field->GetFieldType();
			if (type->GetUsage() == Type::Usage::Pointer && fieldType->GetUsage() == Type::Usage::Pointer)
			{
				if (((PointerType*)type)->GetUnpointedType()->IsBasedOn(*((PointerType*)fieldType)->GetUnpointedType()))
					return field;
			}
			else if (type->IsBasedOn(*fieldType))
				return field;
		}

		return nullptr;
	}
}