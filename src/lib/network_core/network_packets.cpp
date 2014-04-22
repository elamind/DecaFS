#include "network_packets.h"

static int next_seq_num = 1;

// -------------------- NetworkPacket --------------------------

// serialization
Packet::Packet(uint32_t flag, int derived_size) : flag(flag) {

  seq_num = next_seq_num++;
  packet_size = header_size + derived_size;
  packet = (char*)malloc(packet_size);

  std::cout << "Packet Created: size: " <<  packet_size << std::endl;

  // set packet header data
  uint32_t* ptr = (uint32_t*)packet;
  ptr[0] = packet_size;
  ptr[1] = seq_num;
  ptr[2] = flag;
}

// deserialization
Packet::Packet(void* buf, ssize_t size) :
  packet_size(size), packet(buf) {

  packet_size = ((uint32_t*)buf)[0];
  seq_num = ((uint32_t*)buf)[1];
  flag = ((uint32_t*)buf)[2];
}

int Packet::dataSize() {
  return header_size;
}

std::ostream& Packet::print(std::ostream& stream) const {

  stream << "Packet" << std::endl;
  stream << "\tpacket_size: " << packet_size << std::endl;
  stream << "\tseq_num: " << seq_num << std::endl;
  stream << "\tflag: " << flag << std::endl;
  return stream; 
}

std::ostream& operator<<(std::ostream& stream, const Packet &p) {
  return p.print(stream); 
}

// ---------------------- FilePacket --------------------

// constructor from data
// serialization
FilePacket::FilePacket(int flag, int derived_size, uint32_t fd, uint32_t file_id, 
 uint32_t stripe_id, uint32_t chunk_num, uint32_t offset, uint32_t count) 
 : Packet(flag, derived_size + data_size),
   fd(fd),
   file_id(file_id),
   stripe_id(stripe_id),
   chunk_num(chunk_num),
   offset(offset),
   count(count) {

  uint32_t* base = (uint32_t*)(((uint8_t*)packet) + Packet::dataSize());
  base[0] = fd;
  base[1] = file_id;
  base[2] = stripe_id;
  base[3] = chunk_num;
  base[4] = offset;
  base[5] = count;
}

// constructor from a buffer 
// deserialization
FilePacket::FilePacket(void* buf, ssize_t size) : Packet(buf, size) {

  uint32_t* base = (uint32_t*)(((uint8_t*)buf) + Packet::dataSize());

  fd = base[0];
  file_id = base[1];
  stripe_id = base[2];
  chunk_num = base[3];
  offset = base[4];
  count = base[5];
}

int FilePacket::dataSize() {

  return Packet::dataSize() + data_size;
}

std::ostream& FilePacket::print(std::ostream& stream) const {

  stream << "FilePacket" << std::endl;
  stream << "\tfd: " << fd << std::endl;
  stream << "\tfile_id: " << file_id << std::endl;
  stream << "\tstripe_id: " << stripe_id << std::endl;
  stream << "\tchunk_num: " << chunk_num << std::endl;
  stream << "\toffset: " << offset << std::endl;
  stream << "\tcount: " << count << std::endl;
  return Packet::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const FilePacket &packet) {
  return packet.print(stream);
}

// ---------------------- FileDataPacket ------------


FileDataPacket::FileDataPacket(void* buf, ssize_t size) : FilePacket(buf, size) {

  data_buffer = ((uint8_t*)buf) + FilePacket::dataSize();
}

FileDataPacket::FileDataPacket(int flag, int derived_size, uint32_t fd, uint32_t file_id, 
 uint32_t stripe_id, uint32_t chunk_num, uint32_t offset, uint32_t count, uint8_t* buf) 
 : FilePacket(flag, count, fd, file_id, stripe_id, chunk_num, offset, count) {

  data_buffer = buf;

  uint8_t* data_loc = ((uint8_t*)packet) + FilePacket::dataSize();

  memcpy(data_loc, buf, count);
  // TODO do i need to free buf?
  // or is it the creators responsibility?
}

int FileDataPacket::dataSize() {

  return FilePacket::dataSize() + data_size;
}

std::ostream& FileDataPacket::print(std::ostream& stream) const {

  stream << "FileDataPacket" << std::endl;
  stream << "data_buffer: ";
  stream.write((const char*)data_buffer, count);
  stream << std::endl;

  return FilePacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const FileDataPacket &packet) {
  return packet.print(stream);
}

