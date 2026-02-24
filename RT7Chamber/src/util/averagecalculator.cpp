#include "averagecalculator.h"

AverageCalculator::AverageCalculator()
{
    this->reset();
}

void AverageCalculator::add(double val)
{
    this->size++;
    this->average = (this->average * (size - 1) + val) / size;
}

void AverageCalculator::reset()
{
    this->size = 0;
    this->average = 0;
}

double AverageCalculator::getAverage()
{
    return this->average;
}

int AverageCalculator::getSize()
{
    return this->size;
}
