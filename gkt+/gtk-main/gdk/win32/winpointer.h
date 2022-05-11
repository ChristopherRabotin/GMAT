/* GDK - The GIMP Drawing Kit
 * Copyright (C) 2021 the GTK team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/* This code is derived from portions provided by the mingw-w64 project
 * (mingw-w64.org), originally licensed under the Zope Public License
 * (ZPL) version 2.1, with modifications made on May 12, 2021.
 * Legal notice of the Zope Public License version 2.1 follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions in source code must retain the accompanying copyright
 *       notice, this list of conditions, and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the accompanying
 *       copyright notice, this list of conditions, and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution.
 *    3. Names of the copyright holders must not be used to endorse or promote
 *       products derived from this software without prior written permission
 *       from the copyright holders.
 *    4. The right to distribute this software or to use it for any purpose does
 *       not give you the right to use Servicemarks (sm) or Trademarks (tm) of
 *       the copyright holders.  Use of them is covered by separate agreement
 *       with the copyright holders.
 *    5. If any files are modified, you must cause the modified files to carry
 *       prominent notices stating that you changed the files and the date of
 *       any change.
 */

#ifndef POINTER_FLAG_NONE

#include <windows.h>
#include <tchar.h>

#define WM_POINTERDEVICECHANGE 0x238
#define WM_POINTERDEVICEINRANGE 0x239
#define WM_POINTERDEVICEOUTOFRANGE 0x23a

#define WM_NCPOINTERUPDATE 0x0241
#define WM_NCPOINTERDOWN 0x0242
#define WM_NCPOINTERUP 0x0243
#define WM_POINTERUPDATE 0x0245
#define WM_POINTERDOWN 0x0246
#define WM_POINTERUP 0x0247
#define WM_POINTERENTER 0x0249
#define WM_POINTERLEAVE 0x024a
#define WM_POINTERACTIVATE 0x024b
#define WM_POINTERCAPTURECHANGED 0x024c
#define WM_TOUCHHITTESTING 0x024d
#define WM_POINTERWHEEL 0x024e
#define WM_POINTERHWHEEL 0x024f
#define DM_POINTERHITTEST 0x0250
#define WM_POINTERROUTEDTO 0x0251
#define WM_POINTERROUTEDAWAY 0x0252
#define WM_POINTERROUTEDRELEASED 0x0253

#define POINTER_FLAG_NONE 0x00000000
#define POINTER_FLAG_NEW 0x00000001
#define POINTER_FLAG_INRANGE 0x00000002
#define POINTER_FLAG_INCONTACT 0x00000004
#define POINTER_FLAG_FIRSTBUTTON 0x00000010
#define POINTER_FLAG_SECONDBUTTON 0x00000020
#define POINTER_FLAG_THIRDBUTTON 0x00000040
#define POINTER_FLAG_FOURTHBUTTON 0x00000080
#define POINTER_FLAG_FIFTHBUTTON 0x00000100
#define POINTER_FLAG_PRIMARY 0x00002000
#define POINTER_FLAG_CONFIDENCE 0x00004000
#define POINTER_FLAG_CANCELED 0x00008000
#define POINTER_FLAG_DOWN 0x00010000
#define POINTER_FLAG_UPDATE 0x00020000
#define POINTER_FLAG_UP 0x00040000
#define POINTER_FLAG_WHEEL 0x00080000
#define POINTER_FLAG_HWHEEL 0x00100000
#define POINTER_FLAG_CAPTURECHANGED 0x00200000
#define POINTER_FLAG_HASTRANSFORM 0x00400000

#define POINTER_MOD_SHIFT (0x0004)
#define POINTER_MOD_CTRL (0x0008)

#define TOUCH_FLAG_NONE 0x00000000

#define TOUCH_MASK_NONE 0x00000000
#define TOUCH_MASK_CONTACTAREA 0x00000001
#define TOUCH_MASK_ORIENTATION 0x00000002
#define TOUCH_MASK_PRESSURE 0x00000004

#define PEN_FLAG_NONE 0x00000000
#define PEN_FLAG_BARREL 0x00000001
#define PEN_FLAG_INVERTED 0x00000002
#define PEN_FLAG_ERASER 0x00000004

#define PEN_MASK_NONE 0x00000000
#define PEN_MASK_PRESSURE 0x00000001
#define PEN_MASK_ROTATION 0x00000002
#define PEN_MASK_TILT_X 0x00000004
#define PEN_MASK_TILT_Y 0x00000008

