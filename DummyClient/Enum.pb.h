// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: Enum.proto
// Protobuf C++ Version: 5.29.3

#ifndef Enum_2eproto_2epb_2eh
#define Enum_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>
#include <utility>

#include "google/protobuf/runtime_version.h"
#if PROTOBUF_VERSION != 5029003
#error "Protobuf C++ gencode is built with an incompatible version of"
#error "Protobuf C++ headers/runtime. See"
#error "https://protobuf.dev/support/cross-version-runtime-guarantee/#cpp"
#endif
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_tctable_decl.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "google/protobuf/generated_enum_reflection.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_Enum_2eproto

namespace google {
namespace protobuf {
namespace internal {
template <typename T>
::absl::string_view GetAnyMessageName();
}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Internal implementation detail -- do not use these members.
struct TableStruct_Enum_2eproto {
  static const ::uint32_t offsets[];
};
extern const ::google::protobuf::internal::DescriptorTable
    descriptor_table_Enum_2eproto;
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google

namespace Protocol {
enum KeyType : int {
  KEY_NONE = 0,
  KEY_LEFT_MOUSE = 1,
  KEY_RIGHT_MOUSE = 2,
  KEY_W = 10,
  KEY_A = 11,
  KEY_S = 12,
  KEY_D = 13,
  KEY_Q = 14,
  KEY_E = 15,
  KEY_UP = 20,
  KEY_DOWN = 21,
  KEY_LEFT = 22,
  KEY_RIGHT = 23,
  KEY_SPACE = 30,
  KEY_1 = 31,
  KEY_2 = 32,
  KeyType_INT_MIN_SENTINEL_DO_NOT_USE_ =
      std::numeric_limits<::int32_t>::min(),
  KeyType_INT_MAX_SENTINEL_DO_NOT_USE_ =
      std::numeric_limits<::int32_t>::max(),
};

bool KeyType_IsValid(int value);
extern const uint32_t KeyType_internal_data_[];
constexpr KeyType KeyType_MIN = static_cast<KeyType>(0);
constexpr KeyType KeyType_MAX = static_cast<KeyType>(32);
constexpr int KeyType_ARRAYSIZE = 32 + 1;
const ::google::protobuf::EnumDescriptor*
KeyType_descriptor();
template <typename T>
const std::string& KeyType_Name(T value) {
  static_assert(std::is_same<T, KeyType>::value ||
                    std::is_integral<T>::value,
                "Incorrect type passed to KeyType_Name().");
  return ::google::protobuf::internal::NameOfEnum(KeyType_descriptor(), value);
}
inline bool KeyType_Parse(absl::string_view name, KeyType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<KeyType>(
      KeyType_descriptor(), name, value);
}
enum PlayerType : int {
  PLAYER_TYPE_NONE = 0,
  PLAYER_TYPE_KNIGHT = 1,
  PLAYER_TYPE_MAGE = 2,
  PLAYER_TYPE_ARCHER = 3,
  PlayerType_INT_MIN_SENTINEL_DO_NOT_USE_ =
      std::numeric_limits<::int32_t>::min(),
  PlayerType_INT_MAX_SENTINEL_DO_NOT_USE_ =
      std::numeric_limits<::int32_t>::max(),
};

bool PlayerType_IsValid(int value);
extern const uint32_t PlayerType_internal_data_[];
constexpr PlayerType PlayerType_MIN = static_cast<PlayerType>(0);
constexpr PlayerType PlayerType_MAX = static_cast<PlayerType>(3);
constexpr int PlayerType_ARRAYSIZE = 3 + 1;
const ::google::protobuf::EnumDescriptor*
PlayerType_descriptor();
template <typename T>
const std::string& PlayerType_Name(T value) {
  static_assert(std::is_same<T, PlayerType>::value ||
                    std::is_integral<T>::value,
                "Incorrect type passed to PlayerType_Name().");
  return PlayerType_Name(static_cast<PlayerType>(value));
}
template <>
inline const std::string& PlayerType_Name(PlayerType value) {
  return ::google::protobuf::internal::NameOfDenseEnum<PlayerType_descriptor,
                                                 0, 3>(
      static_cast<int>(value));
}
inline bool PlayerType_Parse(absl::string_view name, PlayerType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PlayerType>(
      PlayerType_descriptor(), name, value);
}

// ===================================================================



// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol


namespace google {
namespace protobuf {

template <>
struct is_proto_enum<::Protocol::KeyType> : std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor<::Protocol::KeyType>() {
  return ::Protocol::KeyType_descriptor();
}
template <>
struct is_proto_enum<::Protocol::PlayerType> : std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor<::Protocol::PlayerType>() {
  return ::Protocol::PlayerType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // Enum_2eproto_2epb_2eh
