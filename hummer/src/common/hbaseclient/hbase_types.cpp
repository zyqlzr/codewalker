/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "hbase_types.h"

namespace apache { namespace hadoop { namespace hbase { namespace thrift {

int _kAssocVisibilityValues[] = {
  AssocVisibility::VISIBLE,
  AssocVisibility::DELETED,
  AssocVisibility::UNUSED,
  AssocVisibility::HIDDEN,
  AssocVisibility::HARD_DELETE
};
const char* _kAssocVisibilityNames[] = {
  "VISIBLE",
  "DELETED",
  "UNUSED",
  "HIDDEN",
  "HARD_DELETE"
};
const std::map<int, const char*> _AssocVisibility_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(5, _kAssocVisibilityValues, _kAssocVisibilityNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

const char* TCell::ascii_fingerprint = "1CCCF6FC31CFD1D61BBBB1BAF3590620";
const uint8_t TCell::binary_fingerprint[16] = {0x1C,0xCC,0xF6,0xFC,0x31,0xCF,0xD1,0xD6,0x1B,0xBB,0xB1,0xBA,0xF3,0x59,0x06,0x20};

uint32_t TCell::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->value);
          this->__isset.value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->timestamp);
          this->__isset.timestamp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TCell::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("TCell");
  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeBinary(this->value);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("timestamp", ::apache::thrift::protocol::T_I64, 2);
  xfer += oprot->writeI64(this->timestamp);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* ColumnDescriptor::ascii_fingerprint = "3B18638852FDF9DD911BC1174265F92E";
const uint8_t ColumnDescriptor::binary_fingerprint[16] = {0x3B,0x18,0x63,0x88,0x52,0xFD,0xF9,0xDD,0x91,0x1B,0xC1,0x17,0x42,0x65,0xF9,0x2E};

uint32_t ColumnDescriptor::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->name);
          this->__isset.name = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->maxVersions);
          this->__isset.maxVersions = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->compression);
          this->__isset.compression = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->inMemory);
          this->__isset.inMemory = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->bloomFilterType);
          this->__isset.bloomFilterType = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->bloomFilterVectorSize);
          this->__isset.bloomFilterVectorSize = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->bloomFilterNbHashes);
          this->__isset.bloomFilterNbHashes = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 8:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->blockCacheEnabled);
          this->__isset.blockCacheEnabled = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 9:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->timeToLive);
          this->__isset.timeToLive = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t ColumnDescriptor::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("ColumnDescriptor");
  xfer += oprot->writeFieldBegin("name", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeBinary(this->name);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("maxVersions", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->maxVersions);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("compression", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeString(this->compression);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("inMemory", ::apache::thrift::protocol::T_BOOL, 4);
  xfer += oprot->writeBool(this->inMemory);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("bloomFilterType", ::apache::thrift::protocol::T_STRING, 5);
  xfer += oprot->writeString(this->bloomFilterType);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("bloomFilterVectorSize", ::apache::thrift::protocol::T_I32, 6);
  xfer += oprot->writeI32(this->bloomFilterVectorSize);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("bloomFilterNbHashes", ::apache::thrift::protocol::T_I32, 7);
  xfer += oprot->writeI32(this->bloomFilterNbHashes);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("blockCacheEnabled", ::apache::thrift::protocol::T_BOOL, 8);
  xfer += oprot->writeBool(this->blockCacheEnabled);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("timeToLive", ::apache::thrift::protocol::T_I32, 9);
  xfer += oprot->writeI32(this->timeToLive);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* TRegionInfo::ascii_fingerprint = "B58AB7A239831F8614F7B6709C89DC7B";
const uint8_t TRegionInfo::binary_fingerprint[16] = {0xB5,0x8A,0xB7,0xA2,0x39,0x83,0x1F,0x86,0x14,0xF7,0xB6,0x70,0x9C,0x89,0xDC,0x7B};

uint32_t TRegionInfo::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->startKey);
          this->__isset.startKey = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->endKey);
          this->__isset.endKey = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->id);
          this->__isset.id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->name);
          this->__isset.name = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte(this->version);
          this->__isset.version = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->serverName);
          this->__isset.serverName = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->port);
          this->__isset.port = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TRegionInfo::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("TRegionInfo");
  xfer += oprot->writeFieldBegin("startKey", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeBinary(this->startKey);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("endKey", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeBinary(this->endKey);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("id", ::apache::thrift::protocol::T_I64, 3);
  xfer += oprot->writeI64(this->id);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("name", ::apache::thrift::protocol::T_STRING, 4);
  xfer += oprot->writeBinary(this->name);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("version", ::apache::thrift::protocol::T_BYTE, 5);
  xfer += oprot->writeByte(this->version);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("serverName", ::apache::thrift::protocol::T_STRING, 6);
  xfer += oprot->writeBinary(this->serverName);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("port", ::apache::thrift::protocol::T_I32, 7);
  xfer += oprot->writeI32(this->port);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* Mutation::ascii_fingerprint = "ECAE90B49C4CD40E8A134DAC8E9E8EF7";
const uint8_t Mutation::binary_fingerprint[16] = {0xEC,0xAE,0x90,0xB4,0x9C,0x4C,0xD4,0x0E,0x8A,0x13,0x4D,0xAC,0x8E,0x9E,0x8E,0xF7};

uint32_t Mutation::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->isDelete);
          this->__isset.isDelete = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->column);
          this->__isset.column = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->value);
          this->__isset.value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->writeToWAL);
          this->__isset.writeToWAL = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->timestamp);
          this->__isset.timestamp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Mutation::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("Mutation");
  xfer += oprot->writeFieldBegin("isDelete", ::apache::thrift::protocol::T_BOOL, 1);
  xfer += oprot->writeBool(this->isDelete);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("column", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeBinary(this->column);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeBinary(this->value);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("writeToWAL", ::apache::thrift::protocol::T_BOOL, 4);
  xfer += oprot->writeBool(this->writeToWAL);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("timestamp", ::apache::thrift::protocol::T_I64, 5);
  xfer += oprot->writeI64(this->timestamp);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* BatchMutation::ascii_fingerprint = "9D5FFA15917400B819764E7C4E4C6133";
const uint8_t BatchMutation::binary_fingerprint[16] = {0x9D,0x5F,0xFA,0x15,0x91,0x74,0x00,0xB8,0x19,0x76,0x4E,0x7C,0x4E,0x4C,0x61,0x33};

uint32_t BatchMutation::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->row);
          this->__isset.row = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->mutations.clear();
            uint32_t _size0;
            ::apache::thrift::protocol::TType _etype3;
            iprot->readListBegin(_etype3, _size0);
            this->mutations.resize(_size0);
            uint32_t _i4;
            for (_i4 = 0; _i4 < _size0; ++_i4)
            {
              xfer += this->mutations[_i4].read(iprot);
            }
            iprot->readListEnd();
          }
          this->__isset.mutations = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t BatchMutation::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("BatchMutation");
  xfer += oprot->writeFieldBegin("row", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeBinary(this->row);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("mutations", ::apache::thrift::protocol::T_LIST, 2);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRUCT, static_cast<uint32_t>(this->mutations.size()));
    std::vector<Mutation> ::const_iterator _iter5;
    for (_iter5 = this->mutations.begin(); _iter5 != this->mutations.end(); ++_iter5)
    {
      xfer += (*_iter5).write(oprot);
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* TRowResult::ascii_fingerprint = "AE98EA4F344566FAFE04FA5E5823D1ED";
const uint8_t TRowResult::binary_fingerprint[16] = {0xAE,0x98,0xEA,0x4F,0x34,0x45,0x66,0xFA,0xFE,0x04,0xFA,0x5E,0x58,0x23,0xD1,0xED};

uint32_t TRowResult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->row);
          this->__isset.row = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_MAP) {
          {
            this->columns.clear();
            uint32_t _size6;
            ::apache::thrift::protocol::TType _ktype7;
            ::apache::thrift::protocol::TType _vtype8;
            iprot->readMapBegin(_ktype7, _vtype8, _size6);
            uint32_t _i10;
            for (_i10 = 0; _i10 < _size6; ++_i10)
            {
              Text _key11;
              xfer += iprot->readBinary(_key11);
              TCell& _val12 = this->columns[_key11];
              xfer += _val12.read(iprot);
            }
            iprot->readMapEnd();
          }
          this->__isset.columns = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TRowResult::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("TRowResult");
  xfer += oprot->writeFieldBegin("row", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeBinary(this->row);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("columns", ::apache::thrift::protocol::T_MAP, 2);
  {
    xfer += oprot->writeMapBegin(::apache::thrift::protocol::T_STRING, ::apache::thrift::protocol::T_STRUCT, static_cast<uint32_t>(this->columns.size()));
    std::map<Text, TCell> ::const_iterator _iter13;
    for (_iter13 = this->columns.begin(); _iter13 != this->columns.end(); ++_iter13)
    {
      xfer += oprot->writeBinary(_iter13->first);
      xfer += _iter13->second.write(oprot);
    }
    xfer += oprot->writeMapEnd();
  }
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* TScan::ascii_fingerprint = "2C111FDF8CD162886ECCCBB9C9051083";
const uint8_t TScan::binary_fingerprint[16] = {0x2C,0x11,0x1F,0xDF,0x8C,0xD1,0x62,0x88,0x6E,0xCC,0xCB,0xB9,0xC9,0x05,0x10,0x83};

uint32_t TScan::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->startRow);
          this->__isset.startRow = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->stopRow);
          this->__isset.stopRow = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->timestamp);
          this->__isset.timestamp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->columns.clear();
            uint32_t _size14;
            ::apache::thrift::protocol::TType _etype17;
            iprot->readListBegin(_etype17, _size14);
            this->columns.resize(_size14);
            uint32_t _i18;
            for (_i18 = 0; _i18 < _size14; ++_i18)
            {
              xfer += iprot->readBinary(this->columns[_i18]);
            }
            iprot->readListEnd();
          }
          this->__isset.columns = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->caching);
          this->__isset.caching = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->filterString);
          this->__isset.filterString = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TScan::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("TScan");
  if (this->__isset.startRow) {
    xfer += oprot->writeFieldBegin("startRow", ::apache::thrift::protocol::T_STRING, 1);
    xfer += oprot->writeBinary(this->startRow);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.stopRow) {
    xfer += oprot->writeFieldBegin("stopRow", ::apache::thrift::protocol::T_STRING, 2);
    xfer += oprot->writeBinary(this->stopRow);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.timestamp) {
    xfer += oprot->writeFieldBegin("timestamp", ::apache::thrift::protocol::T_I64, 3);
    xfer += oprot->writeI64(this->timestamp);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.columns) {
    xfer += oprot->writeFieldBegin("columns", ::apache::thrift::protocol::T_LIST, 4);
    {
      xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, static_cast<uint32_t>(this->columns.size()));
      std::vector<Text> ::const_iterator _iter19;
      for (_iter19 = this->columns.begin(); _iter19 != this->columns.end(); ++_iter19)
      {
        xfer += oprot->writeBinary((*_iter19));
      }
      xfer += oprot->writeListEnd();
    }
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.caching) {
    xfer += oprot->writeFieldBegin("caching", ::apache::thrift::protocol::T_I32, 5);
    xfer += oprot->writeI32(this->caching);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.filterString) {
    xfer += oprot->writeFieldBegin("filterString", ::apache::thrift::protocol::T_STRING, 6);
    xfer += oprot->writeBinary(this->filterString);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* TaoAssocGetResult::ascii_fingerprint = "62C2D09CFDA6764EAE246139DD2F0023";
const uint8_t TaoAssocGetResult::binary_fingerprint[16] = {0x62,0xC2,0xD0,0x9C,0xFD,0xA6,0x76,0x4E,0xAE,0x24,0x61,0x39,0xDD,0x2F,0x00,0x23};

uint32_t TaoAssocGetResult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->id2);
          this->__isset.id2 = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->id1Type);
          this->__isset.id1Type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->id2Type);
          this->__isset.id2Type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->time);
          this->__isset.time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->dataVersion);
          this->__isset.dataVersion = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->data);
          this->__isset.data = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TaoAssocGetResult::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("TaoAssocGetResult");
  xfer += oprot->writeFieldBegin("id2", ::apache::thrift::protocol::T_I64, 1);
  xfer += oprot->writeI64(this->id2);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("id1Type", ::apache::thrift::protocol::T_I64, 2);
  xfer += oprot->writeI64(this->id1Type);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("id2Type", ::apache::thrift::protocol::T_I64, 3);
  xfer += oprot->writeI64(this->id2Type);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("time", ::apache::thrift::protocol::T_I64, 4);
  xfer += oprot->writeI64(this->time);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("dataVersion", ::apache::thrift::protocol::T_I64, 5);
  xfer += oprot->writeI64(this->dataVersion);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("data", ::apache::thrift::protocol::T_STRING, 6);
  xfer += oprot->writeBinary(this->data);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* IOError::ascii_fingerprint = "EFB929595D312AC8F305D5A794CFEDA1";
