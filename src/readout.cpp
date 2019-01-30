
#include "readout.h"
#include "cvplot/cvplot.h"
#include <opencv2/opencv.hpp>
#include <librobosub/timeutil.h>

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
	
	int graphwidth = 200;
	
	//params: identifier title numSeries xpos ypos xsize ysize xmin xmax ymin ymax
	ContinuousPlotGraphic rtt("rtt", "Robot RTT (ms)", 1, 0, 0, 600, 300, 0, graphwidth, 0, 10);
	ContinuousPlotData rtt_rtt(graphwidth); rtt.addSeries("RTT", rtt_rtt);
	
	ContinuousPlotGraphic cpu("cpu", "Robot Resource Usage", 1, 0, 300, 600, 300, 0, graphwidth, 0, 100);
	ContinuousPlotData cpu_cpu(graphwidth); cpu.addSeries("CPU"   , cpu_cpu);
	ContinuousPlotData cpu_ram(graphwidth); cpu.addSeries("Memory", cpu_ram);
	
	ContinuousPlotGraphic accel("accel", "Acceleration", 3, 0, 600, 600, 300, 0, graphwidth, -2, 2);
	ContinuousPlotData accel_x(graphwidth); accel.addSeries("X", accel_x);
	ContinuousPlotData accel_y(graphwidth); accel.addSeries("Y", accel_y);
	ContinuousPlotData accel_z(graphwidth); accel.addSeries("Z", accel_z);
	
	while(true){
		if(data->valid){
			rtt_rtt.addPoint(data->rtt);
			
			cpu_cpu.addPoint(data->cpu);
			cpu_ram.addPoint(data->ram);
			
			accel_x.addPoint(data->accel_x);
			accel_y.addPoint(data->accel_y);
			accel_z.addPoint(data->accel_z);
		}
		
		rtt.show();
		cpu.show();
		accel.show();
		
		robosub::Time::waitMillis(10);
	}
}
