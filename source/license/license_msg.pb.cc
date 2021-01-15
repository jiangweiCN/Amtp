// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: license_msg.proto

#include "license_msg.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace license_msg {
class LicenseDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<License> _instance;
} _License_default_instance_;
class LicenseInfoDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<LicenseInfo> _instance;
} _LicenseInfo_default_instance_;
}  // namespace license_msg
static void InitDefaultsscc_info_License_license_5fmsg_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::license_msg::_License_default_instance_;
    new (ptr) ::license_msg::License();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::license_msg::License::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_License_license_5fmsg_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_License_license_5fmsg_2eproto}, {}};

static void InitDefaultsscc_info_LicenseInfo_license_5fmsg_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::license_msg::_LicenseInfo_default_instance_;
    new (ptr) ::license_msg::LicenseInfo();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::license_msg::LicenseInfo::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_LicenseInfo_license_5fmsg_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_LicenseInfo_license_5fmsg_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_license_5fmsg_2eproto[2];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_license_5fmsg_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_license_5fmsg_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_license_5fmsg_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::license_msg::License, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::license_msg::License, type_),
  PROTOBUF_FIELD_OFFSET(::license_msg::License, check_),
  PROTOBUF_FIELD_OFFSET(::license_msg::License, info_len_),
  PROTOBUF_FIELD_OFFSET(::license_msg::License, info_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, cmd_ip_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, cmd_port_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, data_ip_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, data_port_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, box_id_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, pwd_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, manufactor_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, begin_time_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, end_time_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, release_id_),
  PROTOBUF_FIELD_OFFSET(::license_msg::LicenseInfo, jwumq_id_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::license_msg::License)},
  { 9, -1, sizeof(::license_msg::LicenseInfo)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::license_msg::_License_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::license_msg::_LicenseInfo_default_instance_),
};

const char descriptor_table_protodef_license_5fmsg_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\021license_msg.proto\022\013license_msg\"`\n\007Lice"
  "nse\022&\n\004type\030\001 \001(\0162\030.license_msg.LicenseT"
  "ype\022\r\n\005check\030\002 \001(\r\022\020\n\010info_len\030\003 \001(\r\022\014\n\004"
  "info\030\004 \001(\014\"\320\001\n\013LicenseInfo\022\016\n\006cmd_ip\030\001 \001"
  "(\t\022\020\n\010cmd_port\030\002 \001(\r\022\017\n\007data_ip\030\003 \001(\t\022\021\n"
  "\tdata_port\030\004 \001(\r\022\016\n\006box_id\030\005 \001(\t\022\013\n\003pwd\030"
  "\006 \001(\t\022\022\n\nmanufactor\030\007 \001(\t\022\022\n\nbegin_time\030"
  "\010 \001(\004\022\020\n\010end_time\030\t \001(\004\022\022\n\nrelease_id\030\n "
  "\001(\r\022\020\n\010jwumq_id\030\013 \001(\t*\037\n\013LicenseType\022\007\n\003"
  "dev\020\000\022\007\n\003atu\020\001b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_license_5fmsg_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_license_5fmsg_2eproto_sccs[2] = {
  &scc_info_License_license_5fmsg_2eproto.base,
  &scc_info_LicenseInfo_license_5fmsg_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_license_5fmsg_2eproto_once;
static bool descriptor_table_license_5fmsg_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_license_5fmsg_2eproto = {
  &descriptor_table_license_5fmsg_2eproto_initialized, descriptor_table_protodef_license_5fmsg_2eproto, "license_msg.proto", 382,
  &descriptor_table_license_5fmsg_2eproto_once, descriptor_table_license_5fmsg_2eproto_sccs, descriptor_table_license_5fmsg_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_license_5fmsg_2eproto::offsets,
  file_level_metadata_license_5fmsg_2eproto, 2, file_level_enum_descriptors_license_5fmsg_2eproto, file_level_service_descriptors_license_5fmsg_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_license_5fmsg_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_license_5fmsg_2eproto), true);