// ------------------------ ReadChunkPacket ------------------

ReadChunkRequest::ReadChunkRequest(void* buf, ssize_t packet_size) 
 : FilePacket(buf, packet_size) {

}

ReadChunkRequest::ReadChunkRequest(uint32_t fd, uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num, uint32_t offset, uint32_t count) 
 : FilePacket(READ_CHUNK, 0, fd, file_id, stripe_id, chunk_num, offset, count) {

}

std::ostream& ReadChunkRequest::print(std::ostream& stream) const {

  stream << "ReadChunkRequest" << std::endl;
  return FilePacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const ReadChunkRequest &req) {
  return req.print(stream);
}

// ------------------------ WriteChunkResponse ------------------

WriteChunkResponse::WriteChunkResponse(void* buf, ssize_t packet_size) 
 : FilePacket(buf, packet_size) {

}

WriteChunkResponse::WriteChunkResponse(uint32_t fd, uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num, uint32_t offset, uint32_t count) 
 : FilePacket(WRITE_CHUNK_RESPONSE, 0, fd, file_id, stripe_id, chunk_num, offset, count) {

}

std::ostream& WriteChunkResponse::print(std::ostream& stream) const {

  stream << "WriteChunkResponse" << std::endl;
  return FilePacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const WriteChunkResponse &req) {
  return req.print(stream);
}

// ------------------------ WriteChunkPacket ---------------------------

WriteChunkRequest::WriteChunkRequest(void* buf, ssize_t packet_size) 
 : FileDataPacket(buf, packet_size) {

}

WriteChunkRequest::WriteChunkRequest(uint32_t fd, uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num, uint32_t offset, uint32_t count, uint8_t * buf) 
 : FileDataPacket(WRITE_CHUNK, 0, fd, file_id, stripe_id, chunk_num, offset, count, buf) {

}

std::ostream& WriteChunkRequest::print(std::ostream& stream) const {

  stream << "WriteChunkRequest" << std::endl;
  return FileDataPacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const WriteChunkRequest &req) {
  return req.print(stream);
}

// ------------------------ ReadChunkResponse ---------------------------

ReadChunkResponse::ReadChunkResponse(void* buf, ssize_t packet_size) 
 : FileDataPacket(buf, packet_size) {

}

ReadChunkResponse::ReadChunkResponse(uint32_t fd, uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num, uint32_t offset, uint32_t count, uint8_t * buf) 
 : FileDataPacket(READ_CHUNK_RESPONSE, 0, fd, file_id, stripe_id, chunk_num, offset, count, buf) {

}

std::ostream& ReadChunkResponse::print(std::ostream& stream) const {

  stream << "ReadChunkResponse" << std::endl;
  return FileDataPacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const ReadChunkResponse &req) {
  return req.print(stream);
}

// ------------------------------- DeleteChunkPacket -----------------------------

DeleteChunkRequest::DeleteChunkRequest(void* buf, ssize_t size)
 : FilePacket(buf, size) {

}

DeleteChunkRequest::DeleteChunkRequest(uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num)
 : FilePacket(DELETE_CHUNK, 0, fd, file_id, stripe_id, chunk_num, -1, -1) {

}

std::ostream& DeleteChunkRequest::print(std::ostream& stream) const {

  stream << "DeleteChunkRequest" << std::endl;
  return FilePacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const DeleteChunkRequest &req) {
  return req.print(stream);
}

// ------------------------------- DeleteChunkResponse -----------------------------

DeleteChunkResponse::DeleteChunkResponse(void* buf, ssize_t size)
 : FilePacket(buf, size) {

}

DeleteChunkResponse::DeleteChunkResponse(uint32_t file_id, uint32_t stripe_id,
 uint32_t chunk_num)
 : FilePacket(DELETE_CHUNK_RESPONSE, 0, fd, file_id, stripe_id, chunk_num, -1, -1) {

}

std::ostream& DeleteChunkResponse::print(std::ostream& stream) const {

  stream << "DeleteChunkResponse" << std::endl;
  return FilePacket::print(stream);
}

std::ostream& operator<<(std::ostream& stream, const DeleteChunkResponse &req) {
  return req.print(stream);
}