const uint8_t IOError::binary_fingerprint[16] = {0xEF,0xB9,0x29,0x59,0x5D,0x31,0x2A,0xC8,0xF3,0x05,0xD5,0xA7,0x94,0xCF,0xED,0xA1};

uint32_t IOError::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->message);
          this->__isset.message = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t IOError::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("IOError");
  xfer += oprot->writeFieldBegin("message", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->message);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* IllegalArgument::ascii_fingerprint = "EFB929595D312AC8F305D5A794CFEDA1";
const uint8_t IllegalArgument::binary_fingerprint[16] = {0xEF,0xB9,0x29,0x59,0x5D,0x31,0x2A,0xC8,0xF3,0x05,0xD5,0xA7,0x94,0xCF,0xED,0xA1};

uint32_t IllegalArgument::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->message);
          this->__isset.message = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t IllegalArgument::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("IllegalArgument");
  xfer += oprot->writeFieldBegin("message", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->message);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* AlreadyExists::ascii_fingerprint = "EFB929595D312AC8F305D5A794CFEDA1";
const uint8_t AlreadyExists::binary_fingerprint[16] = {0xEF,0xB9,0x29,0x59,0x5D,0x31,0x2A,0xC8,0xF3,0x05,0xD5,0xA7,0x94,0xCF,0xED,0xA1};

uint32_t AlreadyExists::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->message);
          this->__isset.message = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t AlreadyExists::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("AlreadyExists");
  xfer += oprot->writeFieldBegin("message", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->message);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

}}}} // namespace
