//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//CRS STM32 INJECTOR DRIVER
//Version: V1.0
//Date:2019-1
//www.hongwavetech
//数据操作文件
//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
#include "database.h"
#include "capture.h"
#include "version.h"

//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//变量定义

const unsigned char DataStr[] = __DATE__;
const unsigned char TimeStr[] = __TIME__;

WorkUnionDef WorkUnion;
InfoUnionDef InfoUnion;

//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//初始化工作参数
//输入：无
//输出：无
//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
void InitWorkPara(void)
{

}


//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//初始化工作状态
//输入：无
//输出：无
//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
void InitWorkInfo(void)
{
	InfoUnion.input.Pump.Version.FWVersion = VERSION;
	//InfoUnion.input.Version.BootVersion = 0;
	InfoUnion.input.Pump.Version.ProductID = 0;
	InfoUnion.input.Pump.Version.CPUID[0] = *(vu32 *)(0x1ffff7e8);
	InfoUnion.input.Pump.Version.CPUID[1] = *(vu32 *)(0x1ffff7ec);
	InfoUnion.input.Pump.Version.CPUID[2] = *(vu32 *)(0x1ffff7f0);
	InfoUnion.input.Pump.Version.ReleaseDate = 0;

	InfoUnion.input.Version.BuildYear = (u32)FW_DATE >> 16;
	InfoUnion.input.Version.BuildDate = (u32)FW_DATE & 0xffff;
}


//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
//初始化数据
//输入：无
//输出：无
//=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=
void InitData(void)
{
	InitWorkPara();
	InitWorkInfo();

}
