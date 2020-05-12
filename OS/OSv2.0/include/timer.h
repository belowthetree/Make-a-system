#ifndef TIMER_H
#define TIMER_H

typedef struct{
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
	int century;
} TIME;

TIME time;

void InitTimer();

























#endif