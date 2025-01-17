//===- LLDBPropertyDefEmitter.cpp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// These tablegen backends emits LLDB's PropertyDefinition values.
//
//===----------------------------------------------------------------------===//

#include "LLDBTableGenBackends.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/StringMatcher.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <map>
#include <vector>

using namespace llvm;

/// Map of properties definitions to their associated records. Also makes sure
/// our property definitions are sorted in a deterministic way.
typedef std::map<std::string, std::vector<Record *>> RecordsByDefinition;

/// Groups all properties by their definition.
static RecordsByDefinition getPropertyList(std::vector<Record *> Properties) {
  RecordsByDefinition result;
  for (Record *Property : Properties)
    result[Property->getValueAsString("Definition").str()].push_back(Property);
  return result;
}

static void emitPropertyEnum(Record *Property, raw_ostream &OS) {
  OS << "eProperty";
  OS << Property->getName();
  OS << ",\n";
}

static void emitProperty(Record *Property, raw_ostream &OS) {
  OS << "  {";

  // Emit the property name.
  OS << "\"" << Property->getValueAsString("Name") << "\"";
  OS << ", ";

  // Emit the property type.
  OS << "OptionValue::eType";
  OS << Property->getValueAsString("Type");
  OS << ", ";

  // Emit the property's global value.
  OS << (Property->getValue("Global") ? "true" : "false");
  OS << ", ";

  bool hasDefaultUnsignedValue = Property->getValue("HasDefaultUnsignedValue");
  bool hasDefaultEnumValue = Property->getValue("HasDefaultEnumValue");
  bool hasDefaultStringValue = Property->getValue("HasDefaultStringValue");

  // Guarantee that every property has a default value.
  assert((hasDefaultUnsignedValue || hasDefaultEnumValue ||
          hasDefaultStringValue) &&
         "Property must have a default value");

  // Guarantee that no property has both a default unsigned value and a default
  // enum value, since they're bothed stored in the same field.
  assert(!(hasDefaultUnsignedValue && hasDefaultEnumValue) &&
         "Property cannot have both a unsigned and enum default value.");

  // Emit the default uint value.
  if (hasDefaultUnsignedValue) {
    OS << std::to_string(Property->getValueAsInt("DefaultUnsignedValue"));
  } else if (hasDefaultEnumValue) {
    OS << Property->getValueAsString("DefaultEnumValue");
  } else {
    OS << "0";
  }
  OS << ", ";

  // Emit the default string value.
  if (hasDefaultStringValue) {
    if (auto D = Property->getValue("DefaultStringValue")) {
      OS << "\"";
      OS << D->getValue()->getAsUnquotedString();
      OS << "\"";
    } else {
      OS << "\"\"";
    }
  } else {
    OS << "nullptr";
  }
  OS << ", ";

  // Emit the enum values value.
  if (Property->getValue("EnumValues"))
    OS << Property->getValueAsString("EnumValues");
  else
    OS << "{}";
  OS << ", ";

  // Emit the property description.
  if (auto D = Property->getValue("Description")) {
    OS << "\"";
    OS << D->getValue()->getAsUnquotedString();
    OS << "\"";
  } else {
    OS << "\"\"";
  }

  OS << "},\n";
}

/// Emits all property initializers to the raw_ostream.
static void emityProperties(std::string PropertyName,
                            std::vector<Record *> PropertyRecords,
                            raw_ostream &OS) {
  // Generate the macro that the user needs to define before including the
  // *.inc file.
  std::string NeededMacro = "LLDB_PROPERTIES_" + PropertyName;
  std::replace(NeededMacro.begin(), NeededMacro.end(), ' ', '_');

  // All options are in one file, so we need put them behind macros and ask the
  // user to define the macro for the options that are needed.
  OS << "// Property definitions for " << PropertyName << "\n";
  OS << "#ifdef " << NeededMacro << "\n";
  OS << "static constexpr PropertyDefinition g_" << PropertyName
     << "_properties[] = {\n";
  for (Record *R : PropertyRecords)
    emitProperty(R, OS);
  OS << "};\n";
  // We undefine the macro for the user like Clang's include files are doing it.
  OS << "#undef " << NeededMacro << "\n";
  OS << "#endif // " << PropertyName << " Property\n\n";
}

/// Emits all property initializers to the raw_ostream.
static void emitPropertyEnum(std::string PropertyName,
                             std::vector<Record *> PropertyRecords,
                             raw_ostream &OS) {
  // Generate the macro that the user needs to define before including the
  // *.inc file.
  std::string NeededMacro = "LLDB_PROPERTIES_" + PropertyName;
  std::replace(NeededMacro.begin(), NeededMacro.end(), ' ', '_');

  // All options are in one file, so we need put them behind macros and ask the
  // user to define the macro for the options that are needed.
  OS << "// Property enum cases for " << PropertyName << "\n";
  OS << "#ifdef " << NeededMacro << "\n";
  for (Record *R : PropertyRecords)
    emitPropertyEnum(R, OS);
  // We undefine the macro for the user like Clang's include files are doing it.
  OS << "#undef " << NeededMacro << "\n";
  OS << "#endif // " << PropertyName << " Property\n\n";
}

void lldb_private::EmitPropertyDefs(RecordKeeper &Records, raw_ostream &OS) {
  emitSourceFileHeader("Property definitions for LLDB.", OS);

  std::vector<Record *> Properties =
      Records.getAllDerivedDefinitions("Property");
  for (auto &PropertyRecordPair : getPropertyList(Properties)) {
    emityProperties(PropertyRecordPair.first, PropertyRecordPair.second, OS);
  }
}

void lldb_private::EmitPropertyEnumDefs(RecordKeeper &Records,
                                        raw_ostream &OS) {
  emitSourceFileHeader("Property definition enum for LLDB.", OS);

  std::vector<Record *> Properties =
      Records.getAllDerivedDefinitions("Property");
  for (auto &PropertyRecordPair : getPropertyList(Properties)) {
    emitPropertyEnum(PropertyRecordPair.first, PropertyRecordPair.second, OS);
  }
}
