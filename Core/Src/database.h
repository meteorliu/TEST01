#ifndef _DATABASE_H_
#define _DATABASE_H_

#define INPUT_BUF_SIZE sizeof(InfoUnionDef)
#define HOLD_BUF_SIZE sizeof(WorkUnionDef)

#define ApplicationAddress     			  0x8005000
#define PAGE_SIZE                         (0x800)    /* 1 Kbytes */

#define FLASH_STORE_VERSION 1

#ifdef FLASH_STORE_VERSION
#define PCBVERSION_ADDRESS (ApplicationAddress - PAGE_SIZE)  //0x8004C00
#define BOOTLOADER_ADDRESS (ApplicationAddress - PAGE_SIZE + 4) //BOOTLOADER版本号存放地址
#else
#define PCBVERSION_ADDRESS 0x03f8 //PCB版本号存放地址
#define BOOTLOADER_ADDRESS 0x03f7 //BOOTLOADER版本号存放地址
#endif

#define  u8  uint8_t
#define  u16 uint16_t
#define  u32 uint32_t

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//常量定义
#define INIT_FLAG  		0x8d           //初始化标志
#define SERIAL_ADDR  	0x1f0        //序列号存放地址

#define true 1
#define false 0

//#define USE_BOOTLOADER  1
#define SIDE_FALLING 1
#define SIDE_RISING  0


//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//升级设置
typedef struct
{
	u16 Shield;          //屏蔽通讯,1的时候不回复任何信息
	u16 Download;        //1 开始更新程序进入BOOTloader
} UpdateDef;

//版本信息定义
typedef struct
{
	u16  FWVersion;
	u16  BootVersion;
	u16  HWVersion;
	u32  CPUID[3];
	u32  ProductID;
	u16  ReleaseDate;
	u16  CustomerID;
	u16  Serial;
	u16  BuildYear;
	u16  BuildDate;
} VersionDef;


typedef struct
{

}holddef;

typedef struct
{
	UpdateDef Update;
	VersionDef Version;
}inputdef;

//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//读写数据 联合体
typedef union
{
	holddef Hold;
	u16  HoldBuf[sizeof(holddef)/2 + 1];
} WorkUnionDef;

//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//只读数据 联合体
typedef union
{
	inputdef input;
	u16  Buf[sizeof(InfoStatusdef)/2 + 1];
} InfoUnionDef;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//变量声明
extern InfoUnionDef InfoUnion; //只读
extern WorkUnionDef WorkUnion; //保持
extern void InitData(void);
#endif
