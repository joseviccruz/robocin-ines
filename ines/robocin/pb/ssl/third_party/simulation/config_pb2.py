# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: robocin/pb/ssl/third_party/simulation/config.proto
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.protobuf import any_pb2 as google_dot_protobuf_dot_any__pb2
from robocin.pb.ssl.third_party.detection import geometry_pb2 as robocin_dot_pb_dot_ssl_dot_third__party_dot_detection_dot_geometry__pb2
from robocin.pb.ssl.third_party.game_controller import common_pb2 as robocin_dot_pb_dot_ssl_dot_third__party_dot_game__controller_dot_common__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n2robocin/pb/ssl/third_party/simulation/config.proto\x12%robocin.pb.ssl.third_party.simulation\x1a\x19google/protobuf/any.proto\x1a\x33robocin/pb/ssl/third_party/detection/geometry.proto\x1a\x37robocin/pb/ssl/third_party/game_controller/common.proto\"\xc2\x01\n\x0bRobotLimits\x12 \n\x18\x61\x63\x63_speedup_absolute_max\x18\x01 \x01(\x02\x12\x1f\n\x17\x61\x63\x63_speedup_angular_max\x18\x02 \x01(\x02\x12\x1e\n\x16\x61\x63\x63_brake_absolute_max\x18\x03 \x01(\x02\x12\x1d\n\x15\x61\x63\x63_brake_angular_max\x18\x04 \x01(\x02\x12\x18\n\x10vel_absolute_max\x18\x05 \x01(\x02\x12\x17\n\x0fvel_angular_max\x18\x06 \x01(\x02\"b\n\x10RobotWheelAngles\x12\x13\n\x0b\x66ront_right\x18\x01 \x02(\x02\x12\x12\n\nback_right\x18\x02 \x02(\x02\x12\x11\n\tback_left\x18\x03 \x02(\x02\x12\x12\n\nfront_left\x18\x04 \x02(\x02\"\x98\x03\n\nRobotSpecs\x12?\n\x02id\x18\x01 \x02(\x0b\x32\x33.robocin.pb.ssl.third_party.game_controller.RobotId\x12\x14\n\x06radius\x18\x02 \x01(\x02:\x04\x30.09\x12\x14\n\x06height\x18\x03 \x01(\x02:\x04\x30.15\x12\x0c\n\x04mass\x18\x04 \x01(\x02\x12\x1d\n\x15max_linear_kick_speed\x18\x07 \x01(\x02\x12\x1b\n\x13max_chip_kick_speed\x18\x08 \x01(\x02\x12\x1a\n\x12\x63\x65nter_to_dribbler\x18\t \x01(\x02\x12\x42\n\x06limits\x18\n \x01(\x0b\x32\x32.robocin.pb.ssl.third_party.simulation.RobotLimits\x12M\n\x0cwheel_angles\x18\r \x01(\x0b\x32\x37.robocin.pb.ssl.third_party.simulation.RobotWheelAngles\x12$\n\x06\x63ustom\x18\x0e \x03(\x0b\x32\x14.google.protobuf.Any\"5\n\rRealismConfig\x12$\n\x06\x63ustom\x18\x01 \x03(\x0b\x32\x14.google.protobuf.Any\"\x86\x02\n\x0fSimulatorConfig\x12H\n\x08geometry\x18\x01 \x01(\x0b\x32\x36.robocin.pb.ssl.third_party.detection.SSL_GeometryData\x12\x46\n\x0brobot_specs\x18\x02 \x03(\x0b\x32\x31.robocin.pb.ssl.third_party.simulation.RobotSpecs\x12L\n\x0erealism_config\x18\x03 \x01(\x0b\x32\x34.robocin.pb.ssl.third_party.simulation.RealismConfig\x12\x13\n\x0bvision_port\x18\x04 \x01(\r')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'robocin.pb.ssl.third_party.simulation.config_pb2', _globals)
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _globals['_ROBOTLIMITS']._serialized_start=231
  _globals['_ROBOTLIMITS']._serialized_end=425
  _globals['_ROBOTWHEELANGLES']._serialized_start=427
  _globals['_ROBOTWHEELANGLES']._serialized_end=525
  _globals['_ROBOTSPECS']._serialized_start=528
  _globals['_ROBOTSPECS']._serialized_end=936
  _globals['_REALISMCONFIG']._serialized_start=938
  _globals['_REALISMCONFIG']._serialized_end=991
  _globals['_SIMULATORCONFIG']._serialized_start=994
  _globals['_SIMULATORCONFIG']._serialized_end=1256
# @@protoc_insertion_point(module_scope)