#define POINTER_MESSAGE_FLAG_NEW 0x00000001
#define POINTER_MESSAGE_FLAG_INRANGE 0x00000002
#define POINTER_MESSAGE_FLAG_INCONTACT 0x00000004
#define POINTER_MESSAGE_FLAG_FIRSTBUTTON 0x00000010
#define POINTER_MESSAGE_FLAG_SECONDBUTTON 0x00000020
#define POINTER_MESSAGE_FLAG_THIRDBUTTON 0x00000040
#define POINTER_MESSAGE_FLAG_FOURTHBUTTON 0x00000080
#define POINTER_MESSAGE_FLAG_FIFTHBUTTON 0x00000100
#define POINTER_MESSAGE_FLAG_PRIMARY 0x00002000
#define POINTER_MESSAGE_FLAG_CONFIDENCE 0x00004000
#define POINTER_MESSAGE_FLAG_CANCELED 0x00008000

#define GET_POINTERID_WPARAM(wParam) (LOWORD (wParam))
#define IS_POINTER_FLAG_SET_WPARAM(wParam, flag) (((DWORD)HIWORD (wParam) &(flag)) == (flag))
#define IS_POINTER_NEW_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_NEW)
#define IS_POINTER_INRANGE_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_INRANGE)
#define IS_POINTER_INCONTACT_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_INCONTACT)
#define IS_POINTER_FIRSTBUTTON_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_FIRSTBUTTON)
#define IS_POINTER_SECONDBUTTON_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_SECONDBUTTON)
#define IS_POINTER_THIRDBUTTON_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_THIRDBUTTON)
#define IS_POINTER_FOURTHBUTTON_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_FOURTHBUTTON)
#define IS_POINTER_FIFTHBUTTON_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_FIFTHBUTTON)
#define IS_POINTER_PRIMARY_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_PRIMARY)
#define HAS_POINTER_CONFIDENCE_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_CONFIDENCE)
#define IS_POINTER_CANCELED_WPARAM(wParam) IS_POINTER_FLAG_SET_WPARAM (wParam, POINTER_MESSAGE_FLAG_CANCELED)

#define PA_ACTIVATE MA_ACTIVATE
#define PA_NOACTIVATE MA_NOACTIVATE

typedef DWORD POINTER_INPUT_TYPE;
typedef UINT32 POINTER_FLAGS;
typedef UINT32 TOUCH_FLAGS;
typedef UINT32 TOUCH_MASK;
typedef UINT32 PEN_FLAGS;
typedef UINT32 PEN_MASK;

enum tagPOINTER_INPUT_TYPE {
  PT_POINTER = 0x00000001,
  PT_TOUCH = 0x00000002,
  PT_PEN = 0x00000003,
  PT_MOUSE = 0x00000004,
  PT_TOUCHPAD = 0x00000005
};

typedef enum tagFEEDBACK_TYPE {
  FEEDBACK_TOUCH_CONTACTVISUALIZATION = 1,
  FEEDBACK_PEN_BARRELVISUALIZATION = 2,
  FEEDBACK_PEN_TAP = 3,
  FEEDBACK_PEN_DOUBLETAP = 4,
  FEEDBACK_PEN_PRESSANDHOLD = 5,
  FEEDBACK_PEN_RIGHTTAP = 6,
  FEEDBACK_TOUCH_TAP = 7,
  FEEDBACK_TOUCH_DOUBLETAP = 8,
  FEEDBACK_TOUCH_PRESSANDHOLD = 9,
  FEEDBACK_TOUCH_RIGHTTAP = 10,
  FEEDBACK_GESTURE_PRESSANDTAP = 11,
  FEEDBACK_MAX = 0xffffffff
} FEEDBACK_TYPE;

typedef enum tagPOINTER_BUTTON_CHANGE_TYPE {
  POINTER_CHANGE_NONE,
  POINTER_CHANGE_FIRSTBUTTON_DOWN,
  POINTER_CHANGE_FIRSTBUTTON_UP,
  POINTER_CHANGE_SECONDBUTTON_DOWN,
  POINTER_CHANGE_SECONDBUTTON_UP,
  POINTER_CHANGE_THIRDBUTTON_DOWN,
  POINTER_CHANGE_THIRDBUTTON_UP,
  POINTER_CHANGE_FOURTHBUTTON_DOWN,
  POINTER_CHANGE_FOURTHBUTTON_UP,
  POINTER_CHANGE_FIFTHBUTTON_DOWN,
  POINTER_CHANGE_FIFTHBUTTON_UP,
} POINTER_BUTTON_CHANGE_TYPE;

