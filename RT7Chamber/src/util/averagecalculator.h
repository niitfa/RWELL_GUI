#ifndef AVERAGECALCULATOR_H
#define AVERAGECALCULATOR_H

class AverageCalculator
{
    int size;
    double average;
public:
    AverageCalculator();
    void add(double);
    void reset();
    double getAverage();
    int getSize();
};

#endif // AVERAGECALCULATOR_H
