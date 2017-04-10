/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
/*ergate-001*/


#ifndef TOUCHPANEL_H
#define TOUCHPANEL_H

/* Pre-defined definition */
#define TPD_TYPE_CAPACITIVE
#define TPD_TYPE_RESISTIVE
#define TPD_POWER_SOURCE         MT6323_POWER_LDO_VGP1
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_DELAY                (2*HZ/100)

#define TPD_HAVE_TREMBLE_ELIMINATION

#define TPD_HAVE_POWER_ON_OFF

#define PRESSURE_FACTOR	10

#define MS_TS_MSG21XX_X_MAX               480
#define MS_TS_MSG21XX_Y_MAX               800

#define TPD_HAVE_BUTTON
//#define TPD_PROXIMITY1

#define TPD_BUTTON_HEIGHT		100
#define TPD_KEY_COUNT           3
#define TPD_KEYS                {KEY_BACK, KEY_HOMEPAGE, KEY_MENU}
#define TPD_KEYS_DIM            {{80,850,160,TPD_BUTTON_HEIGHT},{240,850,160,TPD_BUTTON_HEIGHT},{400,850,160,TPD_BUTTON_HEIGHT}}
#define MAX_TOUCH_FINGER		2
#define TPD_I2C_NUMBER			1

#define	REPORT_PACKET_LENGTH	8
#define	TPD_Y_INVERT				1
//#define	REVERSE_X			1
//#define	SWAP_X_Y				1
//#define TPD_CLOSE_POWER_IN_SLEEP
#define LCT_ADD_TP_VERSION		1	//gpg add 
//#define TPD_PROXIMITY
//#define __FIRMWARE_UPDATE__		//update fw through device node
//#ifdef __FIRMWARE_UPDATE__
//#define ENABLE_AUTO_UPDATA	//update fw during power on
//#endif

#endif
