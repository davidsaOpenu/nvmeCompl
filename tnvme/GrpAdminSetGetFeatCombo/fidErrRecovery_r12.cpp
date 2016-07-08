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

#include "fidErrRecovery_r12.h"

#include <boost/format.hpp>
#include "globals.h"
#include "grpDefs.h"
#include "../Queues/acq.h"
#include "../Queues/asq.h"
#include "../Utils/kernelAPI.h"
#include "../Utils/irq.h"
#include "../Utils/io.h"
#include "../Utils/queues.h"
#include "../Cmds/identifyDefs.h"
#include "../Cmds/featureDefs.h"

namespace GrpAdminSetGetFeatCombo {


FIDErrRecovery_r12::FIDErrRecovery_r12(
    string grpName, string testName) :
    Test(grpName, testName, SPECREV_12)
{
    // 63 chars allowed:     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    mTestDesc.SetCompliance("revision 1.2, section 5");
    mTestDesc.SetShort(     "Verify changes are allowed on FID=Error recovery 1.2");
    // No string size limit for the long description
    mTestDesc.SetLong(
        "Reset ctrlr to cause a clearing of DUT state. Issue GetFeatures, "
        "FID = 0x05. Programmatically and dynamically force new, legal values "
        "into all fields of CE.DW0 and assoc changes with SetFeatures. Redo "
        "GetFeatures to verify settings were accepted. Version 1.2 now checks "
        "for memory deallocation support and will set the DULBE bit if "
        "support is found.");
}


FIDErrRecovery_r12::~FIDErrRecovery_r12()
{
    ///////////////////////////////////////////////////////////////////////////
    // Allocations taken from the heap and not under the control of the
    // RsrcMngr need to be freed/deleted here.
    ///////////////////////////////////////////////////////////////////////////
}


FIDErrRecovery_r12::
FIDErrRecovery_r12(const FIDErrRecovery_r12 &other) : Test(other)
{
    ///////////////////////////////////////////////////////////////////////////
    // All pointers in this object must be NULL, never allow shallow or deep
    // copies, see Test::Clone() header comment.
    ///////////////////////////////////////////////////////////////////////////
}


FIDErrRecovery_r12 &
FIDErrRecovery_r12::operator=(const FIDErrRecovery_r12 &other)
{
    ///////////////////////////////////////////////////////////////////////////
    // All pointers in this object must be NULL, never allow shallow or deep
    // copies, see Test::Clone() header comment.
    ///////////////////////////////////////////////////////////////////////////
    Test::operator=(other);
    return *this;
}


Test::RunType
FIDErrRecovery_r12::RunnableCoreTest(bool preserve)
{
    ///////////////////////////////////////////////////////////////////////////
    // All code contained herein must never permanently modify the state or
    // configuration of the DUT. Permanence is defined as state or configuration
    // changes that will not be restored after a cold hard reset.
    ///////////////////////////////////////////////////////////////////////////

    return ((preserve == true) ? RUN_FALSE : RUN_TRUE);   // Test is destructive
}


void
FIDErrRecovery_r12::RunCoreTest()
{
    /** \verbatim
     * Assumptions:
     * None.
     * \endverbatim
     */

    SharedACQPtr acq;
    SharedASQPtr asq;
    Queues::BasicAdminQueueSetup(acq, asq, ACQ_GROUP_ID, ASQ_GROUP_ID);

    LOG_NRM("Create Get features and set features cmds");
    SharedGetFeaturesPtr getFeaturesCmd =
            SharedGetFeaturesPtr(new GetFeatures());
    SharedSetFeaturesPtr setFeaturesCmd =
            SharedSetFeaturesPtr(new SetFeatures());

    LOG_NRM("Set and Get features for error recovery (FID = 0x%x)",
            FID[FID_ERR_RECOVERY]);
    getFeaturesCmd->SetFID(FID[FID_ERR_RECOVERY]);
    setFeaturesCmd->SetFID(FID[FID_ERR_RECOVERY]);

    ConstSharedIdentifyPtr idCtrlr = gInformative->GetIdentifyCmdCtrlr();
    for (uint64_t i = 1; i <= idCtrlr->GetValue(IDCTRLRCAP_NN); i++) {
        LOG_NRM("Processing namspc %ld", i);
        setFeaturesCmd->SetNSID(i);
        getFeaturesCmd->SetNSID(i);

        // checks NS for deallocation support
        ConstSharedIdentifyPtr idNamspc = gInformative->GetIdentifyCmdNamspc(i);
        bool DULBEsupport = (idNamspc->GetValue(IDNAMESPC_NSFEAT) &
            NSFEAT_DEALLOCATED_UNWRITTEN_LBA) != 0;

        bool fieldMismatch = false;
        for (uint32_t tlerPow2 = 1; tlerPow2 <= 0xFFFF; tlerPow2 <<= 1) {
            // tler = {(0, 1, 2), (1, 2, 3), ..., (0xFFFF, 0x10000, 0x10001)}
            for (uint32_t tler = (tlerPow2 - 1); tler <= (tlerPow2 + 1);
                tler++) {
                if (tler > 0xFFFF)
                    break;

                fieldMismatch = SendCommands(acq, asq, setFeaturesCmd,
                    getFeaturesCmd, tler, false);
                if (fieldMismatch)
                    throw FrmwkEx(HERE, "Error recovery fields mismatched.");

                // Run again with DUBLE field set if supported
                if (DULBEsupport) {
                    fieldMismatch = SendCommands(acq, asq, setFeaturesCmd,
                        getFeaturesCmd, tler, true);
                    if (fieldMismatch)
                        throw FrmwkEx(HERE,
                            "Error recovery fields mismatched.");
                }
            }
        }
    }

}

bool
FIDErrRecovery_r12::SendCommands(SharedACQPtr acq, SharedASQPtr asq,
    SharedSetFeaturesPtr setFeaturesCmd, SharedGetFeaturesPtr getFeaturesCmd,
    const uint16_t tler, const bool dulbe) const
{
    string work;
    union CE ce;
    struct nvme_gen_cq acqMetrics;

    LOG_NRM("Set & Get features for time lim err recovery # %d ", tler);
    setFeaturesCmd->SetErrRecoveryTLER(tler);
    LOG_NRM("Set & Get features for dealloc unwritten block # %d ", dulbe);
    setFeaturesCmd->SetErrRecoveryDULBE(dulbe);

    LOG_WARN("SetFeatNSID: %d", setFeaturesCmd->GetNSID());

    LOG_NRM("Issue set features cmd with TLER = %d, DUBLE = %d", tler, dulbe);

    work = str(boost::format("tler.%d.x100ms,dulbe.%d") % tler % dulbe);
    IO::SendAndReapCmd(mGrpName, mTestName, CALC_TIMEOUT_ms(1),
            asq, acq, setFeaturesCmd, work, true);

    acqMetrics = acq->GetQMetrics();

    LOG_NRM("Issue get features cmd & check tler = %d (x100ms), dulbe = %d",
        tler, dulbe);
    IO::SendAndReapCmd(mGrpName, mTestName, CALC_TIMEOUT_ms(1),
            asq, acq, getFeaturesCmd, work, false);

    ce = acq->PeekCE(acqMetrics.head_ptr);
    uint16_t getTLER = ce.t.dw0 & ER_DW11_TLER;
    uint32_t getDULBE = ((ce.t.dw0 & ER_DW11_DULBE) >> 16);
    LOG_NRM("Get Features tler = %d(x100ms), dulbe = %d", getTLER, getDULBE);
    if (tler != getTLER) {
        LOG_ERR("TLER get feat does not match set feat"
            "(expected, rcvd) = (%d, %d)", tler, getTLER);
        return true;
    }
    if (dulbe != getDULBE) {
        LOG_ERR("DULBE get feat does not match set feat"
            "(expected, rcvd) = (%d, %d)", dulbe, getDULBE);
        return true;
    }
    return false;
}


}   // namespace
