
#include "readout.h"
#include "cvplot/cvplot.h"
#include <opencv2/opencv.hpp>
#include <librobosub/timeutil.h>

struct Vec3{
	double x, y, z;
};

struct Mat33{
	double
		a11, a12, a13,
		a21, a22, a23,
		a31, a32, a33
	;
	
	Vec3 multiplyVector(Vec3 v){
		Vec3 o;
		o.x = v.x*a11 + v.y*a12 + v.z*a13;
		o.y = v.x*a21 + v.y*a22 + v.z*a23;
		o.z = v.x*a31 + v.y*a32 + v.z*a33;
		return o;
	}
	
	Vec3 fromEuler(Vec3 e){
		a11 =  cos(e.z)*cos(e.x) - cos(e.y)*sin(e.x)*sin(e.z);
		a12 =  cos(e.z)*sin(e.x) + cos(e.y)*cos(e.x)*sin(e.z);
		a13 =  sin(e.z)*sin(e.y);
		a21 = -sin(e.z)*cos(e.x) - cos(e.y)*sin(e.x)*cos(e.z);
		a22 = -sin(e.z)*sin(e.x) + cos(e.y)*cos(e.x)*cos(e.z);
		a23 =  cos(e.z)*sin(e.y);
		a31 =  sin(e.y)*sin(e.x);
		a32 = -sin(e.y)*cos(e.x);
		a33 =  cos(e.y);
	}
};

struct ContinuousPlotData{
	float* dataPoints;
	int queuePos;
	int queueSize;
	
	ContinuousPlotData(int size){
		dataPoints = new float[size];
		queuePos = 0;
		queueSize = size;
		
		for(int i=0; i<size; i++){
			dataPoints[i] = 0;
		}
	}
	~ContinuousPlotData(){
		delete(dataPoints);
	}
	
	void addPoint(float val){
		dataPoints[queuePos] = val;
		queuePos = (queuePos+1)%queueSize;
	}
	
	void putIntoSeries(cvplot::Series& series){
		series.clear();
		for(int i=0; i<queueSize; i++){
			series.addValue(dataPoints[(queuePos+i)%queueSize]);
		}
	}
};

struct ContinuousPlotSeries{
	ContinuousPlotData* data;
	const char* seriesName;
};

struct ContinuousPlotGraphic{
	ContinuousPlotSeries* series;
	int numSeries;
	
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	
	const char* figureName;
	const char* title;
	
	ContinuousPlotGraphic(const char* figureName_, const char* title_, int numSeries_, float xpos, float ypos, float xsize, float ysize, float xmin_, float xmax_, float ymin_, float ymax_){
		numSeries = 0;
		figureName = figureName_;
		title = title_;
		
		xmin = xmin_;
		xmax = xmax_;
		ymin = ymin_;
		ymax = ymax_;
		
		cvplot::setWindowTitle(figureName, title);
		cvplot::moveWindow    (figureName, xpos, ypos);
		cvplot::resizeWindow  (figureName, xsize, ysize);
		
		series = new ContinuousPlotSeries[numSeries_];
	}
	
	void addSeries(const char* seriesName_, ContinuousPlotData& data){
		series[numSeries].data = &data;
		series[numSeries].seriesName = seriesName_;
		numSeries++;
	}
	
	void show(){
		for(int i=0; i<numSeries; i++){
			series[i].data->putIntoSeries(cvplot::figure(figureName).series(series[i].seriesName));
		}
		cvplot::figure(figureName).showManualBounds(true, xmin, xmax, ymin, ymax);
	}
};

void readout(ReadoutData* data){
	
	int pointsToGraph = 200;
	int graphHeight = 200;
	int graphWidth = 600;
	
	int curGraphY = -graphHeight;
	
	//params: identifier title numSeries xpos ypos xsize ysize xmin xmax ymin ymax
	ContinuousPlotGraphic rtt("rtt", "Robot RTT", 1, 0, curGraphY+=graphHeight, graphWidth, graphHeight, 0, pointsToGraph, 0, 10);
	ContinuousPlotData rtt_rtt(pointsToGraph); rtt.addSeries("RTT ms", rtt_rtt);
	
	ContinuousPlotGraphic cpu("cpu", "Robot Resource Usage", 1, 0, curGraphY+=graphHeight, graphWidth, graphHeight, 0, pointsToGraph, 0, 100);
	ContinuousPlotData cpu_cpu(pointsToGraph); cpu.addSeries("CPU %"   , cpu_cpu);
	ContinuousPlotData cpu_ram(pointsToGraph); cpu.addSeries("Memory %", cpu_ram);
	
	ContinuousPlotGraphic accel("accel", "Acceleration", 3, 0, curGraphY+=graphHeight, graphWidth, graphHeight, 0, pointsToGraph, -2, 2);
	ContinuousPlotData accel_x(pointsToGraph); accel.addSeries("X G", accel_x);
	ContinuousPlotData accel_y(pointsToGraph); accel.addSeries("Y G", accel_y);
	ContinuousPlotData accel_z(pointsToGraph); accel.addSeries("Z G", accel_z);
	
	ContinuousPlotGraphic stuff("stuff", "Stuff", 1, 0, curGraphY+=graphHeight, graphWidth, graphHeight, 0, pointsToGraph, 0, 256);
	ContinuousPlotData stuff_stuff(pointsToGraph); stuff.addSeries("Stuff", stuff_stuff);
	
	unsigned char i;
	
	while(true){
		if(data->valid){
			rtt_rtt.addPoint(data->rtt);
			
			cpu_cpu.addPoint(data->cpu);
			cpu_ram.addPoint(data->ram);
			
			accel_x.addPoint(data->accel_x);
			accel_y.addPoint(data->accel_y);
			accel_z.addPoint(data->accel_z);
		}
		
		stuff_stuff.addPoint(i+=5);
		
		rtt.show();
		cpu.show();
		accel.show();
		stuff.show();
		
		robosub::Time::waitMillis(10);
	}
}
