# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: robocin/pb/ssl/third_party/detection/raw_wrapper.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from robocin.pb.ssl.third_party.detection import geometry_pb2 as robocin_dot_pb_dot_ssl_dot_third__party_dot_detection_dot_geometry__pb2
from robocin.pb.ssl.third_party.detection import raw_pb2 as robocin_dot_pb_dot_ssl_dot_third__party_dot_detection_dot_raw__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n6robocin/pb/ssl/third_party/detection/raw_wrapper.proto\x12$robocin.pb.ssl.third_party.detection\x1a\x33robocin/pb/ssl/third_party/detection/geometry.proto\x1a.robocin/pb/ssl/third_party/detection/raw.proto\"\xaa\x01\n\x11SSL_WrapperPacket\x12K\n\tdetection\x18\x01 \x01(\x0b\x32\x38.robocin.pb.ssl.third_party.detection.SSL_DetectionFrame\x12H\n\x08geometry\x18\x02 \x01(\x0b\x32\x36.robocin.pb.ssl.third_party.detection.SSL_GeometryData')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'robocin.pb.ssl.third_party.detection.raw_wrapper_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _globals['_SSL_WRAPPERPACKET']._serialized_start=198
  _globals['_SSL_WRAPPERPACKET']._serialized_end=368
# @@protoc_insertion_point(module_scope)