namespace license_msg {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* LicenseType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_license_5fmsg_2eproto);
  return file_level_enum_descriptors_license_5fmsg_2eproto[0];
}
bool LicenseType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void License::InitAsDefaultInstance() {
}
class License::_Internal {
 public:
};

License::License()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:license_msg.License)
}
License::License(const License& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  info_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_info().empty()) {
    info_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.info_);
  }
  ::memcpy(&type_, &from.type_,
    static_cast<size_t>(reinterpret_cast<char*>(&info_len_) -
    reinterpret_cast<char*>(&type_)) + sizeof(info_len_));
  // @@protoc_insertion_point(copy_constructor:license_msg.License)
}

void License::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_License_license_5fmsg_2eproto.base);
  info_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&info_len_) -
      reinterpret_cast<char*>(&type_)) + sizeof(info_len_));
}

License::~License() {
  // @@protoc_insertion_point(destructor:license_msg.License)
  SharedDtor();
}

void License::SharedDtor() {
  info_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void License::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const License& License::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_License_license_5fmsg_2eproto.base);
  return *internal_default_instance();
}


void License::Clear() {
// @@protoc_insertion_point(message_clear_start:license_msg.License)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  info_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&info_len_) -
      reinterpret_cast<char*>(&type_)) + sizeof(info_len_));
  _internal_metadata_.Clear();
}

const char* License::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .license_msg.LicenseType type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
          _internal_set_type(static_cast<::license_msg::LicenseType>(val));
        } else goto handle_unusual;
        continue;
      // uint32 check = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          check_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 info_len = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          info_len_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes info = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          auto str = _internal_mutable_info();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* License::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:license_msg.License)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .license_msg.LicenseType type = 1;
  if (this->type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_type(), target);
  }

  // uint32 check = 2;
  if (this->check() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_check(), target);
  }

  // uint32 info_len = 3;
  if (this->info_len() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_info_len(), target);
  }

  // bytes info = 4;
  if (this->info().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        4, this->_internal_info(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:license_msg.License)
  return target;
}

size_t License::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:license_msg.License)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes info = 4;
  if (this->info().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_info());
  }

  // .license_msg.LicenseType type = 1;
  if (this->type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_type());
  }

  // uint32 check = 2;
  if (this->check() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_check());
  }

  // uint32 info_len = 3;
  if (this->info_len() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_info_len());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void License::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:license_msg.License)
  GOOGLE_DCHECK_NE(&from, this);
  const License* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<License>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:license_msg.License)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:license_msg.License)
    MergeFrom(*source);
  }
}

void License::MergeFrom(const License& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:license_msg.License)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.info().size() > 0) {

    info_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.info_);
  }
  if (from.type() != 0) {
    _internal_set_type(from._internal_type());
  }
  if (from.check() != 0) {
    _internal_set_check(from._internal_check());
  }
  if (from.info_len() != 0) {
    _internal_set_info_len(from._internal_info_len());
  }
}

void License::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:license_msg.License)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void License::CopyFrom(const License& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:license_msg.License)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool License::IsInitialized() const {
  return true;
}

void License::InternalSwap(License* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  info_.Swap(&other->info_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(type_, other->type_);
  swap(check_, other->check_);
  swap(info_len_, other->info_len_);
}

::PROTOBUF_NAMESPACE_ID::Metadata License::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void LicenseInfo::InitAsDefaultInstance() {
}
class LicenseInfo::_Internal {
 public:
};

LicenseInfo::LicenseInfo()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:license_msg.LicenseInfo)
}
LicenseInfo::LicenseInfo(const LicenseInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  cmd_ip_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_cmd_ip().empty()) {
    cmd_ip_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.cmd_ip_);
  }
  data_ip_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_data_ip().empty()) {
    data_ip_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.data_ip_);
  }
  box_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_box_id().empty()) {
    box_id_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.box_id_);
  }
  pwd_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_pwd().empty()) {
    pwd_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.pwd_);
  }
  manufactor_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_manufactor().empty()) {
    manufactor_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.manufactor_);
  }
  jwumq_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_jwumq_id().empty()) {
    jwumq_id_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.jwumq_id_);
  }
  ::memcpy(&cmd_port_, &from.cmd_port_,
    static_cast<size_t>(reinterpret_cast<char*>(&release_id_) -
    reinterpret_cast<char*>(&cmd_port_)) + sizeof(release_id_));
  // @@protoc_insertion_point(copy_constructor:license_msg.LicenseInfo)
}

