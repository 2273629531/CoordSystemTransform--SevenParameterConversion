# CoordSystemTransform使用说明
 
## CoordSystemTransform

## 效果截图
<div align=center><img src="http://www.lgwimonday.cn/cosyblog/wordpress/wp-content/uploads/2018/07/2018070903435979.jpg" alt="CoordSystemTransform效果截图"/></div>

## 使用说明

1. 引入库文件

   CoordSystemTransform工具由C++语言编写，只有两个文件，**Trans.h**和**Trans.cpp**文件，只需将这两个文件引入项目中便可直接调用函数接口。

2. 接口调用

   (1) 建立Trans对象

   ```c++
   Trans trans2(8.5, -28, -14.8888, -0.888, 1.9999, -3.1111, 4.22222, coord::XIAN80, coord::WGS84);
   ```

   (2) 坐标系转换

   ```c++
   //方法1
   Point result1 = trans2.plane2geodetic(Point(19348965.21, 2894958.496, 0), 6378140, 6356755.2882, 1 / 298.257, 6378137, 1 / 298.2572236, 6);
   //方法2
   Point result1_copy = trans2.plane2geodetic(Point(19348965.21, 2894958.496, 0), 6);
   //plane2geodetic重载了两种不同方法，具体说明请下载源码见注释
   //result1和result1_copy结果相同，都是最终的转换结果，任选其一即可
   ```

## wgs84坐标系与西安80互相转换

1. wgs84转西安80

   ```c++
   Trans trans(-8.5, 28, 14.8888, 0.888, -1.9999, 3.1111, -4.22222,coord::WGS84,coord::XIAN80);
   Point result_copy = trans.geodetic2plane(Point(108.635229183, 26.111425709, 0), 6);
   ```

2. 西安80转wgs84

   ```c++
   Trans trans2(8.5, -28, -14.8888, -0.888, 1.9999, -3.1111, 4.22222, coord::XIAN80, coord::WGS84);
   Point result1_copy = trans2.plane2geodetic(Point(19348965.21, 2894958.496, 0), 6);
   ```

## GIS坐标系扩展方法

目前，CoordSystemTransform只引入了XIAN80和WGS84两种类型的大地坐标系，如果你的坐标是其他坐标系的，可以通过扩展坐标系的方法扩展本项目。如果你有意愿对本项目进行扩展，请在github中Fork代码，并将修改发至本项目

1. 在**Trans.h**的枚举类**GEODETIC_COORD_SYSTEM**中添加新的大地坐标系变量

   ```c++
   /**
    * 大地坐标系枚举类
   */
   enum GEODETIC_COORD_SYSTEM
   {
       XIAN80,
       WGS84
   };
   ```

2. 在**Trans.h**的**EllipsoidParamArray**数组中添加新大地坐标系对应椭球体的名称和参数（a,b,f）

   ```c++
   /**
   * 各个椭球体参数的数组 （大地坐标系名称，a,b,f）
   */
   static EllipsoidParam EllipsoidParamArray[] =
   {
   	{ "XIAN80", 6378140, 6356755.2882, 1 / 298.257 },//名称、长半轴、短半轴、扁率
   	{ "WGS84", 6378137, 6356752.314, 1 / 298.2572236 }
   };
   ```

   **注意**：步骤1和2添加的变量顺序要对应

