#include "Trans.h"
using namespace coord;

Trans::Trans(double dx, double dy, double dz, double Ox, double Oy, double Oz, double k,
	GEODETIC_COORD_SYSTEM _coordSystem_origin, GEODETIC_COORD_SYSTEM _coordSystem_result)
	:dx(dx), dy(dy), dz(dz), Ox(toRadians(Ox / 3600)), Oy(toRadians(Oy / 3600)), Oz(toRadians(Oz / 3600)), k(k / 1000000),
	coordSystem_origin(_coordSystem_origin), coordSystem_result(_coordSystem_result)
{
}

double Trans::toRadians(double angdeg){
	return angdeg / 180.0 * M_PI;
}

double Trans::toDegrees(double angrad)
{
	return angrad * 180.0 / M_PI;
}

Point Trans::geodetic2spatial(Point pt, double a, double b)
{
	double ee = (a * a - b * b) / (a * a);
	double L = toRadians(pt.x), B = toRadians(pt.y), H = pt.z;
	double N = a / sqrt(1 - ee * sin(B) * sin(B));
	Point point;
	point.x = (N + H) * cos(B) * cos(L);
	point.y = (N + H) * cos(B) * sin(L);
	point.z = (N * (1 - ee) + H) * sin(B);
	return point;
}

Point Trans::qicanshu(Point point)
{
	Point pointResult;
	pointResult.x = dx + point.x * (1 + k) + Oz * point.y - Oy * point.z;
	pointResult.y = dy + point.y * (1 + k) - Oz * point.x + Ox * point.z;
	pointResult.z = dz + point.z * (1 + k) + Oy * point.x - Ox * point.y;
	return pointResult;
}

Point Trans::spatial2geodetic(Point pt, double a, double f)
{
	double X = pt.x, Y = pt.y, Z = pt.z;
	//double f = (a - b)/ a;
	double e2 = 2 * f - f * f; //e^2;
	double L = toDegrees(atan(Y / X) + M_PI);
	double B2 = atan(Z / sqrt(X * X + Y * Y));
	double B1;
	double N;
	while (true) {
		N = a / sqrt(1 - f * (2 - f) * sin(B2) * sin(B2));
		B1 = atan((Z + N * f * (2 - f) * sin(B2)) / sqrt(X * X + Y * Y));
		if (fabs(B1 - B2) < 0.0000000001)
			break;
		B2 = B1;
	}
	double H = Z / sin(B2) - N * (1 - e2);
	double B = toDegrees(B2);
	Point pointResult;
	pointResult.x = L;
	pointResult.y = B;
	pointResult.z = H;
	return pointResult;
}

Point Trans::gaussPrj_geodetic2plane(Point point, double a, double b,double f, int ZoneWide, int Lo)
{
	//计算带号和中央经线经度
	int ProjNo = 0;
	if (Lo == DEFFAULT_LONTITUDE)//如果没有输入中央经线
	{
		ProjNo = ((int)point.x) / 6 + 1;//6度带带号
		Lo = (ProjNo)* ZoneWide - ZoneWide / 2;//中央经线
	}
	else{//用户输入了中央经线
		ProjNo = (Lo + ZoneWide / 2) / ZoneWide;
	}
	double longitude1, latitude1, longitude0, X0, Y0, xval, yval;
	double  e2, ee, NN, T, C, A, M, iPI;
	
	longitude0 = toRadians(Lo);
	longitude1 = toRadians(point.x);
	latitude1 = toRadians(point.y);
	
	e2 = 2 * f - f * f;
	ee = e2 * (1.0 - e2);
	NN = a / sqrt(1.0 - e2 * sin(latitude1) * sin(latitude1));
	T = tan(latitude1) * tan(latitude1);
	C = ee * cos(latitude1) * cos(latitude1);
	A = (longitude1 - longitude0) * cos(latitude1);
	M = a * ((1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256) * latitude1 - (3 * e2 / 8 + 3 * e2 * e2 / 32 + 45 * e2 * e2 * e2 / 1024) * sin(2 * latitude1) + (15 * e2 * e2 / 256 + 45 * e2 * e2 * e2 / 1024) * sin(4 * latitude1) - (35 * e2 * e2 * e2 / 3072) * sin(6 * latitude1));
	xval = NN * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * ee) * A * A * A * A * A / 120);
	yval = M + NN * tan(latitude1) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24 + (61 - 58 * T + T * T + 600 * C - 330 * ee) * A * A * A * A * A * A / 720);

	Point pointResult;
	pointResult.x = xval + 1000000 * ProjNo + 500000;
	pointResult.y = yval;
	pointResult.z = point.z;
	return pointResult;

}

