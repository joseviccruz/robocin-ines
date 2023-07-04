import time

from google.protobuf.timestamp_pb2 import Timestamp

SECONDS_TO_NANOSECONDS = 1_000_000_000


def timestamp_from_unix_seconds(secs: float) -> Timestamp:
    floating_point_unix_seconds = secs

    seconds = int(floating_point_unix_seconds)
    nanos = int(floating_point_unix_seconds * SECONDS_TO_NANOSECONDS)

    return Timestamp(seconds=seconds, nanos=nanos)


def now() -> Timestamp:
    floating_point_unix_nanos = time.time_ns()

    seconds = int(floating_point_unix_nanos / SECONDS_TO_NANOSECONDS)
    nanos = int(floating_point_unix_nanos % SECONDS_TO_NANOSECONDS)

    return Timestamp(seconds=seconds, nanos=nanos)


def unix_seconds_from_timestamp(timestamp: Timestamp) -> float:
    return timestamp.seconds + timestamp.nanos / SECONDS_TO_NANOSECONDS
