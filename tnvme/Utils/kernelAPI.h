/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _KERNELAPI_H_
#define _KERNELAPI_H_

#include "tnvme.h"
#include "dnvme.h"
#include "fileSystem.h"
#include "../Singletons/regDefs.h"


/**
* This class is meant not be instantiated because it should only ever contain
* static members. These utility functions can be viewed as wrappers to
* perform common, repetitious tasks which avoids coping the same chunks of
* code throughout the framework.
*
* @note This class may throw exceptions, please see comment within specific
*       methods.
*/
class KernelAPI
{
public:
    KernelAPI();
    virtual ~KernelAPI();

    typedef enum {
        MMR_CQ,             // Desire to map SQ memory
        MMR_SQ,             // Desire to map CQ memory
        MMR_META,           // Desire to map meta data memory
        MMAPREGION_FENCE    // always must be last element
     } MmapRegion;

    /**
     * For all mmap operations this method understand the unique encoding
     * which is needed to specify what memory to map into user space.
     * @note This method may throw
     * @param bufLength Pass the # of bytes consisting of the buffers total size
     * @param bufID Pass the buffer's ID corresponding to param region
     * @param region Pass what region of memory is being requested for mapping
     * @return A user space pointer to the mapped kernel memory, NULL indicates
     *      a failed mapping attempt.
     */
    static uint8_t *mmap(size_t bufLength, uint16_t bufID, MmapRegion region);
    static void munmap(uint8_t *memPtr, size_t bufLength);

    /**
     * Dump the entire dnvme metrics structure for the specified device. The
     * filename will be opened and cleared of any contents before writing.
     * @note This method may throw
     * @param filename Pass the filename as generated by macro
     *      FileSystem::PrepDumpFile().
     */
    static void DumpKernelMetrics(DumpFilename filename);

    /**
     * Dump all the ctrl'r space registers.
     * @note This method may throw
     * @param filename Pass the filename as generated by macro
     *      FileSystem::PrepDumpFile().
     * @param verbose Pass true to log action, false to be silent
     */
    static void DumpCtrlrSpaceRegs(DumpFilename filename, bool verbose = true);
    static void DumpPciSpaceRegs(DumpFilename filename, bool verbose = true);

    /// Log the contents of the specified SQ metrics struct
    static void LogCQMetrics(struct nvme_gen_cq &cqMetrics);
    /// Log the contents of the specified SQ metrics struct
    static void LogSQMetrics(struct nvme_gen_sq &sqMetrics);

    /**
     * Write a custom string into the system log via dnvme to mark a point
     * in time possibly for debug.
     * @param log Pass the logging statement to write
     */
    static void WriteToDnvmeLog(string log);


private:
    static void RegToFile(int fd, const PciSpcType regMetrics, uint64_t value);
};


#endif