//Point Trans::gaussPrj_geodetic2plane(Point point, double a, double b, double f,int ZoneWide, int Lo)
//{
//	//计算中央经线和带号
//	int ProjNo = 0;
//	if (Lo == DEFFAULT_LONTITUDE)//没有输入中央经线
//	{
//		ProjNo = ((int)point.x) / 6 + 1;//六度带带号
//		Lo = (ProjNo)* ZoneWide - ZoneWide / 2;//中央经线
//	}
//	else{//用户输入中央经线
//		ProjNo = (Lo + ZoneWide / 2) / ZoneWide;
//	}
//
//	double L = toRadians(point.x), B = toRadians(point.y);
//
//
//	//辅助量
//	double cosB = cos(B);
//	double sinB = sin(B);
//	double cosB_2 = cosB * cosB;
//	double l = L - toRadians(Lo);
//	double ll = l * l;
//
//	//计算系数
//	double t = tan(B);
//	double e1 = sqrt((a * a - b * b) / (b * b));
//	double eta = e1 * cos(B);
//	double c = a * a / b;
//	double N = c / sqrt(1 + eta * eta);
//
//	double a0 = 32144.5189 - (135.3646 - (0.7034 - 0.0041 * cosB_2) * cosB_2) * cosB_2;
//	double a4 = (0.25 + 0.00253 * cosB_2) * cosB_2 - 0.04167;
//	double a6 = (0.167 * cosB_2 - 0.083) * cosB_2;
//	double a3 = (0.3333333 + 0.001123 * cosB_2) * cosB_2 - 0.1666667;
//	double a5 = 0.00878 - (0.1702 - 0.20382 * cosB_2) * cosB_2;
//
//	//计算高斯平面坐标
//	double x = 6367452.1328 * B - (a0 - (0.5 + (a4 + a6 * ll) * ll) * ll * N) * cosB * sinB;
//	double y = (1 + (a3 + a5 * ll) * ll) * l * N * cosB;
//
//	Point pointResult;
//	pointResult.x = y + 1000000 * ProjNo + 500000;
//	pointResult.y = x;
//  pointResult.z = point.z;
//	return pointResult;
//}

Point Trans::gaussInversePrj_plane2geodetic(Point point, double a, double f, int ZoneWide, int Lo)
{
	double X = point.x;
	double Y = point.y;
	double _a = a;
	//double _f = (a-b)/a;
	double _f = f;
	int ProjNo = 0;
	double longitude1, latitude1, longitude0, latitude0, X0, Y0, xval, yval;
	double e1, e2, ee, NN, T, C, M, D, R, u, fai, iPI;
	iPI = 0.0174532925199433; //3.1415926535898/180.0;

	if (Lo == DEFFAULT_LONTITUDE)//未输入中央经线
	{
		ProjNo = (int)(X / 1000000L); //带号
		longitude0 = (ProjNo)* ZoneWide - ZoneWide / 2;//中央经线
	}
	else{//用户输入了中央经线
		ProjNo = (Lo + ZoneWide / 2) / ZoneWide;
		longitude0 = Lo;
	}


	longitude0 = longitude0 * iPI; //中央经线

	X0 = ProjNo * 1000000L + 500000L;
	Y0 = 0;
	xval = X - X0;
	yval = Y - Y0;
	e2 = 2 * _f - _f * _f;
	e1 = (1.0 - sqrt(1 - e2)) / (1.0 + sqrt(1 - e2));
	ee = e2 / (1 - e2);
	M = yval;
	u = M / (_a * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));
	fai = u + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * u) + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * u)
		+ (151 * e1 * e1 * e1 / 96) * sin(6 * u) + (1097 * e1 * e1 * e1 * e1 / 512) * sin(8 * u);
	C = ee * cos(fai) * cos(fai);
	T = tan(fai) * tan(fai);
	NN = _a / sqrt(1.0 - e2 * sin(fai) * sin(fai));
	R = _a * (1 - e2) / sqrt((1 - e2 * sin(fai) * sin(fai)) * (1 - e2 * sin(fai) * sin(fai)) * (1 - e2 * sin(fai) * sin(fai)));
	D = xval / NN;
	//计算经度(Longitude)纬度(Latitude)
	longitude1 = longitude0 + (D - (1 + 2 * T + C) * D * D * D / 6 + (5 - 2 * C + 28 * T - 3 * C * C + 8 * ee + 24 * T * T) * D * D * D * D * D / 120) / cos(fai);
	latitude1 = fai - (NN * tan(fai) / R) * (D * D / 2 - (5 + 3 * T + 10 * C - 4 * C * C - 9 * ee) * D * D * D * D / 24 + (61 + 90 * T + 298 * C + 45 * T * T - 256 * ee - 3 * C * C) * D * D * D * D * D * D / 720);
	//转换为度 DD
	Point resultPt;
	resultPt.x = (longitude1 / iPI);
	resultPt.y = (latitude1 / iPI);
	resultPt.z = point.z;
	return resultPt;
}

Point Trans::geodetic2plane(Point point, double a_origin, double b_origin, double a_result, double b_result, double f_result, int ZoneWide, int Lo)
{
	return gaussPrj_geodetic2plane(spatial2geodetic(qicanshu(geodetic2spatial(point, a_origin, b_origin)), a_result, f_result), a_result, b_result,f_result,6);

}

Point Trans::plane2geodetic(Point point, double a_origin, double b_origin, double f_origin, double a_result, double f_result, int ZoneWide, int Lo)
{
	return spatial2geodetic(qicanshu(geodetic2spatial(gaussInversePrj_plane2geodetic(point, a_origin, f_origin, 6), a_origin, b_origin)), a_result, f_result);
}

Point Trans::geodetic2plane(Point point, int ZoneWide, int Lo)
{
	EllipsoidParam ellipsoidParam_origin = EllipsoidParamArray[coordSystem_origin];
	EllipsoidParam ellipsoidParam_result = EllipsoidParamArray[coordSystem_result];
	return geodetic2plane(point, ellipsoidParam_origin.a, ellipsoidParam_origin.b, ellipsoidParam_result.a, ellipsoidParam_result.b,
		ellipsoidParam_result.f, ZoneWide, Lo);
}

Point Trans::plane2geodetic(Point point, int ZoneWide, int Lo)
{
	EllipsoidParam ellipsoidParam_origin = EllipsoidParamArray[coordSystem_origin];
	EllipsoidParam ellipsoidParam_result = EllipsoidParamArray[coordSystem_result];
	return plane2geodetic(point, ellipsoidParam_origin.a, ellipsoidParam_origin.b, ellipsoidParam_origin.f, ellipsoidParam_result.a, ellipsoidParam_result.f,
		ZoneWide, Lo);
}

Trans::~Trans()
{
}
