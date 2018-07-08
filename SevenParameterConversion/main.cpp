#include "Trans.h"
#include <iostream>
#include <string>
using namespace std;
using namespace coord;

int main(){

	//这里的七参数随便写的，改成你自己的七参数
	Trans trans(-8.5, 28, 14.8888, 0.888, -1.9999, 3.1111, -4.22222,coord::WGS84,coord::XIAN80);
	Point pt(108.635229183, 26.111425709, 0);
	Point result = trans.geodetic2plane(pt, 6378137, 6356752.314, 6378140, 6356755.2882, 1 / 298.257, 6);
	Point result_copy = trans.geodetic2plane(Point(108.635229183, 26.111425709, 0), 6);

	Point pt1(19348965.21, 2894958.496, 0);
	Trans trans2(8.5, -28, -14.8888, -0.888, 1.9999, -3.1111, 4.22222, coord::XIAN80, coord::WGS84);
	Point result1 = trans2.plane2geodetic(pt1, 6378140, 6356755.2882, 1 / 298.257, 6378137, 1 / 298.2572236, 6);
	Point result1_copy = trans2.plane2geodetic(Point(19348965.21, 2894958.496, 0), 6);
	
	cout.precision(12);
	cout.setf(ios::fixed);

	cout << "输入" << EllipsoidParamArray[trans.getOriginCoordSystem()].name <<"坐标："<<
	pt.x << "，" << pt.y << "，" << pt.z << endl;
	cout << "经七参数转换" << endl;
	cout << "输出" << EllipsoidParamArray[trans.getResultCoordSystem()].name << "坐标：" <<
		result.x << "，" << result.y << "，" << result.z << endl << endl;
	
	cout << "输入" << EllipsoidParamArray[trans2.getOriginCoordSystem()].name << "坐标：" <<
		pt1.x << "，" << pt1.y << "，" << pt1.z << endl ;
	cout << "经七参数转换" << endl;
	cout << "输出" << EllipsoidParamArray[trans2.getResultCoordSystem()].name << "坐标：" <<
		result1.x << "，" << result1.y << "，" << result1.z << endl << endl;

	system("pause");

}