#include<lpc21xx.h>
#include<stdio.h>

void delay(unsigned int x)
{
   unsigned int del;
   for(del=0;del<x;del++);
}