typedef struct tagPOINTER_INFO {
  POINTER_INPUT_TYPE pointerType;
  UINT32 pointerId;
  UINT32 frameId;
  POINTER_FLAGS pointerFlags;
  HANDLE sourceDevice;
  HWND hwndTarget;
  POINT ptPixelLocation;
  POINT ptHimetricLocation;
  POINT ptPixelLocationRaw;
  POINT ptHimetricLocationRaw;
  DWORD dwTime;
  UINT32 historyCount;
  INT32 InputData;
  DWORD dwKeyStates;
  UINT64 PerformanceCount;
  POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef struct tagPOINTER_TOUCH_INFO {
  POINTER_INFO pointerInfo;
  TOUCH_FLAGS touchFlags;
  TOUCH_MASK touchMask;
  RECT rcContact;
  RECT rcContactRaw;
  UINT32 orientation;
  UINT32 pressure;
} POINTER_TOUCH_INFO;

typedef struct tagPOINTER_PEN_INFO {
  POINTER_INFO pointerInfo;
  PEN_FLAGS penFlags;
  PEN_MASK penMask;
  UINT32 pressure;
  UINT32 rotation;
  INT32 tiltX;
  INT32 tiltY;
} POINTER_PEN_INFO;

typedef enum {
  POINTER_FEEDBACK_DEFAULT = 1,
  POINTER_FEEDBACK_INDIRECT = 2,
  POINTER_FEEDBACK_NONE = 3
} POINTER_FEEDBACK_MODE;

typedef struct tagUSAGE_PROPERTIES {
  USHORT level;
  USHORT page;
  USHORT usage;
  INT32 logicalMinimum;
  INT32 logicalMaximum;
  USHORT unit;
  USHORT exponent;
  BYTE count;
  INT32 physicalMinimum;
  INT32 physicalMaximum;
} USAGE_PROPERTIES, *PUSAGE_PROPERTIES;

typedef struct tagPOINTER_TYPE_INFO {
  POINTER_INPUT_TYPE  type;
  union {
      POINTER_TOUCH_INFO touchInfo;
      POINTER_PEN_INFO penInfo;
  } DUMMYUNIONNAME;
} POINTER_TYPE_INFO, *PPOINTER_TYPE_INFO;

#define POINTER_DEVICE_PRODUCT_STRING_MAX 520
#define PDC_ARRIVAL 0x001
#define PDC_REMOVAL 0x002
#define PDC_ORIENTATION_0 0x004
#define PDC_ORIENTATION_90 0x008
#define PDC_ORIENTATION_180 0x010
#define PDC_ORIENTATION_270 0x020
#define PDC_MODE_DEFAULT 0x040
#define PDC_MODE_CENTERED 0x080
#define PDC_MAPPING_CHANGE 0x100
#define PDC_RESOLUTION 0x200
#define PDC_ORIGIN 0x400
#define PDC_MODE_ASPECTRATIOPRESERVED 0x800

typedef enum tagPOINTER_DEVICE_TYPE {
  POINTER_DEVICE_TYPE_INTEGRATED_PEN = 0x00000001,
  POINTER_DEVICE_TYPE_EXTERNAL_PEN = 0x00000002,
  POINTER_DEVICE_TYPE_TOUCH = 0x00000003,
  POINTER_DEVICE_TYPE_TOUCH_PAD = 0x00000004,
  POINTER_DEVICE_TYPE_MAX = 0xffffffff
} POINTER_DEVICE_TYPE;

typedef struct tagPOINTER_DEVICE_INFO {
  DWORD displayOrientation;
  HANDLE device;
  POINTER_DEVICE_TYPE pointerDeviceType;
  HMONITOR monitor;
  ULONG startingCursorId;
  USHORT maxActiveContacts;
  WCHAR productString[POINTER_DEVICE_PRODUCT_STRING_MAX];
} POINTER_DEVICE_INFO;

typedef struct tagPOINTER_DEVICE_PROPERTY {
  INT32 logicalMin;
  INT32 logicalMax;
  INT32 physicalMin;
  INT32 physicalMax;
  UINT32 unit;
  UINT32 unitExponent;
  USHORT usagePageId;
  USHORT usageId;
} POINTER_DEVICE_PROPERTY;

typedef enum tagPOINTER_DEVICE_CURSOR_TYPE {
  POINTER_DEVICE_CURSOR_TYPE_UNKNOWN = 0x00000000,
  POINTER_DEVICE_CURSOR_TYPE_TIP = 0x00000001,
  POINTER_DEVICE_CURSOR_TYPE_ERASER = 0x00000002,
  POINTER_DEVICE_CURSOR_TYPE_MAX = 0xffffffff
} POINTER_DEVICE_CURSOR_TYPE;

typedef struct tagPOINTER_DEVICE_CURSOR_INFO {
  UINT32 cursorId;
  POINTER_DEVICE_CURSOR_TYPE cursor;
} POINTER_DEVICE_CURSOR_INFO;

#endif /* POINTER_FLAG_NONE */

#if WINVER < 0x0601

typedef struct tagGESTURECONFIG {
  DWORD dwID;
  DWORD dwWant;
  DWORD dwBlock;
} GESTURECONFIG,*PGESTURECONFIG;

#endif /* WINVER < 0x0601 */

#ifndef MICROSOFT_TABLETPENSERVICE_PROPERTY
#define MICROSOFT_TABLETPENSERVICE_PROPERTY _T("MicrosoftTabletPenServiceProperty")
#endif
