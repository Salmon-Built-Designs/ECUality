#pragma once

#include <Arduino.h>
#include "Parameter.h"
#include "Scale.h"
#include "Map.h"
#include "FuelTweaker.h"
#include "MovingAverage.h"


// operating control variables
char good_ee_loads;
uint8_t auto_stat;
unsigned char run_condition;
uint8_t status_mode;

// task variables
unsigned int ms_freq_of_task[20];
unsigned int task_runtime[20];
unsigned int ms_since_last_task[20] = { 0 };
void(*task[20]) (void);
uint8_t n_tasks;
uint8_t i_autoreport; 

// sensor input variables
int air_flow, rpm, o2, air_temp, coolant_temp, oil_pressure;
unsigned int tach_period, inj_duration;
MovingAverage avg_rpm(4);

// variables that capture dynamic aspects of sensor input
int air_flow_d, air_flow_snap, o2_d;

// fault counters
unsigned int n_rpm_hi_faults, n_rpm_lo_faults, n_air_faults;

// EE-Backed Parameters //////////////////////////////////

Parameter global_offset("glo", -1000, 1000);
Parameter coasting_rpm("crp", 800, 2500);
Parameter idling_rpm("irp", 400, 2200);
Parameter air_thresh("ath", 50, 125);
Parameter accel_stabilize_rate("acc", 5, 500);		// the rate at which accelerator pump transient decays.
Parameter cold_threshold("cld", 80, 190);			// the temperature below which enrichment kicks in.  (100F to 150F)
Parameter cranking_dur("cra", 800, 3000);			// the injector duration while cranking not counting choke (500 - 2000) 
Parameter idle_dur("idl", 500, 2000);					// the injector duration while idling. 
Scale choke_scale("cho", -40, 200, 0, 2000, 0);		// scales engine temperature to "per 1024" (think percent) increase of injector itme
Scale temp_scale("tem", 0, 1023, -40, 250, 12);	// scales measured voltage on temp sensors to actual temp. in F
Scale air_rpm_scale("gri", 0, 255, 200, 6000, 8);	// the x and y gridlines (air-flow and rpm) for the injector map.  (not a scaling function) 
Map inj_map("inj_map", "???", &air_rpm_scale, 8, 400, 3000);	// the 2d map defining injector durations with respect to air-flow and rpm
Map offset_map("offset_map", "???", &air_rpm_scale, 8, -1500, 1500); // local modifications to the inj_map, applied by the optimizer. 
Map change_map("change_map", "???", &air_rpm_scale, 8, -1500, 1500);	// map that contains the changes made since power-up. 

Parameter idle_slope("ids", 200, 4000);	// a slope that determines how fast the injector duration increases with decreasing rpm at idle.

// Added 3-5-2016 
// * appending to end so I don't have to change the formatting of the EEPROM
Parameter idle_offset("iof", -1000, 1000);		// Just like global offset, but only used during idle conditions
Scale idle_scale("isc", 0, 2200, 300, 1100, 2);		// Linear "map" RPM v inj.Dur at idle.  Higher dur at lower RPM. 

FuelTweaker boss(run_condition, air_flow, rpm, avg_rpm.average, o2,
	global_offset.value, offset_map, change_map);				// presently contains 10 parameters

void NameParams() {
	global_offset.setName(F("global_offset"));
	coasting_rpm.setName(F("coasting_rpm"));
	idling_rpm.setName(F("idling_rpm"));
	air_thresh.setName(F("air_thresh"));
	accel_stabilize_rate.setName(F("accel_stab_rate"));
	cold_threshold.setName(F("cold_threshold"));
	cranking_dur.setName(F("cranking_dur"));
	idle_dur.setName(F("idle_dur"));

	idle_slope.setName(F("idle_slope"));
	idle_offset.setName(F("idle_offset"));

	// Scales
	choke_scale.setName(F("choke_scale"));
	temp_scale.setName(F("temp_scale"));
	air_rpm_scale.setName(F("air_rpm_grid"));
	idle_scale.setName(F("idle_scale"));
}