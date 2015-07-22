/*
 * BasicDefinitions.h
 *
 *  Created on: 21-06-2015
 *      Author: bsp
 */

#ifndef BASICDEFINITIONS_H_
#define BASICDEFINITIONS_H_

typedef long int TickT;

template<class EnumType>
int enumValue(EnumType e) {
  return static_cast<int>(e);
}

#endif /* BASICDEFINITIONS_H_ */
