//
// Created by LGW on 2017/5/11.
//
#pragma once
//避免同一个头文件被include多次
//但是这种方法只是针对文件，而不是指内容相同的两个文件，并且不能跨平台，有的编译器并不支持，到时要改成#ifndef
#include <math.h>
#include <string>
#define M_PI 3.14159265358979323846
#define DEFFAULT_LONTITUDE -9999
using namespace std;
//在所有的标准库函数命名空间std中进行定义，避免发生重命名的问题

namespace coord {
	/**
	* 坐标点 结构体
	*/
	struct Point  //声明结构体类型
	{
		double x;
		double y;
		double z;
		Point() {}
		Point(double _x, double _y, double _z) :x(_x), y(_y), z(_z){}
	};

	/**
	* 椭球体参数
	*/
	struct EllipsoidParam
	{
		string name;
		double a;
		double b;
		double f;
	};

	/**
	* 大地坐标系枚举类
	*/
	enum GEODETIC_COORD_SYSTEM
	{
		XIAN80,
		WGS84,
		CGCS2000
	};

	/**
	* 各个椭球体参数的数组 （大地坐标系名称，a,b,f）
	*/
	static EllipsoidParam EllipsoidParamArray[] =
	{
		{ "XIAN80", 6378140, 6356755.2882, 1 / 298.257 },
		{ "WGS84", 6378137, 6356752.314, 1 / 298.2572236 },
		{ "CGCS2000",6378137, 6356752.3141403558, 1/ 298.257222101 }
	};
}

using namespace coord;
class Trans
{
private:
	double
		dx,//x坐标平移量(米)
		dy,//y坐标平移量(米)
		dz,//z坐标平移量(米)
		Ox,//x坐标旋转角度(秒)
		Oy,//y坐标旋转角度(秒)
		Oz,//z坐标旋转角度(秒)
		k; //尺度因子(ppm)，这里其实应该用变量m表示
	//这里要说明一下，尺度因子有两种单位的表示形式，一种结果约为1，如1.0000045，用k表示；
	//另一种就是ppm的表示形式，稍微比1大一点，如4.5，用m表示。k=1+m/1000000
	
	GEODETIC_COORD_SYSTEM coordSystem_origin;//原始坐标系
	GEODETIC_COORD_SYSTEM coordSystem_result;//结果坐标系
	
private:
	/**
	 * 将角度转换为弧度
	 */
	double toRadians(double angdeg);

	/**
	 * 将弧度转化为角度
	 */
	double toDegrees(double angdeg);


public:
	/**
	 * 七参数 + 原始坐标系 + 结果坐标系
	 */
	Trans(double dx, double dy, double dz, double Ox, double Oy, double Oz, double k,
		GEODETIC_COORD_SYSTEM _coordSystem_origin, GEODETIC_COORD_SYSTEM _coordSystem_result);

	~Trans();

	GEODETIC_COORD_SYSTEM getOriginCoordSystem()
	{
		return coordSystem_origin;
	}

	GEODETIC_COORD_SYSTEM getResultCoordSystem()
	{
		return coordSystem_result;
	}

	/**
	 * 大地坐标转空间坐标
	 * 输入：pt:大地坐标(度°)  a、b:该大地坐标系椭球体的长、短半轴(m)
	 * 输出：空间坐标(m)
	 */
	Point geodetic2spatial(Point pt,double a ,double b);
	
	/**
	 * 七参数计算（spatial1 -> spatial2）
	 * 输入：point:空间坐标(m) 
	 * 输出：空间坐标(m)
	 */
	Point qicanshu(Point point);

	/**
	* 空间坐标转大地坐标
	* 输入：pt:空间坐标(m)  a、f:结果大地坐标系椭球体的长半轴(m)和扁率
	* 输出：大地坐标(度°)
	*/
	Point spatial2geodetic(Point pt, double a, double f);

	/**
	 * 高斯投影 大地坐标->平面坐标
	 * Lo; //中央经线(度) 高斯投影时使用 提供的话按照输入的中央经线计算，没提供则通过带宽计算得出
	 * a、b、f:大地坐标系椭球体的长、短半轴(m)和扁率
	 * ZoneWide:带宽 3 or 6
	 */
	Point gaussPrj_geodetic2plane(Point point, double a, double b, double f, int ZoneWide, int Lo = DEFFAULT_LONTITUDE);

	/**
	 * 高斯反算 平面坐标->大地坐标
	 * Lo; //中央经线(度) 高斯投影时使用 提供的话按照输入的中央经线计算，没提供则通过带宽计算得出 
	 * a、b:结果大地坐标系椭球体的长、短半轴(m)
	 * ZoneWide:带宽 3 or 6
	 */
	Point gaussInversePrj_plane2geodetic(Point point, double a, double b,int ZoneWide, int Lo = DEFFAULT_LONTITUDE);

	/**
	 * 大地坐标系1下的经纬度坐标 -> 大地坐标系2下的高斯投影平面坐标
	 * 输入：point（大地坐标系1下的经纬度坐标）,*_origin分别为大地坐标系1对应椭球体的长、短半轴（m）及扁率
	 * 输入：*_result分别为大地坐标系2对应的椭球体的长、短半轴（m）及扁率，ZoneWide:带宽 3 or 6，Lo; //中央经线(度) 高斯投影时使用，可选项
	 * 输出：大地坐标系2下的高斯投影平面坐标
	 */
	Point geodetic2plane(Point point, double a_origin, double b_origin, double a_result, double b_result, double f_result, int ZoneWide, int Lo = DEFFAULT_LONTITUDE);

	/**
	* 大地坐标系1下的高斯投影平面坐标 -> 大地坐标系2下的经纬度坐标
	* 输入：point（大地坐标系1下的高斯投影平面坐标）,*_origin分别为大地坐标系1对应椭球体的长、短半轴（m）及扁率
	* 输入：*_result分别为大地坐标系2对应的椭球体的长、短半轴（m）及扁率，ZoneWide:带宽 3 or 6，Lo; //中央经线(度) 高斯投影时使用，可选项
	* 输出：大地坐标系2下的经纬度坐标
	*/
	Point plane2geodetic(Point point, double a_origin, double b_origin, double f_origin, double a_result, double f_result, int ZoneWide, int Lo = DEFFAULT_LONTITUDE);

	/**
	 * 大地坐标系1下的经纬度坐标 -> 大地坐标系2下的高斯投影平面坐标
	 * 输入：ZoneWide:带宽 3 or 6；Lo; //中央经线(度) 高斯投影时使用，可选项
	 * 椭球体参数使用构造函数枚举类型对应的参数
	 */
	Point geodetic2plane(Point point, int ZoneWide, int Lo = DEFFAULT_LONTITUDE);

	/**
	 * 大地坐标系1下的高斯投影平面坐标 -> 大地坐标系2下的经纬度坐标
	 * 输入：ZoneWide:带宽 3 or 6；Lo; //中央经线(度) 高斯投影时使用，可选项
	 * 椭球体参数使用构造函数枚举类型对应的参数
	 */
	Point plane2geodetic(Point point, int ZoneWide, int Lo = DEFFAULT_LONTITUDE);
};