void LicenseInfo::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_LicenseInfo_license_5fmsg_2eproto.base);
  cmd_ip_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  data_ip_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  box_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  pwd_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  manufactor_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  jwumq_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&cmd_port_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&release_id_) -
      reinterpret_cast<char*>(&cmd_port_)) + sizeof(release_id_));
}

LicenseInfo::~LicenseInfo() {
  // @@protoc_insertion_point(destructor:license_msg.LicenseInfo)
  SharedDtor();
}

void LicenseInfo::SharedDtor() {
  cmd_ip_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  data_ip_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  box_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  pwd_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  manufactor_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  jwumq_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void LicenseInfo::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const LicenseInfo& LicenseInfo::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_LicenseInfo_license_5fmsg_2eproto.base);
  return *internal_default_instance();
}


void LicenseInfo::Clear() {
// @@protoc_insertion_point(message_clear_start:license_msg.LicenseInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cmd_ip_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  data_ip_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  box_id_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  pwd_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  manufactor_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  jwumq_id_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&cmd_port_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&release_id_) -
      reinterpret_cast<char*>(&cmd_port_)) + sizeof(release_id_));
  _internal_metadata_.Clear();
}

const char* LicenseInfo::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string cmd_ip = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_cmd_ip();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.cmd_ip"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 cmd_port = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          cmd_port_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string data_ip = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_data_ip();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.data_ip"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 data_port = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          data_port_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string box_id = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_box_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.box_id"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string pwd = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50)) {
          auto str = _internal_mutable_pwd();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.pwd"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string manufactor = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_manufactor();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.manufactor"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint64 begin_time = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          begin_time_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint64 end_time = 9;
      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          end_time_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 release_id = 10;
      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          release_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string jwumq_id = 11;
      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          auto str = _internal_mutable_jwumq_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "license_msg.LicenseInfo.jwumq_id"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* LicenseInfo::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:license_msg.LicenseInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string cmd_ip = 1;
  if (this->cmd_ip().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_cmd_ip().data(), static_cast<int>(this->_internal_cmd_ip().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.cmd_ip");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_cmd_ip(), target);
  }

  // uint32 cmd_port = 2;
  if (this->cmd_port() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_cmd_port(), target);
  }

  // string data_ip = 3;
  if (this->data_ip().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_data_ip().data(), static_cast<int>(this->_internal_data_ip().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.data_ip");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_data_ip(), target);
  }

  // uint32 data_port = 4;
  if (this->data_port() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_data_port(), target);
  }

  // string box_id = 5;
  if (this->box_id().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_box_id().data(), static_cast<int>(this->_internal_box_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.box_id");
    target = stream->WriteStringMaybeAliased(
        5, this->_internal_box_id(), target);
  }

  // string pwd = 6;
  if (this->pwd().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_pwd().data(), static_cast<int>(this->_internal_pwd().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.pwd");
    target = stream->WriteStringMaybeAliased(
        6, this->_internal_pwd(), target);
  }

  // string manufactor = 7;
  if (this->manufactor().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_manufactor().data(), static_cast<int>(this->_internal_manufactor().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.manufactor");
    target = stream->WriteStringMaybeAliased(
        7, this->_internal_manufactor(), target);
  }

  // uint64 begin_time = 8;
  if (this->begin_time() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(8, this->_internal_begin_time(), target);
  }

  // uint64 end_time = 9;
  if (this->end_time() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(9, this->_internal_end_time(), target);
  }

  // uint32 release_id = 10;
  if (this->release_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(10, this->_internal_release_id(), target);
  }

  // string jwumq_id = 11;
  if (this->jwumq_id().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_jwumq_id().data(), static_cast<int>(this->_internal_jwumq_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "license_msg.LicenseInfo.jwumq_id");
    target = stream->WriteStringMaybeAliased(
        11, this->_internal_jwumq_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:license_msg.LicenseInfo)
  return target;
}

size_t LicenseInfo::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:license_msg.LicenseInfo)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string cmd_ip = 1;
  if (this->cmd_ip().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_cmd_ip());
  }

  // string data_ip = 3;
  if (this->data_ip().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_data_ip());
  }

  // string box_id = 5;
  if (this->box_id().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_box_id());
  }

  // string pwd = 6;
  if (this->pwd().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_pwd());
  }

  // string manufactor = 7;
  if (this->manufactor().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_manufactor());
  }

  // string jwumq_id = 11;
  if (this->jwumq_id().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_jwumq_id());
  }

  // uint32 cmd_port = 2;
  if (this->cmd_port() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_cmd_port());
  }

  // uint32 data_port = 4;
  if (this->data_port() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_data_port());
  }

  // uint64 begin_time = 8;
  if (this->begin_time() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_begin_time());
  }

  // uint64 end_time = 9;
  if (this->end_time() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_end_time());
  }

  // uint32 release_id = 10;
  if (this->release_id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_release_id());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void LicenseInfo::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:license_msg.LicenseInfo)
  GOOGLE_DCHECK_NE(&from, this);
  const LicenseInfo* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<LicenseInfo>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:license_msg.LicenseInfo)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:license_msg.LicenseInfo)
    MergeFrom(*source);
  }
}

