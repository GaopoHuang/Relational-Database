//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {


  // USE: ctor ---------------------------------------
  Storage::Storage(std::iostream &aStream) : BlockIO(aStream) {
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
  }

  bool Storage::each(const BlockVisitor aVisitor) {
    //implement this as a visitor pattern for storage blocks
    uint32_t theCount = getBlockCount();
    for (uint32_t i=0; i<theCount; i++) {
      Block theBlock;
      readBlock(i,theBlock);
      if(!aVisitor(theBlock,i)) {
        break;
      }
    }
    return true;
  }
  
  //write logic to get the next available free block
  uint32_t Storage::getFreeBlock() {
    uint32_t theFreeNum = kNewBlock;
    each([&](const Block& aBlock, uint32_t blockNum) {
      if(aBlock.isFree()) {
        theFreeNum = blockNum;
        return false;
      }
      return true;
    });
    return (theFreeNum==kNewBlock) ? getBlockCount() : theFreeNum;
  }

  //obtain a series of free blocks
  std::vector<uint32_t> Storage::getFreeBlocks(uint32_t aCount) {
    uint32_t theCount = 0;
    std::vector<uint32_t> theFreeBlocks;
    each([&](const Block& aBlock, uint32_t blockNum) {
      if(aBlock.isFree()) {
        theFreeBlocks.push_back(blockNum);
        theCount++;
      if(theCount==aCount) {return false;}
      }
      return true;
    });
    uint32_t theNewBlockNum = getBlockCount();
    while(theCount<aCount) {
      theFreeBlocks.push_back(theNewBlockNum);
      theNewBlockNum++;
      theCount++;
    }
    return theFreeBlocks;
  }

  //write logic to mark a block as free...
  StatusResult Storage::markBlockAsFree(uint32_t aPos) {
    Block theBlock;
    readBlock(aPos,theBlock);
    theBlock.header.type = 'F';
    writeBlock(aPos,theBlock);
    return StatusResult{Errors::noError};
  }

  // USE: for use with storable API...
  //   Write logic to mark a sequence of blocks as free)
  //   starting at given block number, following block sequence
  //   defined in the block headers...
  StatusResult Storage::releaseBlocks(uint32_t aPos,bool aInclusive) {
    Block theBlock;
    std::vector<uint32_t> thePos;
    uint32_t theNext = aPos;
    if(aInclusive) thePos.push_back(aPos);
    while(theNext != kNewBlock) {
      readBlock(theNext,theBlock);
      theNext = theBlock.header.next;
      if(theNext != kNewBlock) {
        readBlock(theNext,theBlock);
        thePos.push_back(theNext);
      }
    }
    for(auto pos: thePos) {
      markBlockAsFree(pos);
    }

    return StatusResult{Errors::noError};
  }

  //Write logic to break stream into N parts, that fit into
  //a series of blocks. Save each block, and link them together
  //logically using data in the header...
  StatusResult Storage::save(std::iostream &aStream,
                             StorageInfo &anInfo) {
    uint32_t blkCount = (anInfo.size-1) / kPayloadSize + 1; //Ceiling division for # of blocks needed
    uint32_t currPos, nextPos = (anInfo.start==kNewBlock ? getFreeBlock() : anInfo.start);
    for (uint32_t i=0; i<blkCount; i++) {
      currPos = nextPos;
      Block theBlock(anInfo.type);
      aStream.read(theBlock.payload,kPayloadSize);

      //In the last block, set next to -1 and pad payload with 0
      if(i == blkCount-1) {
        nextPos = kNewBlock;
        if(anInfo.size%kPayloadSize!=0) {
          for(size_t pad0pos = anInfo.size%kPayloadSize; pad0pos<kPayloadSize; pad0pos++) {
            theBlock.payload[pad0pos] = 0;
          }
        }
      }
      else {nextPos = getFreeBlock();}
      
      //header information
      theBlock.header.count = blkCount;
      theBlock.header.refId = anInfo.refId;
      theBlock.header.pos = i;
      theBlock.header.next = nextPos;

      writeBlock(currPos,theBlock);
    }                         
    return StatusResult{Errors::noError};
  }

  //Write logic to read an ordered sequence of N blocks, back into
  //a stream for your caller
  StatusResult Storage::load(std::iostream &anOut, uint32_t aBlockNum, bool withHeader) {
    uint32_t nextPos = aBlockNum;
    while(kNewBlock != nextPos) {
      Block theBlock;
      readBlock(nextPos,theBlock);
      theBlock.write(anOut, withHeader);
      nextPos = theBlock.header.next;

      // //debugging
      // std::cout << "In storage load, the next position is " << nextPos << "\n";
    }
    return StatusResult{Errors::noError};
  }
 
}

