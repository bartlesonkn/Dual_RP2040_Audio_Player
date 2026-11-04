/*---------------------------------------------------------------------------/
/  Configurations of FatFs Module (Corrected for Media Player)
/---------------------------------------------------------------------------*/

#define FFCONF_DEF  80386   /* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY  0   /* 0: Read/Write */
#define FF_FS_MINIMIZE  0   /* 0: Full basic functions enabled */
#define FF_USE_FIND     0   
#define FF_USE_MKFS     0   
#define FF_USE_FASTSEEK 0   
#define FF_USE_EXPAND   0   
#define FF_USE_CHMOD    0   
#define FF_USE_LABEL    0   
#define FF_USE_FORWARD  0   

#define FF_USE_STRFUNC  0
#define FF_PRINT_LLI    0
#define FF_PRINT_FLOAT  0
#define FF_STRF_ENCODE  0

/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE    437   /* U.S. codepage */

/* CRITICAL FIX: Enabled LFN so file names with spaces/long titles load properly */
#define FF_USE_LFN      1   /* 1: Enable LFN with static working buffer on BSS */
#define FF_MAX_LFN      255 /* Maximum LFN length in UTF-16 */

#define FF_LFN_UNICODE  0
#define FF_LFN_BUF      255
#define FF_SFN_BUF      12
#define FF_FS_RPATH     0

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES      1   /* 1 logical drive (SD card) */
#define FF_STR_VOLUME_ID    0
#define FF_VOLUME_STRS      "RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
#define FF_MULTI_PARTITION  0

#define FF_MIN_SS       512
#define FF_MAX_SS       512 /* Standard sector size for SD cards */

#define FF_LBA64        0
#define FF_MIN_GPT      0x10000000
#define FF_USE_TRIM     0

/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY      0   /* Normal buffer configuration */
#define FF_FS_EXFAT     0   /* FAT/FAT32 support */

/* CRITICAL FIX: Disables external RTC requirement (get_fattime) to prevent link errors */
#define FF_FS_NORTC     1   /* 1: Use fixed fallback date instead of RTC */
#define FF_NORTC_MON    1
#define FF_NORTC_MDAY   1
#define FF_NORTC_YEAR   2026

#define FF_FS_CRTIME    0
#define FF_FS_NOFSINFO  0
#define FF_FS_LOCK      0
#define FF_FS_REENTRANT 0
#define FF_FS_TIMEOUT   1000

/*--- End of configuration options ---*/