void LicenseInfo::MergeFrom(const LicenseInfo& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:license_msg.LicenseInfo)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.cmd_ip().size() > 0) {

    cmd_ip_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.cmd_ip_);
  }
  if (from.data_ip().size() > 0) {

    data_ip_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.data_ip_);
  }
  if (from.box_id().size() > 0) {

    box_id_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.box_id_);
  }
  if (from.pwd().size() > 0) {

    pwd_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.pwd_);
  }
  if (from.manufactor().size() > 0) {

    manufactor_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.manufactor_);
  }
  if (from.jwumq_id().size() > 0) {

    jwumq_id_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.jwumq_id_);
  }
  if (from.cmd_port() != 0) {
    _internal_set_cmd_port(from._internal_cmd_port());
  }
  if (from.data_port() != 0) {
    _internal_set_data_port(from._internal_data_port());
  }
  if (from.begin_time() != 0) {
    _internal_set_begin_time(from._internal_begin_time());
  }
  if (from.end_time() != 0) {
    _internal_set_end_time(from._internal_end_time());
  }
  if (from.release_id() != 0) {
    _internal_set_release_id(from._internal_release_id());
  }
}

void LicenseInfo::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:license_msg.LicenseInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void LicenseInfo::CopyFrom(const LicenseInfo& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:license_msg.LicenseInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool LicenseInfo::IsInitialized() const {
  return true;
}

void LicenseInfo::InternalSwap(LicenseInfo* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  cmd_ip_.Swap(&other->cmd_ip_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  data_ip_.Swap(&other->data_ip_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  box_id_.Swap(&other->box_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  pwd_.Swap(&other->pwd_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  manufactor_.Swap(&other->manufactor_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  jwumq_id_.Swap(&other->jwumq_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(cmd_port_, other->cmd_port_);
  swap(data_port_, other->data_port_);
  swap(begin_time_, other->begin_time_);
  swap(end_time_, other->end_time_);
  swap(release_id_, other->release_id_);
}

::PROTOBUF_NAMESPACE_ID::Metadata LicenseInfo::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace license_msg
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::license_msg::License* Arena::CreateMaybeMessage< ::license_msg::License >(Arena* arena) {
  return Arena::CreateInternal< ::license_msg::License >(arena);
}
template<> PROTOBUF_NOINLINE ::license_msg::LicenseInfo* Arena::CreateMaybeMessage< ::license_msg::LicenseInfo >(Arena* arena) {
  return Arena::CreateInternal< ::license_msg::LicenseInfo >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
