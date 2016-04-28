/* $Id$ */
/** @file
 * BS3Kit - Initialize all components, real mode.
 */

/*
 * Copyright (C) 2007-2016 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
//#define BS3_USE_RM_TEXT_SEG 1
#include "bs3kit-template-header.h"
#include "bs3-cmn-test.h"
#include <iprt/asm-amd64-x86.h>

BS3_MODE_PROTO_NOSB(void, Bs3EnteredMode,(void));


BS3_DECL(void) Bs3InitAll_rm(void)
{
    uint32_t volatile BS3_FAR *pcTicks = (uint32_t volatile BS3_FAR *)BS3_FP_MAKE(0x40, 0x6c);
    uint32_t                   cInitialTicks = *pcTicks;
    int                        i = 3;

    Bs3CpuDetect_rm_far();
    Bs3InitMemory_rm_far();
    Bs3InitGdt_rm_far();

    /* For for floppy to stop (a couple of ticks), then disable interrupts. */
    ASMIntEnable();
    while (i-- > 0)
    {
        while (*pcTicks == cInitialTicks)
            ASMHalt();
        *pcTicks = cInitialTicks;
    }
    ASMIntDisable();
    Bs3PicMaskAll();

    if (g_uBs3CpuDetected & BS3CPU_F_LONG_MODE)
        Bs3Trap64Init();
    if ((g_uBs3CpuDetected & BS3CPU_TYPE_MASK) >= BS3CPU_80386)
        Bs3Trap32Init();
    if ((g_uBs3CpuDetected & BS3CPU_TYPE_MASK) >= BS3CPU_80286)
        Bs3Trap16Init();
    Bs3TrapRmV86Init();
    Bs3EnteredMode_rm();
}

