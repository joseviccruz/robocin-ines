from google.protobuf.message import Message as Protobuf

import zlib

# Magic number when reading and writing protocol buffers.
MAGIC_NUMBER = 0x3ed7230a


class RecordWriter:
    def __init__(self, file):
        self.file = open(file, 'wb+')
        self.use_compression = True

    def write_protocol_message(self, proto: Protobuf) -> bool:
        uncompressed_buffer = proto.SerializeToString()
        uncompressed_size = len(uncompressed_buffer)
        compressed_buffer = b""
        compressed_size = 0

        if self.use_compression:
            compressed_buffer = self.compress(uncompressed_buffer)
            if compressed_buffer is None:
                return False
            compressed_size = len(compressed_buffer)

        if not self.file.write(MAGIC_NUMBER.to_bytes(4, "little")):
            return False
        if not self.file.write(uncompressed_size.to_bytes(8, "little")):
            return False
        if not self.file.write(compressed_size.to_bytes(8, "little")):
            return False
        if self.use_compression:
            if not self.file.write(compressed_buffer):
                return False
        else:
            if not self.file.write(uncompressed_buffer):
                return False

        return True

    def close(self):
        return self.file.close()

    def set_use_compression(self, use_compression):
        self.use_compression = use_compression

    def compress(self, s):
        compressed_data = zlib.compress(s)
        return compressed_data


class RecordReader:
    def __init__(self, file):
        self.file = open(file, 'rb+')

    def __iter__(self):
        return self

    def __next__(self):
        while True:
            header = self.file.read(20)
            if not header:
                break

            magic_number = int.from_bytes(header[:4], "little")
            if magic_number != MAGIC_NUMBER:
                return None

            uncompressed_size = int.from_bytes(header[4:12], "little")
            compressed_size = int.from_bytes(header[12:], "little")
            compressed_data = self.file.read(compressed_size)

            data = None

            if compressed_data != 0:
                data = zlib.decompress(compressed_data)
                if len(data) != uncompressed_size:
                    return None
            else:
                data = compressed_data

            return data

        raise StopIteration
